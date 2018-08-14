#pragma once

#include <SFML/Audio.hpp>
#include <array>

namespace LDEngine {
	namespace Game {
		
		enum Sound {EMPTYCLIP, EXPLOSION, FLAMETHROWER ,PICKUP, PISTOL, ROCKETLAUNCHER, SHOTGUN, SMG, SNIPER};
		enum Music { INTRO, IDLE, BATTLE  };

		enum State { PLAY, STOP, PAUSE, PLAYANDREPEAT, PLAYFROMSTART, PLAYFROMSTARTANDREPEAT};

		class SoundPipeline {


			std::array<sf::SoundBuffer, 9> SoundBuffers;
			std::array<sf::SoundBuffer, 3> MusicBuffers;

			std::array<sf::Sound, 9> Sounds; 
			std::array<sf::Sound, 3> Musics; 
		public: 
			void LoadSounds();
			void SetSoundState(Sound Sound, State State);
			void SetMusicState(Music Music, State State);
			int GetMusicState(Music Music); 
		}; 

	}
}