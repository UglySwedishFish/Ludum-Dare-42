#include "Core/Core.h"
#include "ShadowPass.h"

namespace LDEngine {
	namespace Rendering {
		ShadowStructure::ShadowStructure()
		{

		}

		void ShadowStructure::PrepareShadowStructure(Vector2i Resolution)
		{
			for (int ShadowProjection = 0; ShadowProjection < 3; ShadowProjection++) {
				ProjectionMatrix[ShadowProjection] = LDEngine::Core::ShadowOrthoMatrix(60., 0.1, 100.);
				ShadowMaps[ShadowProjection] = Core::FrameBufferObject(Resolution, true);
			}
		}

		void Move(Vector3f & Pos, float Speed, float RotationX, float RotationY)
		{
			Pos.x -= (cos(RotationY*(PI / 180.)) * cos(RotationX*(PI / 180.)))*Speed;
			Pos.y += sin(RotationX*(PI / 180.))*Speed;
			Pos.z -= (sin(RotationY*(PI / 180.)) * cos(RotationX*(PI / 180.)))*Speed;
		}

		void ShadowStructure::UpdateRotation(Vector2f Direction, Control::Camera Camera)
		{

			Vector3f Orientation; 

			Move(Orientation, 1.0, Direction.x, Direction.y - 90.); 

			for (int i = 0; i < 3; i++) {
				ViewMatrix[i] = LDEngine::Core::ViewMatrix(Orientation * 50.f, Vector3f(Direction.x, Direction.y, 0.));
			}

		}

		

		void ShadowStructure::BindUniform(unsigned int ShaderID, unsigned int BaseTexture) {

			for (int ShadowMap = 0; ShadowMap < 3; ShadowMap++) {
				std::string Base = "ShadowMaps[" + std::to_string(ShadowMap) + "].";

				std::string Map = Base + "Map";
				std::string ShadowView = Base + "ShadowView";
				std::string ShadowProj = Base + "ShadowProj";

				glActiveTexture(GL_TEXTURE0 + BaseTexture + ShadowMap);
				glBindTexture(GL_TEXTURE_2D, ShadowMaps[ShadowMap].DepthBuffer);
				glUniform1i(glGetUniformLocation(ShaderID, Map.c_str()), BaseTexture + ShadowMap);

				glUniformMatrix4fv(glGetUniformLocation(ShaderID, ShadowView.c_str()), 1, false, glm::value_ptr(ViewMatrix[ShadowMap]));
				glUniformMatrix4fv(glGetUniformLocation(ShaderID, ShadowProj.c_str()), 1, false, glm::value_ptr(ProjectionMatrix[ShadowMap]));

			}

		}

	}
}