#include "Weapons.h"
#include <Mesh/AnimatedMesh.h>
#include <iostream>


LDEngine::Rendering::Mesh::AnimatedKeyFrameModel PistolModel; 

float TimeRightClickHasBeenDown = 0.; 

void LDEngine::Game::PrepareWeaponModels()
{
	PistolModel.Model = LDEngine::Rendering::Mesh::Model("Models/Pistol_.obj");
	PistolModel.Speed = .01; 
	PistolModel.AttatchKeyFrame(LDEngine::Rendering::Mesh::KeyFrame(0, Vector3f(0.5575, -0.271736, -0.562167), Vector3f(0.), Vector3f(1.))); 
	PistolModel.AttatchKeyFrame(LDEngine::Rendering::Mesh::KeyFrame(100, Vector3f(-0.55, 0., 0.), Vector3f(0.), Vector3f(1.)));
	PistolModel.Interpolations[0] = Rendering::Mesh::LINEAR; 
}



bool RayAABBIntersection(Vector3f Origin, Vector3f OneOverDirection, LDEngine::Collisions::AABB & AABB, float & ClosestHit) {

	using std::max;
	using std::min;

	Vector3f Min = AABB.Min; 
	Vector3f Max = AABB.Max; 

	float t1 = (Min.x - Origin.x)*OneOverDirection.x;
	float t2 = (Max.x - Origin.x)*OneOverDirection.x;
	float t3 = (Min.y - Origin.y)*OneOverDirection.y;
	float t4 = (Max.y - Origin.y)*OneOverDirection.y;
	float t5 = (Min.z - Origin.z)*OneOverDirection.z;
	float t6 = (Max.z - Origin.z)*OneOverDirection.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	if ((ClosestHit > tmin || ClosestHit < 0.) && tmax > 0.f && tmax > tmin) {
		ClosestHit = tmin; 
		return true; 
	}
	return false; 
}


void LDEngine::Game::Weapon::Shoot(Vector3f Origin, Vector3f Direction, EnemySystem & Enemies, Collisions::AABB & Boxes, SoundPipeline & Sounds, Window Window)
{

	if (GetTimeSinceLastShot() + GetFireRate() < Window.GetTimeOpened()) {

		//construct Rayboxes around enemies 

		std::vector<Collisions::AABB> EnemyRayBoxes; 


		for (auto & Enemy : Enemies.Enemies) {
			EnemyRayBoxes.push_back(Collisions::AABB(0., Enemy.Position + Vector3f(0., 0.9, 0.), Vector3f(.5, .9, .5)));
		}


		Vector3f ActualDirection = Vector3f(0.); 
		Core::Move_3D(ActualDirection, 1., Direction.x - 180., Direction.y + 90., 1., true); 

		std::cout << ActualDirection.x << ' ' << ActualDirection.y << ' ' << ActualDirection.z << '\n'; 

		Vector3f OneOverDirection = Vector3f(1.f) / ActualDirection; 
		
		float ClosestEnemy = -1.f; 
		int ClosestEnemyIndex = -1; 
		

		float HitHeight = 0.; 

		for (int i = 0; i < EnemyRayBoxes.size(); i++) {

			

			if (RayAABBIntersection(Origin, OneOverDirection, EnemyRayBoxes[i], ClosestEnemy)) {
				ClosestEnemyIndex = i; 


				HitHeight = ((Origin.y + ActualDirection.y * ClosestEnemy) - EnemyRayBoxes[i].Min.y);



			}

		}

		bool IsHeadShot = HitHeight > 1.25;


		if (ClosestEnemyIndex >= 0) {

			if (Enemies.Enemies[ClosestEnemyIndex].Health <= (IsHeadShot ? GetHeadShotMultiplier() : 1) * GetDamage()) {
				Enemies.Enemies[ClosestEnemyIndex].BlowUp = true;
			}
			else
				Enemies.Enemies[ClosestEnemyIndex].Health -= (IsHeadShot ? GetHeadShotMultiplier() : 1) * GetDamage();
			
		}

		std::cout << HitHeight << '\n';


		TimeSinceLastShot = Window.GetTimeOpened(); 

		Sounds.SetSoundState(GetSound(), PLAYFROMSTART);
	}

}

void LDEngine::Game::Pistol::Draw(Control::Camera & Camera, Window & Window, Rendering::Core::Shader & Shader, bool RightKeyDown)
{
	PistolModel.UpdateAnimation(Window.GetFrameTime()); 


	TimeRightClickHasBeenDown += RightKeyDown ? Window.GetFrameTime() : -Window.GetFrameTime(); 

	TimeRightClickHasBeenDown = glm::clamp(TimeRightClickHasBeenDown, 0.f, .1f); 

	int Frame = glm::min(int(floor(TimeRightClickHasBeenDown * 1000.f)),99); 

	float Interpolation = glm::clamp(abs((Window.GetTimeOpened() - GetTimeSinceLastShot()) / GetFireRate() * 2.f - 1.f), 0.f, 1.f); 

	

	Matrix4f EmptyViev = Core::ViewMatrix(Vector3f(0.), Vector3f(0.)); 

	glUniformMatrix4fv(glGetUniformLocation(Shader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(EmptyViev)); 

	PistolModel.Model.Rotation = glm::mix(Vector3f(5., 0., 0.), Vector3f(0., 0., 0.), Interpolation); 

	Rendering::Mesh::DrawAnimatedKeyFrameModel(Shader, PistolModel, Frame, false); 

}

