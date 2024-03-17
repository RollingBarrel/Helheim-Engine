#pragma once

#include "Module.h"
#include <concurrent_queue.h>
#include <vector>

struct HelheimEvent;
enum class HelheimEventType;

class ModuleEvent : public Module
{
public:
	ModuleEvent();
	~ModuleEvent();

	update_status Update(float dt) override;
	bool CleanUp() override;

	void AddObserverToEvent(const HelheimEventType type, Module* moduleToAdd);
	void RemoveObserverFromEvent(const HelheimEventType type, const Module* moduleToRemove);

	void AddEvent(const HelheimEvent& newEvent);
private:
	void ProcessEvents();
	void ProcessEvent(const HelheimEvent& e);

	concurrency::concurrent_queue<HelheimEvent> mEventQueue;
	std::vector<Module*> mObservers[(int)HelheimEventType::COUNT];
};

