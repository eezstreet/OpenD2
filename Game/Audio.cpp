#include "Audio_SDL.hpp"

namespace Audio
{

	// Initialize audio
	void Init(OpenD2ConfigStrc* openconfig)
	{
		// For now, just use SDL audio
		Audio_SDL::Init(openconfig);
	}

	// De-initialize audio
	void Shutdown()
	{
		// For now, just use SDL audio
		Audio_SDL::Shutdown();
	}

	sfx_handle RegisterSound(char* szAudioFile)
	{
		// For now, just use SDL audio
		return Audio_SDL::RegisterSound(szAudioFile);
	}

	mus_handle RegisterMusic(char* szAudioFile)
	{
		// For now, just use SDL audio
		return Audio_SDL::RegisterMusic(szAudioFile);
	}

	void PlaySound(sfx_handle handle, int loops)
	{
		// For now, just use SDL audio
		return Audio_SDL::PlaySound(handle, loops);
	}

	// Change master volume
	void SetMasterVolume(float fNewVolume)
	{
		// Just pipe it into SDL for now
		Audio_SDL::SetMasterVolume(fNewVolume);
	}

	// Change music volume
	void SetMusicVolume(float fNewVolume)
	{
		// Just pipe it into SDL for now
		Audio_SDL::SetMusicVolume(fNewVolume);
	}

	// Change sound volume
	void SetSoundVolume(float fNewVolume)
	{
		// Just pipe it into SDL for now
		Audio_SDL::SetSoundVolume(fNewVolume);
	}
}