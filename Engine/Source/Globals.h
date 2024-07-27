#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#ifdef ENGINE
#define LOG(format, ...) EngineLog(__FILE__, __LINE__, format, __VA_ARGS__);
void EngineLog(const char file[], int line, const char* format, ...);
#else
	#ifdef _DEBUG
	#define LOG(format, ...) CoreLog(__FILE__, __LINE__, format, __VA_ARGS__);
	#else
	#define LOG(format, ...);
	#endif // _DEBUG
void ENGINE_API CoreLog(const char file[], int line, const char* format, ...);
#endif // ENGINE




enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FULLSCREEN false
#define	RESIZEABLE true
#define TITLE "HellHeim Engine"

#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

#endif /* _GLOBALS_H_ */