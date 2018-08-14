#pragma once
#include <Mesh\Mesh.h>
#include <Core Rendering/Shader.h>
#include <Core Rendering/FrameBuffer.h>
namespace LDEngine {
	namespace Rendering {

		enum ParticleSystemType { NONE, EXPLOSION, SMOKE, STATIC, FIRE }; 
		//EXPLOSION = creates a high amount of particles that have a fast speed and low lifespan
		//their Direction is completely random

		//SMOKE = slowly spawns particles that have a very long lifespan and very slow speed 
		//(though they do move) 
		//their direction also changes over time

		//STATIC = creates particles that stay in place, and dont move at all 
		
		//FIRE = creates particles that dont ever die, unless its no longer emmiting


		struct Particle {
			Vector3f Position; 
			Vector3f Direction; //unit vector
			float Speed; 
			float LifeSpan; //if <0, lifespan is endless
			float TimeAlive; 
			float AnimationSpeed; 
			bool IsAlive; //if the particle is alive
			bool FirstPoll; 
			bool ShouldUpdate; 
			Particle(Vector3f Position, Vector3f Direction, float Speed, float LifeSpan, float AnimationSpeed, bool Update = true); 
			void Update(float FrameTime);
		};

		struct ParticleSystem {
			ParticleSystemType Type; 
			std::vector<Particle> Particles; 

			Vector3f Origin; 
			unsigned int PositionsTexture; 
			unsigned int MaxParticles; 
			unsigned short Instances; 
			bool IsEmmiting; //for smoke, fire
			Core::Texture2DArray Textures; 
			float AnimationSpeed; 
			Vector2f SpreadFactor; 
			float Scale; 

			ParticleSystem() : Type(NONE), Particles{}, PositionsTexture(0), Textures(), AnimationSpeed(0.) {}

			void Reset(float MinSpeed, float MaxSpeed, Vector3f Origin); 

			ParticleSystem(Core::Texture2DArray & Texture, float AnimationSpeed, int ParticleCount, float MinSpeed, float MaxSpeed, Vector3f Origin, float Scale = 1.f); //EXPLOSION
			ParticleSystem(Core::Texture2DArray & Texture, float LifeSpan, float AnimatonSpeed, int MaxParticleCount, Vector3f Origin, float MinSpeed, float MaxSpeed, float MaxSpread, float Scale); //SMOKE
			ParticleSystem(Core::Texture2DArray & Texture, float Speed, float AnimationSpeed, int ParticleCount, Vector3f Origin, float MinSpread, float MaxSpread, float Scale); //STATIC
			ParticleSystem(Core::Texture2DArray & Texture, float AnimationSpeed, float LifeSpan, int ParticleCount, Vector3f Direction, float MinSpeed, float MaxSpeed, Vector2f SpreadFactor, Vector3f Origin, float Scale = 1.f); //FIRE
			


			void CreatePositionTexture(unsigned int Size);
			void Draw(Core::Shader ActiveParticleShader, Matrix4f View); 
			void Update(Window Window); 
			
		};


	}
}