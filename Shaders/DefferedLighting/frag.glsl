#version 420

in vec2 TexCoord; 
layout(location = 0) out vec3 Color; 
layout(location = 1) out vec3 ViewPosition; 

uniform mat4 DirectionalViewMatrix; 
uniform mat4 DirectionalProjectionMatrix; 

uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 

uniform vec3 CameraPosition; 
uniform sampler2D Albedo; 
uniform sampler2D LightMap; 
uniform sampler2D Normal; 
uniform sampler2D Position; 
uniform sampler2DShadow DirectionalShadowMap; 
uniform sampler2D Particles; 
uniform sampler2D ParticleDepth; 
uniform sampler2D Depth; 

float seed; 

//2-component rand
float rand() {
    return fract(sin(seed+=.1)*43758.5453123);
}

const vec3 SUNYELLOW = vec3(1.0, .721, 0.075); 
const vec3 SKYBLUE = vec3(0.0,0.5,1.0); 




//simple, directional shadows using something similar to stratified possion disk sampling, although not exactly the same
vec3 GetDirectionalShadowColor(vec3 WorldPos, int Samples, float Offset) { 

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

		Result += texture(DirectionalShadowMap, vec3(ScreenSpace.xy + Direction * .001, ScreenSpace.z - Offset)).xxx; 

	}

	return Result * Factor * SUNYELLOW; 
}


vec3 GetParticleWorldPosition(float ParticleDepth) {
	float Z = ParticleDepth * 2.0 - 1.0;

    vec4 ClipSpace = vec4(TexCoord * 2.0 - 1.0, Z, 1.0);
    vec4 ViewSpace = inverse(ProjectionMatrix) * ClipSpace;

    // Perspective division
    ViewSpace /= ViewSpace.w;

	return vec3(vec4(ViewSpace.xyz,1.0)*inverse(transpose(ViewMatrix)));
}



void main() {



	seed = ((TexCoord.x * TexCoord.y)*1000.0); 

	
	
	float DepthAlbedo = texture(Depth, TexCoord).x; 
	float DepthParticles = texture(ParticleDepth, TexCoord).x; 

	vec3 WorldPosition = DepthAlbedo < DepthParticles ? texture(Position, TexCoord).xyz : GetParticleWorldPosition(DepthParticles); 

	vec3 DiffuseShadows = GetDirectionalShadowColor(WorldPosition, 10, DepthAlbedo < DepthParticles ? 0.005 : 0.05); 

	if(DepthAlbedo > DepthParticles) {
		Color = texture(Particles, TexCoord).xyz * max(DiffuseShadows, mix(SKYBLUE,vec3(1.),.5)*0.35); 
	}
	else {
		vec3 Normal = texture(Normal, TexCoord).xyz; 
		vec3 Alb = texture(Albedo, TexCoord).xyz; 
		vec3 Indirect = texture(LightMap, TexCoord).xyz; 
		Color = Alb * (Indirect + DiffuseShadows); 
	}

	vec4 ViewPos = ViewMatrix * vec4(WorldPosition,1.); 

	ViewPosition = ViewPos.xyz; 

}