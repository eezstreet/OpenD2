#include "Audio_SDL.hpp"

namespace Audio_SDL
{
	static SoundCacheEntry* gpSoundCache;
	static bool gbSoundCacheFull = false;

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

		Log_WarnAssertVoidReturn(SDL_GetNumAudioDevices(false) >= pConfig->dwAudioDevice);
		Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Output Device: %s\n", SDL_GetAudioDeviceName(pConfig->dwAudioDevice, false));

		int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
		//int result = Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, pConfig->dwAudioChannels, 1024, SDL_GetAudioDeviceName(pConfig->dwAudioDevice, false), SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
		if (result == -1)
		{
			Log::Warning(__FILE__, __LINE__, "Could not open audio device.");
			return;
		}

		// Initialize the sound cache
		size_t dwSoundCacheSize = sizeof(SoundCacheEntry) * MAX_SDL_SOUNDFILES;
		gpSoundCache = (SoundCacheEntry*)malloc(dwSoundCacheSize);
		memset(gpSoundCache, 0, dwSoundCacheSize);
	}

	// Shut down the audio subsystem
	void Shutdown()
	{
		// Free the sound cache
		FlushAudioData();
		free(gpSoundCache);

		// We don't need anything other than WAV support.
		// However, if we wanted to support MP3 or OGG, we could use Mix_Quit here.

		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	// Find the next free slot in the sound table
	handle FindNextFreeSoundEntry(char* szAudioPath)
	{
		if (gbSoundCacheFull)
		{
			return INVALID_HANDLE;
		}

		// Hash it. If the current spot is occupied, continue until we find it.
		DWORD dwAttempts = 1;
		handle currentHandle = D2Lib::strhash(szAudioPath, strlen(szAudioPath), MAX_SDL_SOUNDFILES);
		while (dwAttempts < MAX_SDL_SOUNDFILES && gpSoundCache[currentHandle].szSoundPath[0] && D2Lib::stricmp(gpSoundCache[currentHandle].szSoundPath, szAudioPath))
		{
			currentHandle++;
			currentHandle %= MAX_SDL_SOUNDFILES;
			dwAttempts++;
		}

		if (dwAttempts >= MAX_SDL_SOUNDFILES)
		{
			// Ran out of possible attempts - thing's full, don't bother hashing anything else
			gbSoundCacheFull = true;
			return INVALID_HANDLE;
		}

		// Current handle is good
		return currentHandle;
	}

	// Load a WAV into memory
	bool LoadWAV(char* szAudioPath, BYTE** ppWavOutput, DWORD& dwSizeBytes)
	{
		D2MPQArchive* pArchive;
		
		// Find the file and determine how big it should be
		fs_handle file = FSMPQ::FindFile(szAudioPath, nullptr, &pArchive);
		if (file == INVALID_HANDLE)
		{
			return false;
		}

		dwSizeBytes = MPQ::FileSize(pArchive, file);
		if (dwSizeBytes == 0)
		{
			return false;
		}

		// allocate memory for it
		*ppWavOutput = (BYTE*)malloc(dwSizeBytes);
		if (!(*ppWavOutput))
		{
			return false;
		}

		MPQ::ReadFile(pArchive, file, *ppWavOutput, dwSizeBytes);

		return true;
	}

	// Register a sound effect for playing
	sfx_handle RegisterSound(char* szAudioPath)
	{
		sfx_handle ourHandle = FindNextFreeSoundEntry(szAudioPath);
		if (ourHandle == INVALID_HANDLE)
		{
			Log::Warning(__FILE__, __LINE__, "Ran out of free sound space.");
			return ourHandle;
		}

		if (gpSoundCache[ourHandle].szSoundPath[0])
		{
			// Found what was already existing
			return ourHandle;
		}

		BYTE* pWavData = nullptr;
		DWORD dwWavSize = 0;
		if (!LoadWAV(szAudioPath, &pWavData, dwWavSize))
		{
			Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Failed to load %s", szAudioPath);
			return INVALID_HANDLE;
		}

		SDL_RWops* sdlFile = SDL_RWFromMem(pWavData, dwWavSize);
		Log_WarnAssert(sdlFile);
		gpSoundCache[ourHandle].pChunk = Mix_LoadWAV_RW(sdlFile, true);
		D2Lib::strncpyz(gpSoundCache[ourHandle].szSoundPath, szAudioPath, MAX_D2PATH);
		return ourHandle;
	}

	// Register a music effect for playing
	mus_handle RegisterMusic(char* szAudioPath)
	{
		mus_handle ourHandle = FindNextFreeSoundEntry(szAudioPath);
		if (ourHandle == INVALID_HANDLE)
		{
			Log::Warning(__FILE__, __LINE__, "Ran out of free sound space.");
			return ourHandle;
		}

		if (gpSoundCache[ourHandle].szSoundPath[0])
		{
			// Found what was already existing
			return ourHandle;
		}

		BYTE* pWavData = nullptr;
		DWORD dwWavSize = 0;
		if (!LoadWAV(szAudioPath, &pWavData, dwWavSize))
		{
			Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Failed to load %s", szAudioPath);
			return INVALID_HANDLE;
		}

		SDL_RWops* sdlFile = SDL_RWFromMem(pWavData, dwWavSize);
		Log_WarnAssert(sdlFile);
		gpSoundCache[ourHandle].pMusic = Mix_LoadMUS_RW(sdlFile, true);
		D2Lib::strncpyz(gpSoundCache[ourHandle].szSoundPath, szAudioPath, MAX_D2PATH);
		return ourHandle;
	}

	// Flush out all the audio data. Should be done when loading a new act.
	void FlushAudioData()
	{
		for (DWORD i = 0; i < MAX_SDL_SOUNDFILES; i++)
		{
			if (gpSoundCache[i].szSoundPath[0])
			{
				if (gpSoundCache[i].pChunk)
				{
					Mix_FreeChunk(gpSoundCache[i].pChunk);
				}
				else if (gpSoundCache[i].pMusic)
				{
					Mix_FreeMusic(gpSoundCache[i].pMusic);
				}
			}
		}
	}

	// Play a sound
	void PlaySound(sfx_handle handle, int loops)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_SDL_SOUNDFILES)
		{
			return;
		}

		if (!gpSoundCache[handle].pChunk)
		{
			return;
		}

		Mix_PlayChannel(-1, gpSoundCache[handle].pChunk, 0);
	}

	// Play music
	void PlayMusic(mus_handle handle, int loops)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_SDL_SOUNDFILES)
		{
			return;
		}

		if (!gpSoundCache[handle].pChunk)
		{
			return;
		}

		Mix_PlayMusic(gpSoundCache[handle].pMusic, loops);
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