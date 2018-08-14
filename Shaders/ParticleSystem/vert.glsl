#version 330
layout(location=0) in vec3 Pos; 
layout(location=1) in vec2 Texc; 

uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix;
uniform sampler1D Positions; 
uniform float Scale; 

out vec3 TexCoord; 

mat4 ModelMatrixFromPosition(vec3 Position) {
	mat4 Matrix; 

	for(int x=0;x<3;x++) {
		for(int y=0;y<3;y++) {
			Matrix[x][y] = ViewMatrix[y][x]; 
		}
	}

	Matrix[3] = vec4(Position, 1.); 
	
	return Matrix; 
}

void main(void) {	

	vec4 Position = texelFetch(Positions, gl_InstanceID, 0); 

	mat4 ModelMatrix = ModelMatrixFromPosition(Position.xyz); 

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Pos * Scale,1.0); 

	TexCoord = vec3(Texc, floor(Position.a*255.f+.1)); 

}