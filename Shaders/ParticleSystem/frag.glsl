#version 330

out vec3 Albedo;

uniform sampler2DArray Animation; 

in vec3 TexCoord; 


void main() {
	
	

	vec4 RawSample = texture(Animation, vec3(TexCoord.x,1.-TexCoord.y,TexCoord.z)); 

	if(RawSample.a < .5)
		discard; 

	Albedo = RawSample.xyz; 
}