#version 330
layout(location=0) in vec3 Pos; 

uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 
uniform mat4 ModelMatrix; 

void main(void) {
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Pos,1.0); 
}