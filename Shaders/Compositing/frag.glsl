#version 330
in vec2 TexCoord; 
out vec3 Color; 

uniform sampler2D Lighting; 
uniform sampler2D UI; 

void main() {

	vec3 Result = vec3(1.0) - exp(-texture(Lighting, TexCoord).xyz * 0.65);

	Color = pow(Result,vec3(.454545)); 

	vec3 UI = texture(UI, TexCoord).xyz; 
	Color = (UI == vec3(0.)) ? Color : UI; 

}