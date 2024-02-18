#include <string>
using namespace std;

#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);
string GetLogBuffer();

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1240
#define SCREEN_HEIGHT 740
#define FULLSCREEN false
#define	RESIZEABLE true
#define TITLE "Super Awesome Engine"

#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

#endif /* _GLOBALS_H_ */
