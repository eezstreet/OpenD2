#pragma once
#include "Diablo2.hpp"
#include "../Libraries/sdl/SDL_mixer.h"

#define MAX_SDL_SOUNDFILES	4096

namespace Audio_SDL
{
	void Init(OpenD2ConfigStrc* pConfig);
	void Shutdown();

	sfx_handle RegisterSound(char* szSoundPath);
	mus_handle RegisterMusic(char* szMusicPath);
	void FlushAudioData();

	void PlaySound(sfx_handle handle, int loops);

	void SetMasterVolume(float fNewVolume);
	void SetMusicVolume(float fNewVolume);
	void SetSoundVolume(float fNewVolume);

	struct SoundCacheEntry
	{
		char szSoundPath[MAX_D2PATH];
		union {
			Mix_Chunk* pChunk;
			Mix_Music* pMusic;
		};
	};
}