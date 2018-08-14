#pragma once
#include "Mesh.h"

namespace LDEngine {
	namespace Rendering {
		namespace Mesh {
			struct AnimatedMeshModel {
				std::vector<Model> Models = {};
			};

			struct KeyFrame {
				unsigned short PositionInAnimation; 
				Vector3f Position; 
				Vector3f Rotation; 
				Vector3f Scale; 
				KeyFrame(unsigned short PositionInAnimation = 0, Vector3f Position = Vector3f(0.), Vector3f Rotation = Vector3f(0.), Vector3f Scale = Vector3f(0.)) :
					PositionInAnimation(PositionInAnimation), Position(Position), Rotation(Rotation), Scale(Scale) {}
			};

			enum InterpolationTypes { SINE, LINEAR }; //sine uses sine waves, linear just linearly interpolates

			class AnimatedKeyFrameModel {
				unsigned short AnimationLength = 0; 
			public: 
				Model Model;
				unsigned short Frame;
				std::vector<KeyFrame> KeyFrames; 
				InterpolationTypes Interpolations[3] = { SINE, LINEAR, SINE }; //Position, Rotation, Scale
				float Speed = 0.f; 
				float TimeSinceLastFrameChange = 0.f; 
				void AttatchKeyFrame(KeyFrame KeyFrame);
				void UpdateAnimation(float FrameTime); 
			};


			
			void DrawAnimatedMeshModel(Window & Window, Core::Shader Shader, Control::Camera Camera, AnimatedMeshModel & Model, unsigned char Frame);
			AnimatedMeshModel LoadAnimatedMeshModel(const std::string & File, unsigned char Frames, float Speed);
			void DrawAnimatedKeyFrameModel(Core::Shader Shader, AnimatedKeyFrameModel & Model, unsigned short Frame, bool Refractive = false); 
		}
	}
}