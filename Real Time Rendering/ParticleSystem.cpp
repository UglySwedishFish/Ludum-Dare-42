#include "ParticleSystem.h"
#include <iostream>

LDEngine::Rendering::Particle::Particle(Vector3f Position, Vector3f Direction, float Speed, float LifeSpan, float AnimationSpeed, bool Update) :
	Position(Position), Direction(Direction), Speed(Speed), LifeSpan(LifeSpan), AnimationSpeed(AnimationSpeed), TimeAlive(0.), IsAlive(true), ShouldUpdate(Update)
{

}

void LDEngine::Rendering::Particle::Update(float FrameTime)
{
	TimeAlive += FrameTime; 
	Position += Direction * Speed * FrameTime;
	if(LifeSpan > 0. && ShouldUpdate)
	IsAlive = (TimeAlive < LifeSpan);
}



void LDEngine::Rendering::ParticleSystem::Reset(float MinSpeed, float MaxSpeed, Vector3f Origin)
{
	if (Type == EXPLOSION) {
		for (int particle = 0; particle < Particles.size(); particle++) {

			float _AnimationSpeed = LDEngine::Core::mix(AnimationSpeed / 2., AnimationSpeed * 2., float(rand()) / RAND_MAX);

			float ActualLifeSpan = Textures.Length * _AnimationSpeed;

			Vector3f Direction = LDEngine::Core::RandUnit(); 

			Particles[particle] = Particle(Origin, Direction, Particles[particle].Speed, ActualLifeSpan, _AnimationSpeed); 

		}
	}
}

LDEngine::Rendering::ParticleSystem::ParticleSystem(Core::Texture2DArray & Texture, float AnimationSpeed, int ParticleCount, float MinSpeed, float MaxSpeed, Vector3f Origin, float Scale) :
	Textures(Texture), Type(EXPLOSION), AnimationSpeed(AnimationSpeed), PositionsTexture(0), Particles{}, MaxParticles(ParticleCount), Instances(ParticleCount), Origin(Origin), SpreadFactor(0.f), Scale(Scale) {

	for (int particle = 0; particle < ParticleCount; particle++) {

		float _AnimationSpeed = LDEngine::Core::mix(AnimationSpeed / 2., AnimationSpeed * 2., float(rand()) / RAND_MAX); 

		float ActualLifeSpan = Texture.Length * _AnimationSpeed;
		
		Vector3f Direction = LDEngine::Core::RandUnit(); 

		float Speed = LDEngine::Core::mix(MinSpeed, MaxSpeed, float(rand()) / RAND_MAX); 

		Particles.push_back(Particle(Origin, Direction, Speed, ActualLifeSpan, _AnimationSpeed)); 

	}
	CreatePositionTexture(MaxParticles); 
	
}

LDEngine::Rendering::ParticleSystem::ParticleSystem(Core::Texture2DArray & Texture, float Speed, float AnimationSpeed, int MaxParticleCount, Vector3f Origin, float MinSpeed, float MaxSpeed, float MaxSpread, float Scale) :
	Textures(Texture), Type(STATIC), AnimationSpeed(0.f), PositionsTexture(0), Particles{}, MaxParticles(MaxParticleCount), Instances(0), Origin(Origin), SpreadFactor(0.), IsEmmiting(false), Scale(Scale)
{
	for (int particle = 0; particle < MaxParticleCount; particle++) {

		float ActualLifeSpan = Texture.Length * AnimationSpeed;

		Particles.push_back(Particle(Origin, Vector3f(0.), Speed, ActualLifeSpan, AnimationSpeed, false));
		Particles[Particles.size() - 1].IsAlive = false; 

		

	}



	CreatePositionTexture(MaxParticles);
}

LDEngine::Rendering::ParticleSystem::ParticleSystem(Core::Texture2DArray & Texture, float LifeSpan, float AnimationSpeed, int ParticleCount, Vector3f Origin, float MinSpread, float MaxSpread, float Scale):
	Textures(Texture), Type(STATIC), AnimationSpeed(0.f), PositionsTexture(0), Particles{}, MaxParticles(ParticleCount), Instances(0), Origin(Origin), SpreadFactor(0.), IsEmmiting(false), Scale(Scale)
{
	for (int particle = 0; particle < ParticleCount; particle++) {

		float ActualLifeSpan = Texture.Length * AnimationSpeed;

		Particles.push_back(Particle(Origin, Vector3f(0.), 0.f, ActualLifeSpan, AnimationSpeed, false));
		Particles[Particles.size() - 1].IsAlive = false;



	}

	CreatePositionTexture(MaxParticles);
}

LDEngine::Rendering::ParticleSystem::ParticleSystem(Core::Texture2DArray & Texture, float AnimationSpeed, float LifeSpan, int ParticleCount, Vector3f Direction, float MinSpeed, float MaxSpeed, Vector2f SpreadFactor, Vector3f Origin, float Scale) :
	Textures(Texture), Type(FIRE), AnimationSpeed(AnimationSpeed), PositionsTexture(0), Particles{}, MaxParticles(ParticleCount), Instances(ParticleCount), Origin(Origin), SpreadFactor(SpreadFactor), IsEmmiting(true), Scale(Scale)
{
	for (int particle = 0; particle < ParticleCount; particle++) {

		float _AnimationSpeed = LDEngine::Core::mix(AnimationSpeed / 2., AnimationSpeed * 2., float(rand()) / RAND_MAX);

		float Speed = LDEngine::Core::mix(MinSpeed, MaxSpeed, float(rand()) / RAND_MAX);

		Vector3f Position; 

		float RandX = float(rand()) / RAND_MAX;
		float RandY = float(rand()) / RAND_MAX;

		Position.x = glm::mix(Origin.x - SpreadFactor.x, Origin.x + SpreadFactor.x, RandX);
		Position.z = glm::mix(Origin.z - SpreadFactor.y, Origin.z + SpreadFactor.y, RandY);
		Position.y = Origin.y;

		Particles.push_back(Particle(Position, Direction, Speed, 0., _AnimationSpeed));

	}
	CreatePositionTexture(MaxParticles);
}

void LDEngine::Rendering::ParticleSystem::CreatePositionTexture(unsigned int Size)
{
	glGenTextures(1, &PositionsTexture); 
	glBindTexture(GL_TEXTURE_1D, PositionsTexture); 
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, Size, 0, GL_RGBA, GL_FLOAT, nullptr); 
	glBindTexture(GL_TEXTURE_1D, 0);
}

void LDEngine::Rendering::ParticleSystem::Draw(Core::Shader ActiveParticleShader, Matrix4f View)
{

	Textures.Bind(0);
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_1D, PositionsTexture); 

	glUniformMatrix4fv(glGetUniformLocation(ActiveParticleShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(View));
	glUniform1f(glGetUniformLocation(ActiveParticleShader.ShaderID, "Scale"), Scale); 

	Core::DrawQuadsInstanced(Instances); 


		
}

void LDEngine::Rendering::ParticleSystem::Update(Window Window)
{
	Instances = 0; 

	std::vector<Vector4f> Positions = std::vector<Vector4f>(MaxParticles, Vector4f(0.)); 

	for(int Particle = 0; Particle < Particles.size(); Particle++) {
		Particles[Particle].Update(Window.GetFrameTime());
		if (Particles[Particle].IsAlive) {

			int Frame = std::floor(Particles[Particle].TimeAlive / Particles[Particle].AnimationSpeed + .5f); 

			if (Frame >= Textures.Length) {
				if (Type == EXPLOSION) {
					continue; 
				}
				else if (Type == FIRE) {
					//reset particle based on original data 
					if (Frame%Textures.Length == 0 && Particles[Particle].FirstPoll) {

						if (IsEmmiting) {

							Particles[Particle].FirstPoll = false;
							float RandX = float(rand()) / RAND_MAX;
							float RandY = float(rand()) / RAND_MAX;

							Particles[Particle].Position.x = glm::mix(Origin.x - SpreadFactor.x, Origin.x + SpreadFactor.x, RandX);
							Particles[Particle].Position.z = glm::mix(Origin.z - SpreadFactor.y, Origin.z + SpreadFactor.y, RandY);
							Particles[Particle].Position.y = Origin.y;
						}
						else
						{
							Particles[Particle].IsAlive = false; 
							continue; 
						}

					}
					else if(Frame%Textures.Length != 0)
						Particles[Particle].FirstPoll = true;

					Frame = Frame % Textures.Length; 
				}
				else if (Type == STATIC) {
					Particles[Particle].ShouldUpdate = false; 
					Particles[Particle].IsAlive = false;
					continue; 
				}
			}

			Positions[Instances++] = Vector4f(Particles[Particle].Position,float(Frame)/255.f);


		}
		else {
		}
	}

	glBindTexture(GL_TEXTURE_1D, PositionsTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, MaxParticles, 0, GL_RGBA, GL_FLOAT, &Positions[0]);
	glBindTexture(GL_TEXTURE_1D, 0);

}


