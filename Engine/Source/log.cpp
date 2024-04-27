#ifdef ENGINE
#include <windows.h>
#include <stdio.h>
#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ConsolePanel.h"


void EngineLog(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	if (App != nullptr) {
		((ConsolePanel*)App->GetEditor()->GetPanel(CONSOLEPANEL))->SetLog(tmp_string2);
	}
}
#else
	#ifdef _DEBUG
		#include <windows.h>
		#include <stdio.h>
		#include "Globals.h"
		#include "Application.h"
		#include "ModuleEditor.h"
		#include "ConsolePanel.h"
		
		
		void EngineLog(const char file[], int line, const char* format, ...)
		{
			static char tmp_string[4096];
			static char tmp_string2[4096];
			static va_list  ap;
		
			// Construct the string from variable arguments
			va_start(ap, format);
			vsprintf_s(tmp_string, 4096, format, ap);
			va_end(ap);
			sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
			OutputDebugString(tmp_string2);
		}
	#else
		void EngineLog(const char file[], int line, const char* format, ...)
		{

		}
	#endif
#endif