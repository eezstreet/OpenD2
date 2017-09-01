#include "Diablo2.hpp"
#include <time.h>
#include <cstdio>

///////////////////////////////////////////////////////////
//
// LOGGING SYSTEM: Alternative to Fog.dll
// Also, this catches errors before they occur

static fs_handle glogHandle = INVALID_HANDLE;
static D2SystemInfoStrc gSystemInfo;
static DWORD gdwLogFlags = 0;

/*
 *	Starts up the logging subsystem
 *	@author eezstreet
 */
void Log_InitSystem(const char* szLogHeader, const char* szGameName, OpenD2ConfigStrc* pOpenConfig)
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
	Sys_GetSystemInfo(&gSystemInfo);

	time(&curTime);
	tp = gmtime(&curTime);
	nRealYear = tp->tm_year + 1900; // we will need this later
	tp->tm_mon++;	// month starts at 0
	tp->tm_year %= 100; // constrain to 0-99 range

	snprintf(szLogName, MAX_D2PATH, "%s%02i%02i%02i.txt", szLogHeader, tp->tm_year, tp->tm_mon, tp->tm_mday);

	FS_Open(szLogName, &glogHandle, FS_APPEND);

	if (glogHandle == INVALID_HANDLE)
	{	// didn't open it for some reason
		return;
	}

	// Write the log startup message to the log
	char szRealmStartupMessage[128];
	snprintf(szRealmStartupMessage, 128, "%s Realm startup at %02i-%02i-%02i %02i:%02i:%02i" NL,
		gSystemInfo.szComputerName, nRealYear, tp->tm_mon, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);

	FS_WritePlaintext(glogHandle, "---------------------------------------------------------------" NL);
	FS_WritePlaintext(glogHandle, szRealmStartupMessage);
	FS_WritePlaintext(glogHandle, "---------------------------------------------------------------" NL);

	Log_Print(PRIORITY_VISUALS,		"------------------------------------------------------");
	Log_Print(PRIORITY_SYSTEMINFO,	"%i-%02i-%02i %02i:%02i:%02i",
		nRealYear, tp->tm_mon, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
	Log_Print(PRIORITY_SYSTEMINFO,	"%s running under %s", GAME_NAME, gSystemInfo.szOSName);
	Log_Print(PRIORITY_SYSTEMINFO,	"--PROCESSOR:");
	Log_Print(PRIORITY_SYSTEMINFO,	"Vendor: %s", gSystemInfo.szProcessorVendor);
	Log_Print(PRIORITY_SYSTEMINFO,	"Model: %s", gSystemInfo.szProcessorModel);
	Log_Print(PRIORITY_SYSTEMINFO,	"Identifier: %s", gSystemInfo.szProcessorIdentifier);
	Log_Print(PRIORITY_SYSTEMINFO,	"Speed: %s", gSystemInfo.szProcessorSpeed);
	Log_Print(PRIORITY_SYSTEMINFO,	"--RAM:");
	Log_Print(PRIORITY_SYSTEMINFO,	"Physical: %s", gSystemInfo.szRAMPhysical);
	Log_Print(PRIORITY_SYSTEMINFO,	"Virtual: %s", gSystemInfo.szRAMVirtual);
	Log_Print(PRIORITY_SYSTEMINFO,	"Paging: %s", gSystemInfo.szRAMPaging);
	Log_Print(PRIORITY_SYSTEMINFO,	"--WORKING DIRECTORY: %s", gSystemInfo.szWorkingDirectory);
	Log_Print(PRIORITY_VISUALS,		"------------------------------------------------------");
}

/*
 *	Shuts down the logging subsystem
 *	@author eezstreet
 */
void Log_Shutdown()
{
	if (glogHandle == INVALID_HANDLE)
	{	// never initialized / already shut down
		return;
	}

	FS_CloseFile(glogHandle);
	glogHandle = INVALID_HANDLE;
}

/*
 *	Write message to logfile
 *	Note that this automatically adds a newline at the end
 *	@author eezstreet
 */
void Log_Print(OpenD2LogFlags nPriority, char* szFormat, ...)
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
	vsnprintf(buffer, 2048, szFormat, args);
	va_end(args);

	// add the time and newline character
	tm* tp;
	time_t curTime = 0;
	time(&curTime);
	tp = gmtime(&curTime);

	snprintf(buffer2, 2048, "%02i:%02i:%02i  %s" NL, tp->tm_hour, tp->tm_min, tp->tm_sec, buffer);

	// write and clean up
	FS_WritePlaintext(glogHandle, buffer2);
}

/*
 *	Fire off a warning; a non game-breaking event.
 *	@author	eezstreet
 *	@param	bCondition:	If true, the warning will fire. If false, the warning will not fire.
 *	@param	szFile: The name of the source file indicating the error.
 *	@param	szCondition: The text of the condition.
 */
#define MAX_WARNING_TEXT 1024
void Log_Warning(char* szFile, int nLine, char* szCondition)
{
	char szWarningMessage[MAX_WARNING_TEXT]{ 0 };

	snprintf(szWarningMessage, MAX_WARNING_TEXT, 
		NL "Assertion Failure!" NL
		"File: %s" NL
		"Line: %i" NL "Code: %s", szFile, nLine, szCondition);
	D2Win_ShowMessageBox(SDL_MESSAGEBOX_WARNING, GAME_NAME " Warning", szWarningMessage);
	Log_Print(PRIORITY_MESSAGE, szWarningMessage);
}

/*
 *	Fire off an error; a game-breaking event.
 */
extern OpenD2Modules currentModule;
void Log_Error(char* szFile, int nLine, char* szCondition)
{
	char szErrorMessage[MAX_WARNING_TEXT]{ 0 };

	snprintf(szErrorMessage, MAX_WARNING_TEXT,
		NL "Fatal Assertion Failure!" NL
		"File: %s" NL
		"Line: %i" NL "Code: %s", szFile, nLine, szCondition);
	D2Win_ShowMessageBox(SDL_MESSAGEBOX_ERROR, GAME_NAME " Error", szErrorMessage);
	Log_Print(PRIORITY_CRASH, szErrorMessage);

	// pull us out of the main game loop
	currentModule = MODULE_NONE;
}