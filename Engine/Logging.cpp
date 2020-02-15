#include "Diablo2.hpp"
#include <time.h>
#include <cstdio>

///////////////////////////////////////////////////////////
//
// LOGGING SYSTEM: Alternative to Fog.dll
// Also, this catches errors before they occur

extern OpenD2Modules currentModule;

namespace Log
{

	static fs_handle glogHandle = INVALID_HANDLE;
	static D2SystemInfoStrc gSystemInfo;
	static DWORD gdwLogFlags = 0;

	/*
	 *	Starts up the logging subsystem
	 *	@author eezstreet
	 */
	void InitSystem(const char* szLogHeader, const char* szGameName, OpenD2ConfigStrc* pOpenConfig)
	{
		// Logs are stored in homepath/<szLogHeader><YY><MM><DD>.txt
		char szLogName[MAX_D2PATH];
		tm* tp;
		time_t curTime = 0;
		int nRealYear;

		if (pOpenConfig == nullptr)
		{
			return;
		}

		gdwLogFlags = pOpenConfig->dwLogFlags;
		if (gdwLogFlags == 0)
		{	// no log flags means that we can't log!
			return;
		}

		if (glogHandle != INVALID_HANDLE)
		{	// already initialized
			return;
		}

		// Print system info to log
		Sys::GetSystemInfo(&gSystemInfo);

		time(&curTime);
		tp = gmtime(&curTime);
		nRealYear = tp->tm_year + 1900; // we will need this later
		tp->tm_mon++;	// month starts at 0
		tp->tm_year %= 100; // constrain to 0-99 range

		snprintf(szLogName, MAX_D2PATH, "%s%02i%02i%02i.txt", szLogHeader, tp->tm_year, tp->tm_mon, tp->tm_mday);

		FS::Open(szLogName, &glogHandle, FS_APPEND);

		if (glogHandle == INVALID_HANDLE)
		{	// didn't open it for some reason
			return;
		}

		// Write the log startup message to the log
		char szRealmStartupMessage[128];
		snprintf(szRealmStartupMessage, 128, "%s Realm startup at %02i-%02i-%02i %02i:%02i:%02i" NL,
			gSystemInfo.szComputerName, nRealYear, tp->tm_mon, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);

		FS::WritePlaintext(glogHandle, "---------------------------------------------------------------" NL);
		FS::WritePlaintext(glogHandle, szRealmStartupMessage);
		FS::WritePlaintext(glogHandle, "---------------------------------------------------------------" NL);

		Print(PRIORITY_VISUALS, "------------------------------------------------------");
		Print(PRIORITY_SYSTEMINFO, "%i-%02i-%02i %02i:%02i:%02i",
			nRealYear, tp->tm_mon, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
		Print(PRIORITY_SYSTEMINFO, "%s running under %s", GAME_NAME, gSystemInfo.szOSName);
		Print(PRIORITY_SYSTEMINFO, "--PROCESSOR:");
		Print(PRIORITY_SYSTEMINFO, "Vendor: %s", gSystemInfo.szProcessorVendor);
		Print(PRIORITY_SYSTEMINFO, "Model: %s", gSystemInfo.szProcessorModel);
		Print(PRIORITY_SYSTEMINFO, "Identifier: %s", gSystemInfo.szProcessorIdentifier);
		Print(PRIORITY_SYSTEMINFO, "Speed: %s", gSystemInfo.szProcessorSpeed);
		Print(PRIORITY_SYSTEMINFO, "--RAM:");
		Print(PRIORITY_SYSTEMINFO, "Physical: %s", gSystemInfo.szRAMPhysical);
		Print(PRIORITY_SYSTEMINFO, "Virtual: %s", gSystemInfo.szRAMVirtual);
		Print(PRIORITY_SYSTEMINFO, "Paging: %s", gSystemInfo.szRAMPaging);
		Print(PRIORITY_SYSTEMINFO, "--WORKING DIRECTORY: %s", gSystemInfo.szWorkingDirectory);
		Print(PRIORITY_VISUALS, "------------------------------------------------------");
	}

	/*
	 *	Shuts down the logging subsystem
	 *	@author eezstreet
	 */
	void Shutdown()
	{
		if (glogHandle == INVALID_HANDLE)
		{	// never initialized / already shut down
			return;
		}

		FS::CloseFile(glogHandle);
		glogHandle = INVALID_HANDLE;
	}

	/*
	 *	Write message to logfile
	 *	Note that this automatically adds a newline at the end
	 *	@author eezstreet
	 */
	void Print(OpenD2LogFlags nPriority, const char* szFormat, ...)
	{
		if (!(gdwLogFlags & nPriority))
		{	// we aren't allowed to print this message, as per user config
			return;
		}

		// format the arguments
		char buffer[2048];
		char buffer2[2048];
		va_list args;
		va_start(args, szFormat);
		vsnprintf(buffer, sizeof(buffer), szFormat, args);
		va_end(args);

		// add the time and newline character
		tm* tp;
		time_t curTime = 0;
		time(&curTime);
		tp = gmtime(&curTime);

		snprintf(buffer2, sizeof(buffer2), "%02i:%02i:%02i  %s" NL, tp->tm_hour, tp->tm_min, tp->tm_sec, buffer);

		// write and clean up
		FS::WritePlaintext(glogHandle, buffer2);
	}

	/*
	 *	Fire off a warning; a non game-breaking event.
	 *	@author	eezstreet
	 *	@param	bCondition:	If true, the warning will fire. If false, the warning will not fire.
	 *	@param	szFile: The name of the source file indicating the error.
	 *	@param	szCondition: The text of the condition.
	 */
#define MAX_WARNING_TEXT 1024
	void Warning(const char* szFile, const int nLine, const char* szCondition)
	{
		char szWarningMessage[MAX_WARNING_TEXT]{ 0 };

		snprintf(szWarningMessage, MAX_WARNING_TEXT,
			NL "Assertion Failure!" NL
			"File: %s" NL
			"Line: %i" NL "Code: %s", szFile, nLine, szCondition);
		Window::ShowMessageBox(SDL_MESSAGEBOX_WARNING, GAME_NAME " Warning", szWarningMessage);
		Print(PRIORITY_MESSAGE, szWarningMessage);
	}

	void Warning(const char* format, ...)
	{
		static char buffer[2048];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		Window::ShowMessageBox(SDL_MESSAGEBOX_WARNING, GAME_NAME " Warning", buffer);
		Print(PRIORITY_MESSAGE, buffer);
	}

	/*
	 *	Fire off an error; a game-breaking event.
	 */
	void Error(const char* szFile, const int nLine, const char* szCondition)
	{
		char szErrorMessage[MAX_WARNING_TEXT]{ 0 };

		snprintf(szErrorMessage, MAX_WARNING_TEXT,
			NL "Fatal Assertion Failure!" NL
			"File: %s" NL
			"Line: %i" NL "Code: %s", szFile, nLine, szCondition);
		Window::ShowMessageBox(SDL_MESSAGEBOX_ERROR, GAME_NAME " Error", szErrorMessage);
		Print(PRIORITY_CRASH, szErrorMessage);

		// pull us out of the main game loop
		currentModule = MODULE_NONE;

		exit(1);
	}

	void Error(const char* format, ...)
	{
		static char buffer[2048];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		Window::ShowMessageBox(SDL_MESSAGEBOX_ERROR, GAME_NAME " Error", buffer);
		Print(PRIORITY_CRASH, buffer);

		// pull us out of the main game loop
		currentModule = MODULE_NONE;

		exit(1);
	}
}