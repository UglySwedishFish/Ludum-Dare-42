#include "Sound.h"
#include <string>

const std::string SoundNames[9] = { "Empty", "Explosion", "FlameThrower", "Pickup", "Pistol", "Rocketlauncher", "Shotgun", "SMG", "Sniper" }; 
const std::string MusicNames[3] = { "Intro", "Idle", "Battle" }; 

void LDEngine::Game::SoundPipeline::LoadSounds()
{

	for (int Sound = 0; Sound < Sounds.size(); Sound++) {
		std::string File = "Sound/" + SoundNames[Sound] + ".wav"; 
		SoundBuffers[Sound].loadFromFile(File); 
		Sounds[Sound].setBuffer(SoundBuffers[Sound]); 
	}

	for (int Music = 0; Music < Musics.size(); Music++) {
		std::string File = "Music/" + MusicNames[Music] + ".wav";
		MusicBuffers[Music].loadFromFile(File);
		Musics[Music].setBuffer(MusicBuffers[Music]);
	}

}

void LDEngine::Game::SoundPipeline::SetSoundState(Sound Sound, State State)
{
	if (State == PLAYANDREPEAT || State == PLAYFROMSTARTANDREPEAT) {
		Sounds[Sound].setLoop(true); 
	}
	else {
		Sounds[Sound].setLoop(false);
	}

	if (State == PLAYFROMSTART || State == PLAYFROMSTARTANDREPEAT) {
		Sounds[Sound].setPlayingOffset(sf::seconds(0)); 
	}

	if (State == PAUSE) {
		Sounds[Sound].pause(); 
	}
	else if (State == STOP) {
		Sounds[Sound].stop(); 
	}
	else {

		if(Sounds[Sound].getStatus() != sf::SoundSource::Playing || State == PLAYFROMSTART || State == PLAYFROMSTARTANDREPEAT)
		Sounds[Sound].play(); 
	}

}

void LDEngine::Game::SoundPipeline::SetMusicState(Music Music, State State)
{
	if (State == PLAYANDREPEAT || State == PLAYFROMSTARTANDREPEAT) {
		Musics[Music].setLoop(true);
	}
	else {
		Musics[Music].setLoop(false);
	}

	if (State == PLAYFROMSTART || State == PLAYFROMSTARTANDREPEAT) {
		Musics[Music].setPlayingOffset(sf::seconds(0));
	}

	if (State == PAUSE) {
		Musics[Music].pause();
	}
	else if (State == STOP) {
		Musics[Music].stop();
	}
	else {
		if (Musics[Music].getStatus() != sf::SoundSource::Playing || State == PLAYFROMSTART || State == PLAYFROMSTARTANDREPEAT)
		Musics[Music].play();
	}
}

int LDEngine::Game::SoundPipeline::GetMusicState(Music Music)
{
	return Musics[Music].getStatus(); 
}
