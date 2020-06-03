#include "Audio_SDL.hpp"
#include "Audio.hpp"

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

	sfx_handle RegisterSound(const char *szAudioFile)
	{
		// For now, just use SDL audio
		return Audio_SDL::RegisterSound(szAudioFile);
	}

	mus_handle RegisterMusic(const char *szAudioFile)
	{
		// For now, just use SDL audio
		return Audio_SDL::RegisterMusic(szAudioFile);
	}

	void PlaySound(sfx_handle handle, int loops)
	{
		// For now, just use SDL audio
		return Audio_SDL::PlaySound(handle, loops);
	}

	void PlayMusic(mus_handle music, int loops)
	{
		return Audio_SDL::PlayMusic(music, loops);
	}

	void PauseAudio()
	{
		return Audio_SDL::PauseAudio();
	}

	void ResumeAudio()
	{
		return Audio_SDL::ResumeAudio();
	}

	// Change master volume
	void SetMasterVolume(float volume)
	{
		// Just pipe it into SDL for now
		Audio_SDL::SetMasterVolume(volume);
	}

	// Change music volume
	void SetMusicVolume(float volume)
	{
		// Just pipe it into SDL for now
		Audio_SDL::SetMusicVolume(volume);
	}

	// Change sound volume
	void SetSoundVolume(float volume)
	{
		// Just pipe it into SDL for now
		Audio_SDL::SetSoundVolume(volume);
	}
}