#pragma once
#include <vector>
#include <Core/Core.h>
#include <Core/Camera.h>
#include <Collisions/Collision.h>
#include <Mesh/AnimatedMesh.h>
#include <Real Time Rendering/ParticleSystem.h>
#include "Sound.h"

namespace LDEngine {

	const float MaxGravitySpeed = 100.; //(m/s)
	const float GravityAcceleration = 36.; //(m/s^2)

	const float MaxJumpSpeed = 18.; //(m/s)

	namespace Game {

		enum FollowStatus {ALWAYS, SIMILARY, NEVER};
		enum EnemyStatus { TOWARDSPATH, TOWARDSCAMERA, FIRINGATCAMERA }; 

		struct PathInfo {
			Vector2f Position; 
			FollowStatus Follow; 
			PathInfo(Vector2f Position = Vector2f(0.), FollowStatus Status = SIMILARY) : Position(Position), Follow(Status) {}
		};

		struct ParticleRequest {
			Vector3f Position; 
			Vector3f Rotation; 
			ParticleRequest(Vector3f Position = Vector3f(0.), Vector3f Rotation = Vector3f(0.)) : Position(Position), Rotation(Rotation) {}
		};

		using EnemyPath = std::vector<PathInfo>;

		struct Enemy {
			unsigned char Health; //enemies have 100 health. 
			float Direction; //the direction they're looking at
			unsigned char FrameInAnimation; //what frame they're on in the animation 
			unsigned char CurrentPath; 
			float TimeSinceLastFrameChange; 
			float PlayerAgression; 

			Vector3f Position; //their position
			float CurrentSpeed; //current fallign speed

			float Ammo; //their ammo (goes down by 1 per second they fire) 

			bool BlowUp; //tells the enemy system if this enemy wants to self destruct or not
			//(self destruction happens when an enemy is at the final destination and is out of ammo) 

			Enemy(Vector3f Position = Vector3f(0.)) : 
				Position(Position), Health(100), Direction(0.), FrameInAnimation(0), CurrentPath(0), CurrentSpeed(0.), Ammo(0.25), BlowUp(false), TimeSinceLastFrameChange(0.), PlayerAgression(glm::max(float(rand()) / (RAND_MAX*2), 0.f)) {}
			void UpdateEnemy(EnemyPath & Path, Window & Window, Control::Camera & Camera, std::vector<Collisions::AABB> & CollisionBoxes, std::vector<ParticleRequest> & ParticleRequests);
		};

		class EnemySystem {
			Rendering::Mesh::AnimatedMeshModel EnemyAnimation; 
			EnemyPath Path{};
			Rendering::ParticleSystem ParticleSystem; 
			Rendering::ParticleSystem ExplosiveParticleSystem;
		public: 
			std::vector<Enemy> Enemies;
			void PrepareEnemySystem(); 
			void AddEnemy(Enemy Enemy); 
			void HandleEnemies(Window & Window, Control::Camera & Camera, std::vector<Collisions::AABB> & CollisionBoxes, SoundPipeline & Sounds, int & PlayerHealth, int & TowerHealth);
			void DrawEnemies(Window & Window, Control::Camera & Camera, Rendering::Core::Shader & CurrentShader); 
			void DrawEnemyParticleSystem(Matrix4f ViewMatrix, Rendering::Core::Shader & CurrentShader);

		};
	}
}