#pragma once
#include <GL/glew.h>
#include <Mesh\Mesh.h> 


namespace LDEngine {
	namespace Rendering {
		namespace Core {
			struct FrameBufferObject {
				GLuint FrameBuffer, ColorBuffer, DepthBuffer;
				glm::ivec2 Resolution;
				bool GenerateMip; 
				FrameBufferObject(glm::ivec2 Resolution, bool SamplerShadow, bool alpha = false, bool generatemip = false);
				FrameBufferObject(); 
				void Bind();
				void UnBind(Window Window);

			};
			struct MultiPassFrameBufferObject {
				GLuint FrameBuffer, DepthBuffer;
				std::vector<GLuint> ColorBuffers;
				glm::ivec2 Resolution;
				bool GenerateMip; 
				MultiPassFrameBufferObject(glm::ivec2 Resolution, int stages, bool alpha = false, bool generatemip = false);
				MultiPassFrameBufferObject(); 
				void Bind();
				void UnBind(Window Window);
			};
			struct CubeMultiPassFrameBufferObject {
				unsigned int DepthTexture, DepthBuffer, FrameBuffer;
				std::vector<unsigned int> Texture;
				Vector2i Resolution;
				inline CubeMultiPassFrameBufferObject() :
					DepthTexture(0),
					DepthBuffer(0),
					Texture{},
					FrameBuffer(0),
					Resolution(0)
				{}

				void Bind();
				void UnBind(Window Window);
				CubeMultiPassFrameBufferObject(Vector2i Resolution, int Targets);
			};
			struct CubeFrameBufferObject {
				unsigned int DepthTexture, DepthBuffer, FrameBuffer, Texture;
				Vector2i Resolution;
				inline CubeFrameBufferObject() :
					DepthTexture(0),
					DepthBuffer(0),
					Texture{},
					FrameBuffer(0),
					Resolution(0)
				{}

				void Bind();
				void UnBind(Window Window);
				CubeFrameBufferObject(Vector2i Resolution);
			};

			void PreparePostProcess();
			void DrawPostProcessQuad();
			void DrawQuadsInstanced(unsigned int Instances); 
			void DrawPostProcessCube(); 
		}
	}
}