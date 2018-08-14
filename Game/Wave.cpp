#include "Wave.h"
#include <iostream>

LDEngine::Game::WaveState GetRank(int PlayerHealthStart, int PlayerHealth, int BaseHealthStart, int BaseHealth) {

	int HealthDifferencePlayer = PlayerHealthStart - PlayerHealth; 
	int HealthDifferenceBase = BaseHealthStart - BaseHealth; 

	std::cout << HealthDifferencePlayer << ' ' << HealthDifferenceBase << '\n'; 

	for (int PossibleRank = 0; PossibleRank < 4; PossibleRank++) {
		if (HealthDifferencePlayer < 100 * (PossibleRank + 1) && HealthDifferenceBase < 1000 * (PossibleRank + 1)) {
			return LDEngine::Game::WaveState(PossibleRank);
		}
	}

	return LDEngine::Game::RANKE; 
}

LDEngine::Game::WaveState LDEngine::Game::Wave::HandleWave(EnemySystem & Enemies, Window & Window, Vector3f SpawnPoint, int PlayerHealth, int BaseHealth)
{
	if (Enemies.Enemies.size() == 0 && EnemySpawnTimes.size() == 0) {
		
		//get rank based on how enemies were killed every second 

		return GetRank(StartPlayerHealth, PlayerHealth, StartBaseHealth, BaseHealth); //test


	}

	TimeElapsedInWave += Window.GetFrameTime(); 

	std::vector<int> EraseLocations{}; 

	for (int PossibleSpawn = 0; PossibleSpawn < EnemySpawnTimes.size(); PossibleSpawn++) {
		if (TimeElapsedInWave > EnemySpawnTimes[PossibleSpawn]) {
			EraseLocations.push_back(PossibleSpawn);
			Enemies.AddEnemy(Enemy(SpawnPoint)); 
		}
	}
	for (int Erase = EraseLocations.size() - 1; Erase >= 0; Erase--) {
		EnemySpawnTimes.erase(EnemySpawnTimes.begin() + EraseLocations[Erase]);
	}
	
	return ONGOING; 
}
