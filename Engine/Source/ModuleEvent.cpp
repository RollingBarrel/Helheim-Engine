#include "ModuleEvent.h"

ModuleEvent::ModuleEvent()
{
}

ModuleEvent::~ModuleEvent()
{
}

update_status ModuleEvent::Update(float dt)
{
	ProcessEvents();
	return UPDATE_CONTINUE;
}

bool ModuleEvent::CleanUp()
{
	while (!mEventQueue.empty())
	{
		HelheimEvent e(HelheimEventType::UNKNOWN);
		mEventQueue.try_pop(e);
	}
	return true;
}

void ModuleEvent::AddObserverToEvent(const HelheimEventType type, Module* moduleToAdd)
{
	mObservers[static_cast<int>(type)].push_back(moduleToAdd);
}

void ModuleEvent::RemoveObserverFromEvent(const HelheimEventType type, const Module* moduleToRemove)
{
	for (auto it = mObservers[static_cast<int>(type)].begin(); it != mObservers[static_cast<int>(type)].end(); ++it)
	{
		if (*it == moduleToRemove)
		{
			mObservers[static_cast<int>(type)].erase(it);
			return;
		}
	}
}

void ModuleEvent::AddEvent(const HelheimEvent& newEvent)
{
	mEventQueue.push(newEvent);
}

void ModuleEvent::ProcessEvents()
{
	while (!mEventQueue.empty())
	{
		HelheimEvent e(HelheimEventType::UNKNOWN);
		mEventQueue.try_pop(e);
		ProcessEvent(e);
	}
}

void ModuleEvent::ProcessEvent(HelheimEvent& e)
{
	for (Module* m : mObservers[static_cast<int>(e.mType)])
	{
		m->ReceiveEvent(e);
	}
}
