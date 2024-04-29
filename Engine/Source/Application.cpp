#include "Application.h"
#include "Timer.h"

Application* App = nullptr;

Application::Application()
{
	App = this;
}

Application::~Application()
{
}

float Application::GetDt() const 
{ 
	return mCurrentTimer->GetDelta() / (float)1000; 
}