#include "../Shared/D2Shared.hpp"

/////////////////////////////////////////////////
//
//	Global variables

extern D2ModuleImportStrc* trap;
extern D2GameConfigStrc* config;
extern OpenD2ConfigStrc* openConfig;

#define Log_WarnAssert(x, y)	if(!x) { trap->Warning(__FILE__, __LINE__, ##x); return y; }
#define Log_ErrorAssert(x, y)	if(!x) { trap->Error(__FILE__, __LINE__, ##x); return y; }