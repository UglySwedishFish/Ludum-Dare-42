#version 330

layout(location = 0) out vec4 Albedo; 
layout(location = 1) out vec4 LightMapRoughness; 
layout(location = 2) out vec4 NormalMetalness; 
layout(location = 3) out vec4 PositionEmmisive; 

in vec3 Normals; 
in vec3 Position; 
flat in int Material; 
in vec2 TexCoords; 

uniform sampler1D Materials; 
uniform sampler2D LightMap; 
uniform bool HasLightMap; 

void main() {
	vec3 MaterialProperties = texelFetch(Materials, Material*2+1, 0).xyz; 

	Albedo.xyz = texelFetch(Materials,Material*2,0).xyz; 
	LightMapRoughness = vec4(HasLightMap ? texture(LightMap,TexCoords).xyz : vec3(1.),MaterialProperties.x); 
	NormalMetalness = vec4(Normals, MaterialProperties.y); 
	PositionEmmisive = vec4(Position, MaterialProperties.z); 
}