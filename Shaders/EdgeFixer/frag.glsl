#version 330
in vec2 TexCoord; 
out vec3 Color; 
uniform sampler2D LightBakedTexture; 

const int pixels = 25; 

void main() {
	Color = texture(LightBakedTexture,TexCoord).rgb; 
	if(Color == vec3(0.123,0.12,0.927)) {
	vec2 texelSize = vec2(1.0) / vec2(textureSize(LightBakedTexture,0)); 

		float PreviousDistance = 10000.0; 

		for(int x=-pixels;x<pixels;x++) {
			for(int y=-pixels;y<pixels;y++) {
				vec3 texel = texture(LightBakedTexture,TexCoord + vec2(x,y)*texelSize).rgb; 
				
				if(texel != vec3(0.123,0.12,0.927)) {
				float d = dot(vec2(x,y)*texelSize,vec2(x,y)*texelSize); 
					if(d < PreviousDistance) {
						PreviousDistance = d; 
						Color = texel; 
					}
				}
			}
		}
	}
}