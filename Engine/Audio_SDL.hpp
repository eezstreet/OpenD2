#pragma once
#include "Diablo2.hpp"
#include <SDL2_mixer/SDL_mixer.h>

#define MAX_SDL_SOUNDFILES	4096

namespace Audio_SDL
{
	void Init(OpenD2ConfigStrc* pConfig);
	void Shutdown();

	sfx_handle RegisterSound(const char *szSoundPath);
	mus_handle RegisterMusic(const char *szMusicPath);
	void FlushAudioData();

	void PlaySound(sfx_handle handle, int loops);
	void PlayMusic(mus_handle handle, int loops);
	void PauseAudio();
	void ResumeAudio();

	void SetMasterVolume(float volume);
	void SetMusicVolume(float volume);
	void SetSoundVolume(float volume);

	struct AudioChunk
	{
		union {
			Mix_Chunk* pChunk;
			Mix_Music* pMusic;
		} data;

		bool bIsMusic = false;

		~AudioChunk()
		{
			if (bIsMusic && data.pMusic)
			{
				Mix_FreeMusic(data.pMusic);
			}
			else if (data.pChunk)
			{
				Mix_FreeChunk(data.pChunk);
			}
		}

		AudioChunk(): data{} {};
	};
}
