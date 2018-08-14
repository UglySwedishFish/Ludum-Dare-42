#pragma once
#include <Mesh/Mesh.h>
#include <Core Rendering/FrameBuffer.h>
#include <Real Time Rendering/ShadowPass.h>

const std::string OutPutNames[] = { "PosX.png", "NegX.png", "PosY.png", "NegY.png", "PosZ.png", "NegZ.png" };

namespace LDEngine {
	namespace Rendering {
		namespace ReflectionProbes {


			struct ReflectionProbe {
				std::string OutPutFile;
				Vector3f Position; 
				unsigned int Resolution; 
				ReflectionProbe(std::string OutPutFile, Vector3f Position = Vector3f(0.), unsigned int Resolution = 0) : OutPutFile(OutPutFile), Position(Position), Resolution(Resolution) {}
			};

			class ReflectionProbeBaker {
				Core::Shader DefferedRenderer, PostProcessDeffered, ShadowStructureShader, DepthConverter; 
				ShadowStructure Shadows; 
				Core::MultiPassFrameBufferObject DefferedFrameBuffer; 
				Core::FrameBufferObject LightFrameBufferObject, DepthFrameBuffer; 
			public:
				void PrepareReflectionProbeBaking(unsigned int Resolution); 
				void BakeReflectionProbes(Window & Window, std::vector<ReflectionProbe> & ReflectionProbes, std::vector<Mesh::Model> Models, Vector2f ShadowDirection); 

			};

		}
	}
}