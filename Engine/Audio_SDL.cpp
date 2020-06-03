#include "Audio_SDL.hpp"
#include "../Shared/D2HashMap.hpp"
#include "FileSystem.hpp"
#include "Logging.hpp"

namespace Audio_SDL
{
	static HashMap<char, AudioChunk, 128, MAX_D2PATH> gpSoundCache;

	// Start up the audio subsystem
	void Init(OpenD2ConfigStrc* pConfig)
	{
		// We don't need anything other than WAV support.
		// However, if we wanted to support MP3 or OGG, we could use Mix_Init here.

		// Init the audio subsystem of SDL if it wasn't already
		if (!(SDL_WasInit(0) & SDL_INIT_AUDIO))
		{
			SDL_InitSubSystem(SDL_INIT_AUDIO);
		}

		// Determine the proper audio device from commandline arguments (or from what is supported)
		Log::Print(OpenD2LogFlags::PRIORITY_SYSTEMINFO, "SDL_Mixer init...\n");
		Log::Print(OpenD2LogFlags::PRIORITY_SYSTEMINFO, "Enumerate audio devices:\n");
		for (DWORD i = 0; i < SDL_GetNumAudioDevices(false); i++)
		{
			const char* szDeviceName = SDL_GetAudioDeviceName(i, false);
			Log::Print(OpenD2LogFlags::PRIORITY_SYSTEMINFO, "Device %d: %s\n", i, szDeviceName);
		}

		if (pConfig->dwAudioDevice > SDL_GetNumAudioDevices(false))
		{
			pConfig->dwAudioDevice = 0;
		}

		Log_WarnAssertVoidReturn(SDL_GetNumAudioDevices(false) >= pConfig->dwAudioDevice)
		Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Output Device: %s\n", SDL_GetAudioDeviceName(pConfig->dwAudioDevice, false));

		int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
		//int result = Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, pConfig->dwAudioChannels, 1024, SDL_GetAudioDeviceName(pConfig->dwAudioDevice, false), SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
		if (result == -1)
		{
			Log::Warning(__FILE__, __LINE__, "Could not open audio device.");
			return;
		}
	}

	// Shut down the audio subsystem
	void Shutdown()
	{
		// Free the sound cache
		FlushAudioData();

		// We don't need anything other than WAV support.
		// However, if we wanted to support MP3 or OGG, we could use Mix_Quit here.

		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	// Load a WAV into memory
	bool LoadWAV(const char *szAudioPath, BYTE** ppWavOutput, size_t& dwSizeBytes)
	{
		// Find the file and determine how big it should be
		fs_handle f;
		dwSizeBytes = FS::Open(szAudioPath, &f, FS_READ, true);
		if (f == INVALID_HANDLE)
		{
			return false;
		}

		if (dwSizeBytes == 0)
		{
			FS::CloseFile(f);
		}

		// allocate memory for it
		*ppWavOutput = (BYTE*)malloc(dwSizeBytes);
		if (!(*ppWavOutput))
		{
			return false;
		}

		FS::Read(f, *ppWavOutput, dwSizeBytes);
		FS::CloseFile(f);

		return true;
	}

	// Register a sound effect for playing
	sfx_handle RegisterSound(const char *szAudioPath)
	{
		bool bAlreadyAvailable = false;
		sfx_handle ourHandle = gpSoundCache.NextFree(szAudioPath, bAlreadyAvailable);
		if (ourHandle == INVALID_HANDLE)
		{
			Log::Warning(__FILE__, __LINE__, "Ran out of free sound space.");
			return ourHandle;
		}

		if (bAlreadyAvailable)
		{
			return ourHandle;
		}

		BYTE* pWavData = nullptr;
		size_t dwWavSize = 0;
		if (!LoadWAV(szAudioPath, &pWavData, dwWavSize))
		{
			Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Failed to load %s", szAudioPath);
			return INVALID_HANDLE;
		}

		SDL_RWops* sdlFile = SDL_RWFromMem(pWavData, dwWavSize);
		Log_WarnAssert(sdlFile)
		gpSoundCache[ourHandle].bIsMusic = false;
		gpSoundCache[ourHandle].data.pChunk = Mix_LoadWAV_RW(sdlFile, true);
		return ourHandle;
	}

	// Register a music effect for playing
	mus_handle RegisterMusic(const char *szAudioPath)
	{
		bool bAlreadyPresent = false;
		mus_handle ourHandle = gpSoundCache.NextFree(szAudioPath, bAlreadyPresent);
		if (ourHandle == INVALID_HANDLE)
		{
			Log::Warning(__FILE__, __LINE__, "Ran out of free sound space.");
			return ourHandle;
		}

		if(bAlreadyPresent)
		{
			// Found what was already existing
			return ourHandle;
		}

		BYTE* pWavData = nullptr;
		size_t dwWavSize = 0;
		if (!LoadWAV(szAudioPath, &pWavData, dwWavSize))
		{
			Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Failed to load %s", szAudioPath);
			return INVALID_HANDLE;
		}

		SDL_RWops* sdlFile = SDL_RWFromMem(pWavData, dwWavSize);
		Log_WarnAssert(sdlFile)
		gpSoundCache[ourHandle].bIsMusic = true;
		gpSoundCache[ourHandle].data.pMusic = Mix_LoadMUS_RW(sdlFile, true);
		return ourHandle;
	}

	// Flush out all the audio data. Should be done when loading a new act.
	void FlushAudioData()
	{
		gpSoundCache = HashMap<char, AudioChunk, 128, MAX_D2PATH>();
	}

	// Play a sound
	void PlaySound(sfx_handle handle, int loops)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_SDL_SOUNDFILES)
		{
			return;
		}

		if (gpSoundCache[handle].bIsMusic)
		{
			// is actually music
			return;
		}

		Mix_PlayChannel(-1, gpSoundCache[handle].data.pChunk, 0);
	}

	// Play music
	void PlayMusic(mus_handle handle, int loops)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_SDL_SOUNDFILES)
		{
			return;
		}

		if (!gpSoundCache[handle].bIsMusic)
		{	// is not actually music
			return;
		}

		Mix_PlayMusic(gpSoundCache[handle].data.pMusic, loops);
	}

	void PauseAudio()
	{
		Mix_Pause(-1);
		Mix_PauseMusic();
	}

	void ResumeAudio()
	{
		Mix_Resume(-1);
		Mix_ResumeMusic();
	}

	// Change the master volume
	void SetMasterVolume(float volume)
	{
		SetMusicVolume(volume);
		SetSoundVolume(volume);
	}

	// Change the music volume
	void SetMusicVolume(float volume)
	{
		Mix_VolumeMusic(volume);
	}

	// Change the sound volume
	void SetSoundVolume(float volume)
	{
		Mix_Volume(-1, volume);
	}
}