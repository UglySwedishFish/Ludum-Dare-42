#version 440
#extension GL_ARB_bindless_texture : enable 
#extension GL_ARB_gpu_shader5 : enable

in vec2 TexCoord; 

out vec3 PathTracedResult; 

uniform sampler2DArray BaseModelData; 
uniform sampler2D PreviousFrame; 
uniform sampler2DShadow ShadowMap;
uniform mat4 DirectionalViewMatrix; 
uniform mat4 DirectionalProjectionMatrix; 

/*uniform mat4 DirectionalProjectionMatrix; 
uniform mat4 DirectionalViewMatrix;
uniform vec3 SunDirection; */


//TODO: Add comments (proper ones, not like this one), Split functions into files to make code more readable. Remove KD-Tree traversing


uniform sampler2DArray ModelData[14]; 
uniform int VerticeCount[14]; 
uniform ivec2 Resolution[14]; 
uniform int TotalModels; 

uniform int Frame; 

struct Ray {
	vec3 Direction,Origin, OneOverDirection; 
}; 

const float EPSILON = 0.0000001; 

sampler2DArray GetTexture(int Ind, sampler2DArray Array[14]) {
	return Ind == 0 ? Array[0] : 
			Ind == 1 ? Array[1] : 
			Ind == 2 ? Array[2] : 
			Ind == 3 ? Array[3] : 
			Ind == 4 ? Array[4] : 
			Ind == 5 ? Array[5] : 
			Ind == 6 ? Array[6] : 
			Ind == 7 ? Array[7] : 
			Ind == 8 ? Array[8] : 
			Ind == 9 ? Array[9] : 
			Ind == 10 ? Array[10] : 
			Ind == 11 ? Array[11] : 
			Ind == 12 ? Array[12] : Array[13]; 
}

/*
if only life was as simple as 
return Array[Ind]; 
But, as I cry in the night 
not all gpus support dynamic indicies for arrays of sampler2DArrays
and thus, a hack like this had to be implemented
*/


//simple moller-trumbore ray-triangle intersection (from wikipedia) 
bool RayTriangleIntersection(vec3 edge1, vec3 edge2, vec3 edge3, Ray ray, inout vec2 uv, inout float t,float tGlobal) {

	vec3 e0 = edge2 - edge1; 
	vec3 e1 = edge3 - edge1; 

	vec3 h = cross(ray.Direction, e1); 
	float a = dot(e0, h); 

	if(a > -EPSILON && a < EPSILON) 
		return false; 

	float f = 1.0 / a; 

	vec3 s = ray.Origin - edge1; 
	float u = f * dot(s,h); 

	if (u < 0.0 || u > 1.0)
		return false;

	vec3 q = cross(s, e0);
	float v = f * dot(ray.Direction, q);

	if (v < 0.0 || u + v > 1.0)
		return false;
	
	uv = vec2(u,v); 

	t = f * dot(e1, q);

	if (t < 0.01)
		return false;

	return (t > 0.0) && (t < tGlobal || tGlobal < 0.);
	
} 


ivec2 WrapTo2DTexture(int texel, int resolution) {
	float TexelRes = (float(texel)+.1) / float(resolution); 
	int TexelResDivider = int(floor(TexelRes)); 

	return ivec2(TexelResDivider,texel - TexelResDivider*resolution); 
}

vec3 GetTriangleData(int index,int model, int dataWanted) {
	int Y = int(floor((float(index)+.1)/float(Resolution[model].x))); 
	int XSub = Y*Resolution[model].x; 

	ivec2 texel = ivec2(index-XSub, Y); 
	
	return texelFetch(GetTexture(model,ModelData),ivec3(texel, dataWanted),0).rgb; 
}



//https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms 
//^ great site for multiple implementations and discussion on ray aabb intersection

bool RayBoxIntersection(Ray ray, vec3 Min, vec3 Max,out float t) {

	float t1 = (Min.x - ray.Origin.x)*ray.OneOverDirection.x;
	float t2 = (Max.x - ray.Origin.x)*ray.OneOverDirection.x;
	float t3 = (Min.y - ray.Origin.y)*ray.OneOverDirection.y;
	float t4 = (Max.y - ray.Origin.y)*ray.OneOverDirection.y;
	float t5 = (Min.z - ray.Origin.z)*ray.OneOverDirection.z;
	float t6 = (Max.z - ray.Origin.z)*ray.OneOverDirection.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	t = tmin; 

	return tmax > 0.f && tmax > tmin;  

}

int ToInt(float Data) {
	return floatBitsToInt(Data); 
}

float GetVec4Color(vec4 Input, int Index) {
		return 
		Index == 0 ? Input.x : 
		Index == 1 ? Input.y : 
		Index == 2 ? Input.z : 
		Input.w; 
}


const int MAX_STACK_SIZE = 100; 


//hopefully a LOT faster 
float RayModelIntersectionBVH(Ray ray, int Mesh, inout int TriangleHit, inout vec2 UV, out float IntersectionCosts) {


	TriangleHit = -1;

	int Stack[MAX_STACK_SIZE];
	int StackID = 0;
	Stack[StackID++] = 0;
	float ClosestDistance = -1.f;
	int CurrentIndex = 0;
	float Div = 1.0 / 100.;

	int Depth = 1;
	float TempTraversal = 0.;

	//what is the ratio between triangles hit and leaves hit? 
	//this is useful for debugging performance
	//the higher number the better 

	int TrianglesHit = 0;
	int LeavesHit = 0;


	while (StackID) {
		StackID--;
		int BoxID = Stack[StackID];
		vec4 Data1 = texelFetch(GetTexture(Mesh, ModelData), ivec3(WrapTo2DTexture(BoxID, Resolution[Mesh].x).yx, 4), 0);
		vec4 Data2 = texelFetch(GetTexture(Mesh, ModelData), ivec3(WrapTo2DTexture(BoxID + 1, Resolution[Mesh].x).yx, 4), 0);

		if (Data2.a < 0.) { //Branch node

							//IntersectionCosts += Div; 
			if (RayBoxIntersection(ray, Data1.xyz, Data2.xyz, TempTraversal) && (TempTraversal < ClosestDistance || ClosestDistance < 0.)) {

				Stack[StackID++] = ToInt(Data1.a);
				Stack[StackID++] = BoxID + 2;

				if (StackID > MAX_STACK_SIZE) {
					return -100.;
				}


				//if(Depth == 8) {
				//	ClosestDistance = TempTraversal; 
				//}



			}

		}
		else {
			//LEAF NODE 

			if (RayBoxIntersection(ray, Data1.xyz, Data2.xyz, TempTraversal) && (TempTraversal < ClosestDistance || ClosestDistance < 0.)) {

				int StartIndex = ToInt(Data1.a);
				int IndexSize = ToInt(Data2.a);


				for (int Index = 0; Index < IndexSize; Index++) {
					vec3 Triangle1, Triangle2, Triangle3;

					Triangle1 = GetTriangleData((StartIndex + Index) * 3, Mesh, 0);
					Triangle2 = GetTriangleData((StartIndex + Index) * 3 + 1, Mesh, 0);
					Triangle3 = GetTriangleData((StartIndex + Index) * 3 + 2, Mesh, 0);
					vec2 tuv;

					IntersectionCosts += Div;
					if (RayTriangleIntersection(Triangle1, Triangle2, Triangle3, ray, tuv, TempTraversal, ClosestDistance)) {
						TrianglesHit++;
						ClosestDistance = TempTraversal;
						TriangleHit = (StartIndex + Index) * 3;
						UV = tuv;


					}
				}
			}
		}
	}

	return ClosestDistance;

}

//modern ray-world intersection algorithm, using a SAH BVH
float RayWorldIntersectionBVH(Ray ray, inout int MeshHit, inout int TriangleHit, inout vec2 UV, out float IntersectionCosts) {
	MeshHit = -1; 
	TriangleHit = -1; 

	float TTemporary, TClosest = -1.f; 
	vec2 TemporaryUV; 
	int TTriangleHit; 

	for(int Mesh = 0; Mesh < TotalModels; Mesh++) {
		TTemporary = RayModelIntersectionBVH(ray, Mesh, TTriangleHit, TemporaryUV, IntersectionCosts); 
		if((TTemporary < TClosest || TClosest < 0.) && TTemporary > 0.) {
			TClosest = TTemporary; 
			MeshHit = Mesh; 
			TriangleHit = TTriangleHit; 
			UV = TemporaryUV; 
		}
	}
	return TClosest; 
}

float seed; 

//2-component rand
vec2 hash2() {
    return fract(sin(vec2(seed+=0.1,seed+=0.1))*vec2(43758.5453123,22578.1459123));
}

float rand() {
    return fract(sin(seed+=.1)*43758.5453123);
}



//a random hemisphere function
vec3 CosWeightedRandomHemisphereDirection( const vec3 n) {
  	vec2 rv2 = hash2();
    
	vec3  uu = normalize( cross( n, vec3(0.0,1.0,1.0) ) );
	vec3  vv = normalize( cross( uu, n ) );
	
	float ra = sqrt(rv2.y);
	float rx = ra*cos(6.2831*rv2.x); 
	float ry = ra*sin(6.2831*rv2.x);
	float rz = sqrt( 1.0-rv2.y );
	vec3  rr = vec3( rx*uu + ry*vv + rz*n );
    
    return normalize( rr );
}

float Barycentric(vec2 A, vec2 B, vec2 C) {
	return abs(A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)); 
}

const vec3 SKYBLUE = vec3(0.0,0.5,1.0); 
const vec3 SUNYELLOW = vec3(1.0, .721, 0.075); 

vec3 GetDirectionalShadowColor(vec3 WorldPos, int Samples) { 

	//find where the world position is currently on the shadowmap 
	
	//clip space, ndc space, screen space

	vec4 ClipSpace = DirectionalProjectionMatrix * DirectionalViewMatrix * vec4(WorldPos, 1.0); 

	vec3 NDCSpace = ClipSpace.xyz / ClipSpace.w; 

	vec3 ScreenSpace = NDCSpace * .5 + .5; 

	float Factor = 1.0 / float(Samples); 
	
	vec3 Result = vec3(0.); 

	for(int Sample = 0; Sample < Samples; Sample++) {

		vec2 Direction = normalize(vec2(rand(),rand())); 
		//the direction is used to add some extra fake detail to the shadow, and to make it more smooth
		//it does unfortunately also introduce some noise into the actual shadow. 
		//but, this is taken care of by both the temporal and spatial filtering
		//(that also take care of the raw path traced sample) 

		Result += texture(ShadowMap, vec3(ScreenSpace.xy + Direction * .003, ScreenSpace.z - 0.01)).xxx; 

	}

	return Result * Factor * SUNYELLOW; 
}


void main() {
	seed = ((TexCoord.x * TexCoord.y)*1000.0) + float(Frame+1); 

	//get data required for tracing 

	vec3 WorldPosition = texture(BaseModelData, vec3(TexCoord, 0.)).xyz; 
	vec3 Normal = texture(BaseModelData, vec3(TexCoord, 1.)).xyz; 

	

	if(Normal == vec3(0.) && WorldPosition == vec3(0.)) {
		PathTracedResult = vec3(0.123,0.12,0.927); 
		return; 
	}



	Ray RAY; 
	RAY.Direction = CosWeightedRandomHemisphereDirection(Normal); 
	RAY.Origin = WorldPosition;
	RAY.OneOverDirection = 1. / RAY.Direction; 

	vec3 Indirect = vec3(1.); 
	vec3 Result = vec3(0.); 
	int Mesh, Triangle; 
	float Cost;
	vec2 UV; 
	float Traversal; 
	float AO = 1.f; 

	for(int Bounce = 0; Bounce < 3; Bounce++) {
		Traversal = RayWorldIntersectionBVH(RAY, Mesh, Triangle, UV, Cost); 
		if(Triangle!=-1) {

			if(Bounce == 0) 
				AO = max(min(pow(Traversal * .2,2.),1.),.8); 
			
			float RatioP1 = Barycentric(vec2(1.0,0.0),vec2(0.0,1.0),UV); 
			float RatioP2 = Barycentric(vec2(0.0,0.0),vec2(0.0,1.0),UV); 
			float RatioP3 = Barycentric(vec2(0.0,0.0),vec2(1.0,0.0),UV);

			vec3 StoredNormals[3] = vec3[](GetTriangleData(Triangle,Mesh,1),GetTriangleData(Triangle+1,Mesh,1),GetTriangleData(Triangle+2,Mesh,1)); 
			Normal = normalize(StoredNormals[0] * RatioP1 + StoredNormals[1] * RatioP2 + StoredNormals[2] * RatioP3); 

			vec3 Color = GetTriangleData(Triangle,Mesh,3); 

			Indirect*=Color; 


			RAY.Origin = RAY.Origin + RAY.Direction * Traversal; 
			RAY.Direction = CosWeightedRandomHemisphereDirection(Normal); 
			RAY.OneOverDirection = 1. / RAY.Direction; 

			Result += Indirect * GetDirectionalShadowColor(RAY.Origin, 2); 


		}
		else {
			Result += Indirect * SKYBLUE; 
			break; 
		}
	}

	PathTracedResult = Result * AO; 
	
	PathTracedResult = mix(PathTracedResult, texture(PreviousFrame, TexCoord).xyz, float(Frame)/float(Frame+1)); 

}
