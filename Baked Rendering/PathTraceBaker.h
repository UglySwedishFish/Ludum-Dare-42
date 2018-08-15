#pragma once
#include "Core Rendering/FrameBuffer.h"
#include "Core Rendering/Shader.h"
#include "MeshTextureWrapper.h"
#include "Real Time Rendering/ShadowPass.h"
#include "WrapToBakeTexture.h"

void SavePixelDataToImageFromFloatVector(const char *fileName, std::vector<float> data, glm::ivec2 Resolution, bool YInverted = false,bool Alpha = false);


namespace LDEngine {
	namespace Rendering {
		namespace PathTracing {
			class PathTracingPipeLine {
				Core::Shader LightBakerProgressive, EdgeFixer, ShadowStructureShader;
				Core::FrameBufferObject LightBakerFrameBuffer[2]; 
				unsigned int Resolution; 
			public: 
				void PrepareLightBaker(unsigned int Resolution); 
				void BakeLighting(Window & Window, Mesh::Model & WrapModel, std::vector<Mesh::Model> & Models, const char *OutPutFile, int Samples, int BlurStrength, Vector2f ShadowDirection, bool UseDirectionalShadows = false, Vector3f SkyBaseColor = Vector3f(0.0,0.5,1.0), Vector3f SkyHorizonColor = Vector3f(1.0, 0.5 ,0.0)); 
				//wrap model is the model that the texture is created for, models is for the models that will be accounted for in the lighting process
				//Samples tells the light baker how many samples it wants to use for the path tracing process
				//Blur strength tells it how much blur it should use for the blurring process of the output 
			}; 
		}
	}
}