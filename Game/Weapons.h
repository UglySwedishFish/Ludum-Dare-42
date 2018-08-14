#pragma once

#include "Enemy.h"

namespace LDEngine {
	namespace Game {

		enum WeaponType {
			Auto, Semi
		};

		void PrepareWeaponModels(); 

		class Weapon {
			float TimeSinceLastShot; 
			Vector3f Direction; 
		public: 
			virtual int GetDamage() = 0; 
			virtual float GetFireRate() = 0; //seconds per shot 
			virtual WeaponType Type() = 0;
			virtual Sound GetSound() = 0; 
			virtual int GetHeadShotMultiplier() = 0; 
			Vector3f GetDirection() { return Direction;  }
			float GetTimeSinceLastShot() { return TimeSinceLastShot; };
			void Shoot(Vector3f Origin,Vector3f Direction, EnemySystem & Enemies, Collisions::AABB & Boxes, SoundPipeline & Sounds, Window Window);
			virtual void HandleExplosive(EnemySystem & Enemies, Collisions::AABB & Boxes, SoundPipeline & Sounds) {
				
			};
			virtual void Draw(Control::Camera & Camera, Window & Window, Rendering::Core::Shader & Shader, bool RightKeyDown) = 0;

		};

		class Pistol : public Weapon {
		public: 
			int GetDamage() {
				return 25;
			}
			float GetFireRate() {
				return .1f; 
			}
			WeaponType Type() {
				return Semi; 
			}
			Sound GetSound() {
				return PISTOL; 
			}
			int GetHeadShotMultiplier() {
				return 2; 
			}
			void Draw(Control::Camera & Camera, Window & Window, Rendering::Core::Shader & Shader, bool RightKeyDown);
		};



	}
}