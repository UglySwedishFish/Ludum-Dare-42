#include "Enemy.h"
#include <iostream>
#include <Core Rendering/Texture.h>


void LDEngine::Game::Enemy::UpdateEnemy(EnemyPath & Path, Window & Window, Control::Camera & Camera, std::vector<Collisions::AABB>& CollisionBoxes, std::vector<ParticleRequest> & ParticleRequests)
{

	float DistanceToCurrentPathSquared = glm::dot(Path[CurrentPath].Position - Vector2f(Position.x, Position.z), Path[CurrentPath].Position - Vector2f(Position.x, Position.z));

	if (DistanceToCurrentPathSquared < .05)
		CurrentPath++;

	EnemyStatus Status = TOWARDSPATH; 

	PathInfo NextPath = Path[CurrentPath + 1];

	FollowStatus RuleSet = Path[CurrentPath].Follow; 

	float DistanceToFollow = 300. * PlayerAgression; //depending on how much ammo we have, we may or may not want to follow the player.

	float DistanceToCamera = glm::distance(Vector2f(Position.x, Position.z), Vector2f(Camera.Position.x, Camera.Position.z)); 

	//if the distance between the enemy and camera is within the distance treshold and the ruleset isnt never, point attention to camera instead of path. 
	if (DistanceToCamera < DistanceToFollow && RuleSet != NEVER) { 
		if (abs(Position.y - Camera.Position.y) < .25 || RuleSet == ALWAYS)
			Status = TOWARDSCAMERA; 
	}

	Vector2f RotationGoal = Vector2f(0.); 
	
	if (Status == TOWARDSCAMERA) {
		RotationGoal = Vector2f(Camera.Position.x, Camera.Position.z); 
	}
	else {
		RotationGoal = Path[CurrentPath].Position; 
	}

	

	float ActualRotationGoal = atan2(RotationGoal.x - Position.x, RotationGoal.y - Position.z) * 57.2957795; //(180 / pi)

	ActualRotationGoal = ActualRotationGoal < 0. ? 360 + ActualRotationGoal : ActualRotationGoal; 


	//move actual direction towards the desired rotation goal 

	//work out edge cases first

	if (CurrentPath != 9) {

		if (ActualRotationGoal > 180. && Direction > ActualRotationGoal + 180.) {
			Direction -= Window.GetFrameTime() * 360.;
		}
		else if (ActualRotationGoal < 180. && Direction < ActualRotationGoal - 180.) {
			Direction += Window.GetFrameTime() * 360.;
		}
		else {
			if (ActualRotationGoal > Direction) {
				Direction += Window.GetFrameTime() * 360.;
			}
			else {
				Direction -= Window.GetFrameTime() * 360.;
			}
		}
	}

	Direction = Direction > 360. ? Direction - 360. : Direction < 0. ? Direction + 360. : Direction; 

	

	float DirectionX = cos((Direction - 90.)*(PI / 180.))*6.*Window.GetFrameTime();
	float DirectionZ = -sin((Direction - 90.)*(PI / 180.))*6.*Window.GetFrameTime();

	if (CurrentPath != 9) {
		Position.x += DirectionX;
		Position.z += DirectionZ;
	}

	//should we fire at the player or not? 
	//4.0 could be replaced by some kind of accuracy variable
	if ((abs(Direction - ActualRotationGoal) < 4.0 || abs((Direction + 360.) - ActualRotationGoal) < 4.0 || abs((Direction - 360.) - ActualRotationGoal) < 4.) && Status == TOWARDSCAMERA && DistanceToCamera < 4.35 && abs(Position.y - Camera.Position.y) < .25) {
		//we should fire, but for now just print fire into the console 
		
		//say we need about 10 particles or so 

		for (int Particle = 0; Particle < 10; Particle++) {

			Vector3f Direction = glm::normalize(glm::mix(glm::normalize(Vector3f(DirectionX, 0., DirectionZ)), Core::RandUnit(), 0.01)); 

			ParticleRequests.push_back(ParticleRequest(Position + Vector3f(0.16 * DirectionX, 0.97, -0.7 * DirectionZ), Direction)); 


		}

		Ammo -= Window.GetFrameTime() * .5; 
		PlayerAgression -= Window.GetFrameTime() * .5; 

		PlayerAgression = glm::max(PlayerAgression, 0.f); 

		if (Ammo <= 0.)
			BlowUp = true; //BLOW SHIT UP 

	}

	if (Status == TOWARDSPATH && CurrentPath == 9) {
		//we should fire, but for now just print fire into the console 

		//say we need about 10 particles or so 

		for (int Particle = 0; Particle < 10; Particle++) {

			Vector3f Direction = glm::normalize(glm::mix(glm::normalize(Vector3f(DirectionX, 0., DirectionZ)), Core::RandUnit(), 0.01));

			ParticleRequests.push_back(ParticleRequest(Position + Vector3f(0.16 * DirectionX, 0.97, -0.7 * DirectionZ), Direction));


		}

		Ammo -= Window.GetFrameTime() * .5;

		if (Ammo <= 0.)
			BlowUp = true; //BLOW SHIT UP 

	}



	std::array<bool, 6> SideCollisions = { false,false,false,false,false,false };

	for (auto & CollisionBox : CollisionBoxes) {
		CollisionBox.Collision(Position, 1.8, .75, SideCollisions);
	}

	if (SideCollisions[Collisions::PositiveY] || SideCollisions[Collisions::NegativeY])
		CurrentSpeed = 0.; 

	CurrentSpeed -= GravityAcceleration * Window.GetFrameTime(); 
	CurrentSpeed = std::max(CurrentSpeed, -MaxGravitySpeed); 

	Position.y += CurrentSpeed * Window.GetFrameTime(); 

	TimeSinceLastFrameChange += Window.GetFrameTime(); 
	if (TimeSinceLastFrameChange > 0.02) {
		FrameInAnimation++;
		TimeSinceLastFrameChange = 0.;
		if (FrameInAnimation >= 29)
			FrameInAnimation = 0;
	}



}

void LDEngine::Game::EnemySystem::PrepareEnemySystem()
{
	EnemyAnimation = Rendering::Mesh::LoadAnimatedMeshModel("Models/Animations/Enemy", 30, 0.);
	Rendering::Core::Texture2DArray FireAnimation = Rendering::Core::Load2DAnimation("Textures/Animations/fire", 6, Vector2i(135)); 
	Rendering::Core::Texture2DArray ExplosiveAnimation = Rendering::Core::Load2DAnimation("Textures/Animations/explosion", 5, Vector2i(120));



	ParticleSystem = Rendering::ParticleSystem(FireAnimation, 100., 0.05, 2000, Vector3f(0.), .0, .0, 0.1); 
	ExplosiveParticleSystem = Rendering::ParticleSystem(ExplosiveAnimation, .05, 250, 10., 10., Vector3f(-100.), 1.); 

	//grab magical numbers from blender

	Path.push_back(PathInfo(Vector2f(32.3841, 0.04), NEVER)); //first paths for the enemies to take 
	Path.push_back(PathInfo(Vector2f(27.0194, 1.92578), NEVER));
	Path.push_back(PathInfo(Vector2f(24.0302, -1.99284), NEVER));
	Path.push_back(PathInfo(Vector2f(21.9553, 0.559999), NEVER));
	Path.push_back(PathInfo(Vector2f(17., 0.04), NEVER));



	Path.push_back(PathInfo(Vector2f(13.3827, -7.12789), NEVER));
	Path.push_back(PathInfo(Vector2f(12.85, -16.), ALWAYS));
	Path.push_back(PathInfo(Vector2f(5., -8.68)));
	Path.push_back(PathInfo(Vector2f(4., -0.362839), NEVER));
	
}

void LDEngine::Game::EnemySystem::AddEnemy(Enemy Enemy)
{
	Enemies.push_back(Enemy); 
}

void LDEngine::Game::EnemySystem::HandleEnemies(Window & Window, Control::Camera & Camera, std::vector<Collisions::AABB>& CollisionBoxes, SoundPipeline & Sounds,  int & PlayerHealth,  int & TowerHealth)
{

	std::vector<ParticleRequest> ParticleRequests; 
	
	std::vector<int> EraseLocations; 

	for(int Enemy = 0; Enemy < Enemies.size(); Enemy++) {
		Enemies[Enemy].UpdateEnemy(Path, Window, Camera, CollisionBoxes, ParticleRequests);
		if (Enemies[Enemy].BlowUp) {
			Sounds.SetSoundState(EXPLOSION, PLAYFROMSTART); 
			ExplosiveParticleSystem.Reset(125., 100., Enemies[Enemy].Position + Vector3f(0.9));
			EraseLocations.push_back(Enemy); 

		}
	}

	for (int Erase = EraseLocations.size() - 1; Erase >= 0; Erase--) {
		Enemies.erase(Enemies.begin() + EraseLocations[Erase]); 
	}

	EraseLocations.clear(); 
	
	unsigned int DeadParticleStart = 0; 

	for (int PossibleParticleCandidate = 0; PossibleParticleCandidate < ParticleSystem.Particles.size(); PossibleParticleCandidate++) {
		if (!ParticleSystem.Particles[PossibleParticleCandidate].IsAlive) {
			DeadParticleStart = PossibleParticleCandidate;
			break; 
		}
	}


	int Offset = 1; 

	if (ParticleRequests.size() > ParticleSystem.Particles.size() - DeadParticleStart) {
		Offset = ceil(float(ParticleRequests.size()) / (ParticleSystem.Particles.size() - DeadParticleStart)); 
	}

	if (ParticleRequests.size() > 0)
		Sounds.SetSoundState(FLAMETHROWER, PLAYANDREPEAT);
	else
		Sounds.SetSoundState(FLAMETHROWER, PAUSE); 

	for (int Particle = 0; Particle < ParticleRequests.size(); Particle+=Offset) {
		ParticleSystem.Particles[DeadParticleStart + Particle] = Rendering::Particle(ParticleRequests[Particle].Position, ParticleRequests[Particle].Rotation, 14.5, 0.3, 0.05);
	}

	ParticleSystem.Update(Window); 
	ExplosiveParticleSystem.Update(Window); 

	Collisions::AABB CameraAABB = Collisions::AABB(0., Camera.Position + Vector3f(.0, .9, .0), Vector3f(.2, .9, .2));
	Collisions::AABB TowerAABB = Collisions::AABB(0., Vector3f(5.08676, 1.60865, 7.94901), Vector3f(4.914, 5.086, 1.755)); 
	Collisions::AABB BlockadeAABB = Collisions::AABB(0., Vector3f(10.6785, 2.96437, 0.90591), Vector3f(0.342768, 3.20953, 15.4485));

	

	for(int Particle = 0; Particle < ParticleSystem.Particles.size(); Particle++) {


			if (BlockadeAABB == ParticleSystem.Particles[Particle].Position) {
				EraseLocations.push_back(Particle); 
				std::cout << "Blockade!\n"; 
			}
			else if (CameraAABB == ParticleSystem.Particles[Particle].Position) {
				EraseLocations.push_back(Particle);
				PlayerHealth--;
				std::cout << "Player!\n";
			}
			else if (TowerAABB == ParticleSystem.Particles[Particle].Position) {
				EraseLocations.push_back(Particle);
				TowerHealth -= 4;
			}
		
	}

	for (int Erase = EraseLocations.size() - 1; Erase >= 0; Erase--) {
		ParticleSystem.Particles.erase(ParticleSystem.Particles.begin() + EraseLocations[Erase]);
	}

	EraseLocations.clear();

	for (int Particle = 0; Particle < ExplosiveParticleSystem.Particles.size(); Particle++) {

		if (ExplosiveParticleSystem.Particles[Particle].IsAlive) {
			if (BlockadeAABB == ExplosiveParticleSystem.Particles[Particle].Position) {
				EraseLocations.push_back(Particle);
				std::cout << "Blockade!\n";
			}
			else if (CameraAABB == ExplosiveParticleSystem.Particles[Particle].Position) {
				EraseLocations.push_back(Particle);
				PlayerHealth -= 10;
			}
			else if (TowerAABB == ExplosiveParticleSystem.Particles[Particle].Position) {
				EraseLocations.push_back(Particle);
				TowerHealth -= 100;
			}
		}

	}

	for (int Erase = EraseLocations.size() - 1; Erase >= 0; Erase--) {
		ExplosiveParticleSystem.Particles.erase(ExplosiveParticleSystem.Particles.begin() + EraseLocations[Erase]);
	}

}

void LDEngine::Game::EnemySystem::DrawEnemies(Window & Window, Control::Camera & Camera, Rendering::Core::Shader & CurrentShader)
{
	//naive, one drawcall per enemy way of drawing enemies. (wouldve REALLY liked to use instancing but couldn't because no time) 

	for (auto & Enemy : Enemies) {

		if (!Enemy.BlowUp) {
			EnemyAnimation.Models[Enemy.FrameInAnimation].Position = Enemy.Position;
			EnemyAnimation.Models[Enemy.FrameInAnimation].Rotation = Vector3f(0., Enemy.Direction, 0.);
			Rendering::Mesh::DrawAnimatedMeshModel(Window, CurrentShader, Camera, EnemyAnimation, Enemy.FrameInAnimation);
		}

	}
	
}

void LDEngine::Game::EnemySystem::DrawEnemyParticleSystem(Matrix4f ViewMatrix, Rendering::Core::Shader & CurrentShader)
{
	ParticleSystem.Draw(CurrentShader, ViewMatrix); 
	ExplosiveParticleSystem.Draw(CurrentShader, ViewMatrix); 
}
