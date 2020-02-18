#pragma once

#include "../Shared/D2Shared.hpp"

// Audio.cpp
namespace Audio
{
	void Init(OpenD2ConfigStrc* openconfig);
	void Shutdown();
	sfx_handle RegisterSound(char* szAudioFile);
	mus_handle RegisterMusic(char* szAudioFile);
	void PlaySound(sfx_handle handle, int loops);
	void PlayMusic(mus_handle handle, int loops);
	void PauseAudio();
	void ResumeAudio();
	void SetMasterVolume(float volume);
	void SetMusicVolume(float volume);
	void SetSoundVolume(float volume);
}