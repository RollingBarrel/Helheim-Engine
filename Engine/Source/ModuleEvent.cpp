#include "ModuleEvent.h"
#include "HelheimEvent.h"

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
	mObservers[(int)type].push_back(moduleToAdd);
}

void ModuleEvent::RemoveObserverFromEvent(const HelheimEventType type, const Module* moduleToRemove)
{
	for (auto it = mObservers[(int)type].begin(); it != mObservers[(int)type].end(); ++it)
	{
		if (*it == moduleToRemove)
		{
			mObservers[(int)type].erase(it);
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

void ModuleEvent::ProcessEvent(const HelheimEvent& e)
{
	for (Module* m : mObservers[(int)e.mType])
	{
		//m->ReceiveEvent(e);
	}
}
