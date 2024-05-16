#include <stdlib.h>
#include "GameApp.h"
#include "Globals.h"
#include "SDL.h"

//Select Nvidia or Amd GPU insted of integrated graphics
extern "C" {
	_declspec(dllexport) unsigned int NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

enum main_states
{
	MAIN_CREATION,  
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

GameApplication* gameApp = NULL;

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			LOG("Application Creation --------------");
			gameApp = new GameApplication();
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG("Application Init --------------");
			if (gameApp->Init() == false)
			{
				LOG("Application Init exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG("Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = gameApp->Update(App->GetDt());

			if (update_return == UPDATE_ERROR)
			{
				LOG("Application Update exits with error -----");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			LOG("Application CleanUp --------------");
			if (gameApp->CleanUp() == false)
			{
				LOG("Application CleanUp exits with error -----");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}

	}

	delete gameApp;
	return main_return;
}
