#pragma once
#include "RenderPipeLine.h"
#include <Collisions/Collision.h>
#include <Game/Sound.h>


namespace LDEngine {
	namespace Rendering {

		enum GravityState { Falling, Jumping }; 
		enum UIElements {WaveNoRating, WaveRatingEmpty, WaveRatingA, WaveRatingB, WaveRatingC, WaveRatingD, WaveRatingE, Win, Loose, CrossHair};
		enum GameStatus {INTRO, INGAME, WON, LOST};

		namespace Pipeline {
			class Pipeline {
				RenderPipeline Rendering; 
				std::vector<Collisions::AABB> Boxes; 
				GravityState Gravity = Falling;
				Game::GameState CurretGameState; 

				float CurrentSpeed = 0.; 
				Game::SoundPipeline Sounds; 
				Game::Weapon * Weapon; 
				UI::UIPipeLine UI; 
				std::vector<UI::UIElement> AllElements; 
				std::array<UI::UIElement, 10> PlayerHealthBar; 
				std::array<UI::UIElement, 10> BaseHealthBar;
				std::vector<Game::Wave> Waves; 
				GameStatus Status; 

				int PlayerHealth = 1000;
				int TowerHealth = 10000;

			public: 
				void Prepare(Window & Window,Control::Camera & Camera, std::vector<Mesh::Model> & Models, std::vector<Mesh::AnimatedKeyFrameModel> & AnimatedKeyFrameModels, Vector2f Direction);
				void Run(Window & Window, Control::Camera & Camera); 
				void CameraMovement(Control::Camera & Camera, std::array<bool, 6> & CollisionSides, float FrameTime, bool SpaceBar);
			};
		}
	}
}