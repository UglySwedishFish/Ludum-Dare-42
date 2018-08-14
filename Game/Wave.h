#pragma once

#include "Enemy.h"

namespace LDEngine {
	namespace Game {

		enum GameState { GAMESTATE_WAVE, GAMESTATE_IDLE};
		enum WaveState {RANKA, RANKB, RANKC, RANKD, RANKE, ONGOING};


		class Wave {
			unsigned int TotalEnemyCount;
			std::vector<float> EnemySpawnTimes; 
			float TimeElapsedInWave; 

		public: 
			int StartPlayerHealth, StartBaseHealth;
			Wave(std::vector<float> SpawnTimes = {},int StartPlayerHealth = 1000, int StartBaseHealth = 10000) : TotalEnemyCount(SpawnTimes.size()), EnemySpawnTimes(SpawnTimes), TimeElapsedInWave(0.),
				StartPlayerHealth(StartPlayerHealth), StartBaseHealth(StartBaseHealth) {}
			WaveState HandleWave(EnemySystem & Enemies, Window & Window, Vector3f SpawnPoint, int PlayerHealth, int BaseHealth); //returns true if wave is done

		};
	}
}