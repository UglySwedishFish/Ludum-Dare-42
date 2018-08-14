#pragma once
#include "Core Rendering/FrameBuffer.h"
#include "Core Rendering/Shader.h"
#include "Mesh/Mesh.h"

namespace LDEngine {
	namespace Rendering {

		struct ShadowStructure {
			Core::FrameBufferObject ShadowMaps[3];
			Matrix4f ViewMatrix[3], ProjectionMatrix[3];

			ShadowStructure();
			void PrepareShadowStructure(Vector2i Resolution);
			void UpdateRotation(Vector2f Direction, Control::Camera Camera);
			void BindUniform(unsigned int ShaderID, unsigned int BaseTexture);

		};
	}
}