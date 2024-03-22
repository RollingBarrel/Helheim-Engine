#include "HelheimEvent.h"

HelheimEvent::HelheimEvent(HelheimEventType type)
	: mType(type) {}

HelheimEvent::HelheimEvent(HelheimEventType type, EventData* data)
	: mType(type), mData(data) {}
