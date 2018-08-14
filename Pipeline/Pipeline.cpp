#include "Pipeline.h"
#include <iostream>

const std::array<std::string, 10> UIElementPaths = { "WaveNoRating", "WaveRatingEmpty", "WaveA", "WaveB", "WaveC", "WaveD", "WaveE", "WinScreen", "LooseScreen", "CrossHair" };

void LDEngine::Rendering::Pipeline::Pipeline::Prepare(Window & Window, Control::Camera & Camera, std::vector<Mesh::Model>& Models, std::vector<Mesh::AnimatedKeyFrameModel> & AnimatedKeyFrameModels, Vector2f Direction)
{
	Rendering.Prepare(Window, Camera, Models, AnimatedKeyFrameModels, Direction); 
	

	Boxes.push_back(Collisions::AABB(0., Vector3f(0.,0.,0.), Vector3f(0.,0.,0.))); 


	Boxes.push_back(Collisions::AABB(0., Vector3f(31.066, -2.43394, 0.07), Vector3f(100., 2.43919, 100.)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(10.5681, 0.818719, -0.193294), Vector3f(0.711421, 6., 14.6856)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(4.02795, 2.45009, 9.42698), Vector3f(7.05818, 6., 7.05818)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(26.3789, 2.63431, 8.25303), Vector3f(6.38494, 2.75022, 4.92569)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(26.3789, 2.63431, -7.68355), Vector3f(6.38494, 2.75022, 4.92569)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(26.2301, 5.05971, 0.055), Vector3f(6.23928, 0.341867, 12.5172)));

	for (int i = 0; i<9; i++)
		Boxes.push_back(Collisions::AABB(0., Vector3f(16.3112, -3.70314, -8.01652) + (Vector3f(0.5, 0.6, 0.)*Vector3f(i)), Vector3f(0.25, 4.2148, 4.52061)));

	for (int i = 0; i<9; i++)
		Boxes.push_back(Collisions::AABB(0., Vector3f(16.3112, -3.70314, 8.01652) + (Vector3f(0.5, 0.6, 0.)*Vector3f(i)), Vector3f(0.25, 4.2148, 4.52061)));

	for (int i = 0; i<3; i++)
		Boxes.push_back(Collisions::AABB(0., Vector3f(-0.279219, -0.619143, 0.) + (Vector3f(-.4, .4, 0.)*Vector3f(i)), Vector3f(0.2, 1.058, 24.)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(-3.11708, 4.59907, 0.055), Vector3f(2.02691, 4.59907, 33.1458)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(28.8083, 4.59907, 25.71), Vector3f(42.035, 4.59907, 2.28344)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(28.8083, 4.59907, -24.4293), Vector3f(42.035, 4.59907, 2.28344)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(20.5789, 2.37352, -2.15328), Vector3f(0.249306, 2.68432, 2.24487)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(25.0857, 2.37352, 1.87656), Vector3f(0.249306, 2.68432, 2.24487)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.3851, 2.37352, -1.54147), Vector3f(0.249306, 2.68432, 2.24487)));


	Boxes.push_back(Collisions::AABB(0., Vector3f(26.4975, 6.01975, -12.4015), Vector3f(6.07014, 1.02796, 0.280497)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(26.4975, 6.01975, 12.4015), Vector3f(6.07014, 1.02796, 0.280497)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(32.6354, 6.01975, 0.), Vector3f(0.227157, 1.02796, 12.8138)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(32.1089, 5.60195, -6.01975), Vector3f(0.328867, 0.234836, 3.14915)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(32.1089, 5.60195, 6.0687), Vector3f(0.328867, 0.234836, 3.14915)));

	
	Boxes.push_back(Collisions::AABB(0., Vector3f(20.8831, 0.693604, -15.5329), Vector3f(1.34794, 1.35093, 1.63482)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(21.4383, 1.05159, -20.6414), Vector3f(1.6415, 1.64514, 1.99085)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(25.0073, 1.05159, -18.2127), Vector3f(3.52463, 3.53244, 4.27476)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(24.6754, 0.767046, -14.3602), Vector3f(1.03116, 2.05684, 0.819602)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(25.993, 0.483084, -22.7801), Vector3f(1.03116, 1.64922, 0.819602)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(26.8879, 1.05159, -22.6071), Vector3f(0.9563, 1.62144, 1.15982)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.2685, 0.767046, -21.5398), Vector3f(0.675779, 2.05684, 0.819602)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.3663, 1.05159, -19.6713), Vector3f(1.45156, 2.05684, 1.76049)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.3663, 1.05159, -19.6713), Vector3f(1.45156, 2.05684, 1.76049)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.71, 1.05159, -17.8731), Vector3f(1.38704, 1.62144, 1.68223)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.1688, 1.05159, -14.8172), Vector3f(0.9563, 1.62144, 1.45907)));

	Vector3f Offset = Vector3f(2.7255, 0., 37.0087);

	Boxes.push_back(Collisions::AABB(0., Vector3f(20.8831, 0.693604, -15.5329) + Offset, Vector3f(1.34794, 1.35093, 1.63482)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(21.4383, 1.05159, -20.6414) + Offset, Vector3f(1.6415, 1.64514, 1.99085)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(25.0073, 1.05159, -18.2127) + Offset, Vector3f(3.52463, 3.53244, 4.27476)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(24.6754, 0.767046, -14.3602) + Offset, Vector3f(1.03116, 2.05684, 0.819602)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(25.993, 0.483084, -22.7801) + Offset, Vector3f(1.03116, 1.64922, 0.819602)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(26.8879, 1.05159, -22.6071) + Offset, Vector3f(0.9563, 1.62144, 1.15982)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.2685, 0.767046, -21.5398) + Offset, Vector3f(0.675779, 2.05684, 0.819602)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.3663, 1.05159, -19.6713) + Offset, Vector3f(1.45156, 2.05684, 1.76049)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.3663, 1.05159, -19.6713) + Offset, Vector3f(1.45156, 2.05684, 1.76049)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.71, 1.05159, -17.8731) + Offset, Vector3f(1.38704, 1.62144, 1.68223)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(29.1688, 1.05159, -14.8172) + Offset, Vector3f(0.9563, 1.62144, 1.45907)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(47.775, 0.693604, -21.7584), Vector3f(2.93022, 4.58099, 3.55385)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(46.3782, 0.693604, -15.6219), Vector3f(2.3752, 4.58099, 2.8807)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(48.7225, 0.693604, -10.5995), Vector3f(2.23868, 4.58099, 2.71512)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(49.8124, 0.693604, -9.19968), Vector3f(2.23868, 4.58099,  2.71512)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(49.8124, 0.693604, -9.19968), Vector3f(2.23868, 4.58099, 2.71512)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(47.9474, 0.693604, 8.70937), Vector3f(2.23868, 4.58099, 2.71512)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(48.9959, 0.693604, 11.7546), Vector3f(2.23868, 4.58099, 2.71512)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(46.4514, 0.693604, 16.593), Vector3f(2.23868, 4.58099, 2.71512)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(47.6063, 0.693604, 22.4061), Vector3f(2.81449, 4.58099, 3.41349)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(50.5479, 0.503774, -0.11233), Vector3f(1.05, 1.2, 7.11272)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(52.191, 3.29377, -0.316349), Vector3f(0.711421, 1.60613, 7.11272)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(59.8866, 6.51237, -0.797551), Vector3f(0.638827, 1.78958, 9.09725)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(50.2618, 6.51237, 7.25552), Vector3f(4.58399, 1.78958, 0.481946)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(50.2618, 6.51237, -7.70349), Vector3f(4.58399, 1.78958, 0.481946)));


	Boxes.push_back(Collisions::AABB(0., Vector3f(56.9797, 6.51237, 6.14523), Vector3f(4.58399, 1.78958, 1.78958)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(56.9797, 6.51237, -6.81603), Vector3f(4.58399, 1.78958, 1.78958)));

	Boxes.push_back(Collisions::AABB(0., Vector3f(50.9694, 2.93457, 6.6689), Vector3f(1.11078, 1.78958, 0.481946)));
	Boxes.push_back(Collisions::AABB(0., Vector3f(50.9694, 2.93457, -7.08579), Vector3f(1.11078, 1.78958, 0.481946)));

	
	Sounds.LoadSounds(); 
	UI.PrepareUiPipeLine(Window); 

	Game::PrepareWeaponModels(); 
	
	Sounds.SetMusicState(Game::Music::BATTLE, Game::PLAY);
	CurretGameState = Game::GAMESTATE_WAVE; 

	Vector2f AspectRatioCorrection = Vector2f(float(Window.GetResolution().y) / Window.GetResolution().x, 1.);
	//hm.

	float AspectPositionCorrect = 1. / AspectRatioCorrection.x; 


	for (int Element = 0; Element < UIElementPaths.size(); Element++) {

		

		std::string File = "Textures/UI/" + UIElementPaths[Element] + ".png"; 

		AllElements.push_back(UI::UIElement(Vector2f(0.), Vector2f(1.) * AspectRatioCorrection, Core::LoadTexture2D(File.c_str(), false, false))); 
	}

	for (int Bar = 0; Bar < 10; Bar++) {
		std::string _PlayerHealthBar = "Textures/UI/HealthBar_Player/HealthBar_Player_" + std::to_string(Bar + 1) + ".png"; 
		std::string _BaseHealthBar = "Textures/UI/HealthBar_Base/HealthBar_Base_" + std::to_string(Bar + 1) + ".png";


		PlayerHealthBar[Bar] = UI::UIElement(Vector2f(.2 * AspectPositionCorrect, 0.), Vector2f(1.) * AspectRatioCorrection, Core::LoadTexture2D(_PlayerHealthBar.c_str(), false, false));
		BaseHealthBar[Bar] = UI::UIElement(Vector2f(.2 * AspectPositionCorrect, 0.), Vector2f(1.) * AspectRatioCorrection, Core::LoadTexture2D(_BaseHealthBar.c_str(), false, false));


	}

	UI.AllocateElement(AllElements[UIElements::CrossHair]);


	//Waves! 

	for (int Wave = 0; Wave < 10; Wave++) {
		int Enemies = 2 * (Wave + 1);
		float EnemySpawnPoints = 3.f / Enemies;
		std::vector<float> EnemySpawnTimes;
		for (int Enemy = 0; Enemy < Enemies; Enemy++)
			EnemySpawnTimes.push_back((EnemySpawnPoints * Enemy) + 3.f); 

		Waves.push_back(EnemySpawnTimes); 

	}


	Status = INGAME; 


}

void LDEngine::Rendering::Pipeline::Pipeline::Run(Window & Window, Control::Camera & Camera)
{
	bool Running = true; 
	sf::Clock GameClock;
	sf::Event Event;
	bool Jumping = false; 
	
	Weapon = new Game::Pistol; 
	UIElements Rank; 
	bool GivenRankBefore = false; 

	float T = 0.; 
	float TimeElapsedIdle = 0.; 
	unsigned char Wave = 0; 

	Window.GetRawWindow()->setMouseCursorVisible(false); 
	

	while (Running) {

		bool SpaceBar = false;

		if (Status == INGAME) {

			while (Window.GetRawWindow()->pollEvent(Event)) {
				switch (Event.type)
				{
				case sf::Event::Closed:
					delete &Window;
					return;
				case sf::Event::KeyPressed:
					switch (Event.key.code) {
					case sf::Keyboard::Escape:
						Running = false;
						break;
					case sf::Keyboard::Space:
						SpaceBar = true;
						break;

					}
					break;
				case sf::Event::MouseButtonPressed:
					switch (Event.mouseButton.button) {
					case sf::Mouse::Button::Left:
						Weapon->Shoot(Camera.Position + Vector3f(0., 1.5, 0.), Camera.Rotation, Rendering.EnemySystem, Boxes[0], Sounds, Window);
						break;
					}

				}

			}




			Window.SetFrameTime(GameClock.getElapsedTime().asSeconds());

			if (CurretGameState == Game::GAMESTATE_WAVE) {

				Game::WaveState WaveState = Waves[Wave].HandleWave(Rendering.EnemySystem, Window, Vector3f(47.1225, 5., 0.), PlayerHealth, TowerHealth);

				if (WaveState != Game::ONGOING && Wave != Waves.size()-1) {
					Sounds.SetMusicState(Game::BATTLE, Game::STOP);
					Sounds.SetMusicState(Game::Music::IDLE, Game::PLAY);
					Sounds.SetSoundState(Game::PICKUP, Game::PLAY);
					CurretGameState = Game::GAMESTATE_IDLE;

					UI.AllocateElements({});
					UI.AllocateElement(AllElements[UIElements::WaveNoRating]);
					UI.AllocateElement(AllElements[UIElements::CrossHair]);

					Rank = UIElements(UIElements::WaveRatingA + WaveState);
					GivenRankBefore = false;
					TimeElapsedIdle = 0.f;
					


					if (Rank <= UIElements::WaveRatingB)
						PlayerHealth = 1000; 

					

					Wave++; 

					Waves[Wave].StartBaseHealth = TowerHealth; 
					Waves[Wave].StartPlayerHealth = PlayerHealth;

				}
				else if (WaveState != Game::ONGOING) {
					//the player has completed all waves, he will thus win the game 
					Status = WON; 

					UI.AllocateElements({}); 
					UI.AllocateElement(AllElements[UIElements::Win]); 

				}

			}
			else {

				TimeElapsedIdle += Window.GetFrameTime();

				if (TimeElapsedIdle > 3.f) {
					if (!GivenRankBefore) {
						UI.AllocateElements({});
						UI.AllocateElement(AllElements[UIElements::CrossHair]);
						UI.AllocateElement(AllElements[Rank]);

						if (Rank <= UIElements::WaveRatingB)
							Sounds.SetSoundState(Game::PICKUP, Game::PLAY);

					}
					GivenRankBefore = true;
				}
				else if (TimeElapsedIdle > 2.f) {
					UI.AllocateElements({});
					UI.AllocateElement(AllElements[UIElements::CrossHair]);
					UI.AllocateElement(AllElements[UIElements::WaveRatingA + rand() % 5]);
				}
				else if (TimeElapsedIdle > 1.f) {
					UI.AllocateElements({});
					UI.AllocateElement(AllElements[UIElements::CrossHair]);
					UI.AllocateElement(AllElements[UIElements::WaveRatingEmpty]);
				}


				if (Sounds.GetMusicState(Game::Music::IDLE) != sf::SoundSource::Playing) {
					UI.AllocateElements({});
					UI.AllocateElement(AllElements[UIElements::CrossHair]);

					CurretGameState = Game::GAMESTATE_WAVE;
					Sounds.SetMusicState(Game::Music::IDLE, Game::STOP);
					Sounds.SetMusicState(Game::BATTLE, Game::PLAYFROMSTARTANDREPEAT);
				}
			}

			GameClock.restart();
			T += Window.GetFrameTime();
			Window.SetFrameCount(Window.GetFrameCount() + 1);
			Window.SetTimeOpened(Window.GetTimeOpened() + Window.GetFrameTime());

			LDEngine::Core::HandleInput(Camera, 10.0, 0.15, Window, true, true);

			std::array<bool, 6> CollisionSides = { false,false,false,false,false,false };

			for (auto & AABB : Boxes) {
				AABB.Collision(Camera.Position, 1.8, .75, CollisionSides);
			}

			Jumping = CollisionSides[Collisions::PositiveY] && SpaceBar;

			CameraMovement(Camera, CollisionSides, Window.GetFrameTime(), Jumping);

			Camera.PrevView = Camera.View;
			Camera.View = LDEngine::Core::ViewMatrix(Camera.Position + Vector3f(0., 1.5, 0.), Camera.Rotation);

			Rendering.EnemySystem.HandleEnemies(Window, Camera, Boxes, Sounds, PlayerHealth, TowerHealth);

			std::cout << PlayerHealth << ' ' << TowerHealth << '\n'; 

		}
		else {
			Window.GetRawWindow()->pollEvent(Event); 
		}


		if (PlayerHealth <= 0 || TowerHealth <= 0) {
			for (int Sound = 0; Sound < 9; Sound++)
				Sounds.SetSoundState(Game::Sound(Sound), Game::STOP); 
			for (int Music = 0; Music < 3; Music++)
				Sounds.SetMusicState(Game::Music(Music), Game::STOP);
			UI.AllocateElements({}); 
			UI.AllocateElement(AllElements[UIElements::Loose]); 
			Status = LOST; 
		}

		if (Status != INGAME) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		}

		if (Status == INGAME) {
			UI.AllocateElement(PlayerHealthBar[glm::clamp(PlayerHealth / 100, 0, 9)]);
			UI.AllocateElement(BaseHealthBar[glm::clamp(TowerHealth / 1000, 0, 9)]);
		}
		
		UI.DrawUI(Window, Status == INGAME);

		if (Status != INGAME)
			Window.GetRawWindow()->display(); 

		if(Status == INGAME)
		Rendering.Draw(Window, Camera, Weapon, UI); 

		if (Status == INGAME) {
			UI.Remove_Last();
			UI.Remove_Last();
		}

		if (Status != INGAME) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				Running = false; 
		}

	}
}

void LDEngine::Rendering::Pipeline::Pipeline::CameraMovement(Control::Camera & Camera, std::array<bool, 6> & CollisionSides, float FrameTime, bool SpaceBar)
{
	if (CollisionSides[Collisions::PositiveY] || CollisionSides[Collisions::NegativeY]) {
		Gravity = Falling;
		CurrentSpeed = 0.; 
	}

	if (SpaceBar) {
		CurrentSpeed = MaxJumpSpeed; 
		Gravity = Jumping;
	}


	CurrentSpeed -= GravityAcceleration * FrameTime; 
	CurrentSpeed = std::max(CurrentSpeed, -MaxGravitySpeed); 



	Camera.Position.y += CurrentSpeed * FrameTime;
}
