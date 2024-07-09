#ifdef SCRIPTING_EXPORTS
#define SCRIPTING_API __declspec(dllexport)
#else
#define SCRIPTING_API __declspec(dllimport)
#endif
