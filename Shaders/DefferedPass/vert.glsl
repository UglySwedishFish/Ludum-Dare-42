#version 330
layout(location=0) in vec3 Pos; 
layout(location=1) in vec3 TexCoord; 
layout(location=2) in vec3 Normal; 
layout(location=3) in vec3 Tangent; 

uniform mat4 ViewMatrix; 
uniform mat4 ModelMatrix; 
uniform mat4 ProjectionMatrix;

out vec3 Normals; 
out vec3 Position; 
out int Material; 
out vec2 TexCoords; 

void main(void) {	
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Pos,1.0); 
	Position = vec3(ModelMatrix * vec4(Pos,1.)); 
	Normals = transpose(inverse(mat3(ModelMatrix))) * Normal; 
	Material = int(floor(TexCoord.z*100.f+.1f)); 
	TexCoords = TexCoord.xy; 
}