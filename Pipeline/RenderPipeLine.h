#pragma once
#include "Core Rendering/FrameBuffer.h"
#include "Core Rendering/UI.h"
#include "Game/Wave.h"
#include "Game/Weapons.h"
#include "Mesh/AnimatedMesh.h"
#include "Real Time Rendering/ParticleSystem.h"
#include "Real Time Rendering/ShadowPass.h"

namespace LDEngine {
	namespace Rendering {
		namespace Pipeline {

			struct ReflectionProbes {
				std::vector<Vector3f> Positions; 
				unsigned int Probes; 
				unsigned int ProbePositions; 
				unsigned int ProbeWorldPosition; 
			};

			class RenderPipeline {
				Core::Shader DefferedShader, Compositing, ShadowPass, DefferedLightingShader, Particles, FinalLighting;
				Core::MultiPassFrameBufferObject DefferedFrameBuffer, RefractiveDefferedFrameBuffer, DefferedLighting;
				Core::FrameBufferObject ParticleSystemFrameBuffer, FinalLightingFrameBuffer; 
				std::vector<Mesh::Model> Models; 
				std::vector<Mesh::AnimatedKeyFrameModel> AnimatedKeyFrameModels; 
				ShadowStructure Shadows; 
				ReflectionProbes ReflectionProbes; 
				
				
				
			public:

				
				
				Game::EnemySystem EnemySystem;
				Vector2f Direction; 
				bool DrawParticleSystem;
				void Prepare(Window &Window, Control::Camera & Camera, std::vector<Mesh::Model> & Models, std::vector<Mesh::AnimatedKeyFrameModel> & AnimatedKeyFrameModels, Vector2f Direction);
				void Draw(Window & Window, Control::Camera & Camera, Game::Weapon * Weapon, UI::UIPipeLine & UI); 
				void AddEnemy(Game::Enemy Enemy); 
			};
		}
	}
}
