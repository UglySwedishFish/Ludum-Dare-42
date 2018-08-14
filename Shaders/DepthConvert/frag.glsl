#version 330
in vec2 TexCoord; 
out vec4 Color; 

uniform sampler2D Depth; 

const float zNear = 0.1f; 
const float zFar = 1000.f; 

void main() {

	float Z = texture(Depth, TexCoord).x * 2. - 1.; 
	float LinearZ = 2.0 * zNear * zFar / (zFar + zNear - Z * (zFar - zNear));

	int LinearZInt = int(min(LinearZ*1000, 16777216)); 

	int Value1 = LinearZInt % 255; 
	int Value2 = (LinearZInt / 255) % 255; 
	int Value3 = (LinearZInt / 65536); 

	Color = vec4(clamp(float(Value1)/255.f,0.,1.),clamp(float(Value2)/255.f,0.,1.),clamp(float(Value3)/255.f,0.,1.),1.); 
}