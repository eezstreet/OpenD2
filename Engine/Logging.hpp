#pragma once
#include "../Shared/D2Shared.hpp"

// Logging.cpp
#define Log_WarnAssert(x) if(!(x)) { Log::Warning(__FILE__, __LINE__, "" #x); }
#define Log_ErrorAssert(x) if(!(x)) { Log::Error(__FILE__, __LINE__, "" #x); }

#define Log_WarnAssertReturn(x, y) if(!(x)) { Log::Warning(__FILE__, __LINE__, "" #x); return y; }
#define Log_ErrorAssertReturn(x, y) if(!(x)) { Log::Error(__FILE__, __LINE__, "" #x); return y; }

#define Log_WarnAssertVoidReturn(x) if(!(x)) { Log::Warning(__FILE__, __LINE__, "" #x); return; }
#define Log_ErrorAssertVoidReturn(x) if(!(x)) { Log::Error(__FILE__, __LINE__, "" #x); return; }

namespace Log
{
	void InitSystem(const char* szLogHeader, const char* szGameName, OpenD2ConfigStrc* pOpenConfig);
	void Shutdown();
	void Print(OpenD2LogFlags nPriority, const char* szFormat, ...);
	void Warning(const char* szFile, const int nLine, const char* szCondition);
	void Error(const char* szFile, const int nLine, const char* szCondition);
	void Warning(const char* warningFormat, ...);
	void Error(const char* errorFormat, ...);
}