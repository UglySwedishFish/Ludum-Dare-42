#version 330
in vec2 TexCoord; 
out vec3 Color; 

uniform sampler2D Image; 

void main() {
	Color = texture(Image,TexCoord).rgb; 

	if(Color == vec3(0.))
		discard; 

}