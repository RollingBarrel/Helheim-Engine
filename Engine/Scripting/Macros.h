#include "pch.h"
#ifdef SCRIPTING_EXPORTS
#define SCRIPTING_API __declspec(dllexport)
#else
#define SCRIPTING_API __declspec(dllimport)
#endif

#define UNSERIALIZED
#define DISPLAY
#define TOOLTIP(tooltip)
#define RANGE(min,max)
#define HEADER(header) 
#define SPACE 