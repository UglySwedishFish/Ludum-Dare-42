#version 420
in vec2 TexCoord; 
out vec3 Color; 

uniform sampler2D CurrentLighting; 
uniform sampler2D ViewPosition; 
uniform sampler2D Normals; 
uniform sampler2D ParticleDepth; 
uniform sampler2D AlbedoDepth; 
uniform sampler2D RefractiveAlbedo; 
uniform sampler2D RefractiveNormal; 
uniform sampler2D RefractiveWorldPosition; 
uniform sampler2D RefractiveDepth; 

uniform samplerCubeArray ReflectionProbes; 
uniform samplerCubeArray ReflectionProbesWorldPosition; 
uniform sampler1D ReflectionProbePositions; 


uniform vec3 CameraPosition; 


uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 

const int Steps = 32; 
const int BinarySteps = 8; 
const float StepSize = 0.5;
const float IncreaseFactor = 1.1; 
const float Estimation = .5; 

vec3 SpecularTrace(int CubeMap, vec3 CubeMapPos, vec3 WorldPos, vec3 Normal) {
	vec3 IncidentVector = normalize(WorldPos - CameraPosition);
	vec3 Direction = reflect(IncidentVector, Normal);

	vec3 Hit = WorldPos + Normal;

	float Traversal = 0.;

	float CurrentStepSize = StepSize; 

	for (int Step = 1; Step < Steps; Step++) {

		
		Hit += Direction * CurrentStepSize;
		

		CurrentStepSize *= IncreaseFactor;

		Traversal += CurrentStepSize;

		vec3 PositionOnCubeMap = texture(ReflectionProbesWorldPosition, vec4(normalize(Hit - CubeMapPos), CubeMap)).xyz;

		//are we close enough to estimate a hit? 

		if (distance(PositionOnCubeMap, Hit) < (CurrentStepSize * Estimation)) {

			float StepSizeBinarySearch = CurrentStepSize * 0.5;

			for (int StepBinarySearch = 0; StepBinarySearch<BinarySteps; StepBinarySearch++) {

				PositionOnCubeMap = texture(ReflectionProbesWorldPosition, vec4(normalize(Hit - CubeMapPos), CubeMap)).xyz;

				if (distance(PositionOnCubeMap, Hit) < (CurrentStepSize * Estimation)) {
					Traversal += StepSizeBinarySearch;
					Hit -= Direction * StepSizeBinarySearch;
				}
				else {
					Traversal -= StepSizeBinarySearch;
					Hit += Direction * StepSizeBinarySearch;
				}

				StepSizeBinarySearch *= 0.5;
			}

			PositionOnCubeMap = texture(ReflectionProbesWorldPosition, vec4(normalize(Hit - CubeMapPos), CubeMap)).xyz;
			if(distance(PositionOnCubeMap, Hit) < (StepSize * 4.)) 

			return texture(ReflectionProbes, vec4(normalize(Hit - CubeMapPos), CubeMap)).xyz;

		}

		if (Step == Steps - 1) {
			return texture(ReflectionProbes, vec4(Direction, CubeMap)).xyz;
		}


	}

}

vec3 GetReflectionProbes(vec3 WorldPosition, vec3 Normal) {
	vec3 Direction = reflect(normalize(WorldPosition-CameraPosition), Normal); 

	int Count = textureSize(ReflectionProbePositions, 0); 

	float ClosestDistance = -1.f; 
	int CubeMap = 0; 
	vec3 ClosestPosition; 

	for(int i=0;i<Count;i++) {
		
		vec3 Point = texelFetch(ReflectionProbePositions, i, 0).xyz; 

		float Distance = dot(Point-WorldPosition,Point-WorldPosition); 

		if(Distance < ClosestDistance || ClosestDistance < 0.) {
			ClosestDistance = Distance; 
			CubeMap = i; 
			ClosestPosition = Point; 
		}

	}


	return SpecularTrace(CubeMap, ClosestPosition, WorldPosition, Normal); 
}

vec2 ScreenSpaceTracing(vec3 ViewPos, vec3 Normal, bool Refractive) {
	vec3 Direction = Refractive ? refract(normalize(ViewPos), Normal, 1. / 1.33) : reflect(normalize(ViewPos), Normal); 

	Direction = Direction; 

	float StepSize = 0.75; 

	float Traversal = 0.0; 

	for(int Step = 0; Step < 16; Step++) {
		Traversal += StepSize; 
		StepSize *= 1.4; 
		vec3 RayPos = ViewPos + Direction * Traversal; 
		
		vec4 ScreenPos = ProjectionMatrix * vec4(RayPos, 1.0); 
		ScreenPos.xyz /= ScreenPos.w; 

		float Depth = textureLod(ViewPosition,ScreenPos.xy * 0.5 + 0.5,0.).z; 

		if(abs(ScreenPos.x) > 1.0 || abs(ScreenPos.y) > 1.0) 
		return vec2(-1.0); 

		if(Depth > RayPos.z) {

			float StepSizeBinary = StepSize * 0.5; 
			
			for(int StepBinarySearch = 0; StepBinarySearch < 8; StepBinarySearch++) {
				
				RayPos = ViewPos + Direction * Traversal; 

				ScreenPos = ProjectionMatrix * vec4(RayPos, 1.0); 
				ScreenPos.xyz /= ScreenPos.w; 

				Depth = textureLod(ViewPosition,ScreenPos.xy * 0.5 + 0.5,0.).z; 

				if(Depth > RayPos.z) 
				Traversal -= StepSizeBinary; 
				else
				Traversal += StepSizeBinary; 

				StepSizeBinary*=0.5; 
			}


			if(abs(Depth - RayPos.z) < StepSizeBinary * 8.0 || Refractive) 
			return ScreenPos.xy * 0.5 + 0.5; 
		}

		
	}
	return vec2(-1.); 
}



float GetFactor(vec2 TexCoord, float Power) {
	return (TexCoord.x > 0. && TexCoord.y > 0.) ? max(pow(1. - clamp(abs(dot(TexCoord * 2. - 1.,TexCoord * 2. - 1.)),0.,1.),Power),0.) : 0.; 
}


void main() {
	
	float DepthAlbedo = texture(AlbedoDepth, TexCoord).x; 
	float DepthParticles = texture(ParticleDepth, TexCoord).x; 
	float DepthRefractive = texture(RefractiveDepth, TexCoord).x;



	vec3 Lighting = texture(CurrentLighting, TexCoord).xyz; 
	vec3 Normal = texture(Normals, TexCoord).xyz; 

	vec3 ViewPos = DepthAlbedo < DepthRefractive ? texture(ViewPosition, TexCoord).xyz : vec3(ViewMatrix * vec4(texture(RefractiveWorldPosition, TexCoord).xyz, 1.)); 
	vec3 ViewNormal = DepthAlbedo < DepthRefractive ? (vec3(vec4(Normal, 1.) * inverse(ViewMatrix))) : vec3(vec4(texture(RefractiveNormal, TexCoord).xyz, 1.) * inverse(ViewMatrix)); 

	vec2 ScreenSpaceHit = ScreenSpaceTracing(ViewPos, ViewNormal, false); 

	if(DepthParticles < DepthAlbedo && DepthParticles < DepthRefractive) {
	Color = Lighting; 
	}
	else if(DepthAlbedo < DepthRefractive) {

	
	vec3 WorldPosition = vec3(vec4(ViewPos, 1.) * inverse(transpose(ViewMatrix))); 
	

	vec3 ScreenSpaceIndirect = texture(CurrentLighting, ScreenSpaceHit).xyz; 

	Color = mix(Lighting, mix(GetReflectionProbes(WorldPosition,Normal),ScreenSpaceIndirect,GetFactor(ScreenSpaceHit,1.)),.125); 
	}
	else {

		vec2 ScreenSpaceRefractiveHit = ScreenSpaceTracing(ViewPos, ViewNormal, true); 

		float Specular = GetFactor(ScreenSpaceHit,2.) * 0.25; 



		ScreenSpaceRefractiveHit = mix(TexCoord,ScreenSpaceRefractiveHit,GetFactor(ScreenSpaceRefractiveHit,3.)); 

		Color = vec3(texture(RefractiveAlbedo, TexCoord).xyz * mix(texture(CurrentLighting, ScreenSpaceRefractiveHit).xyz,texture(CurrentLighting, ScreenSpaceHit).xyz, Specular)); 


	}

	
	

}