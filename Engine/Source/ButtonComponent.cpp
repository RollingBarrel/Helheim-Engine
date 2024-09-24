#include "Application.h"

#include "GameObject.h"
#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "ScriptComponent.h"
#include "Component.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "Transform2DComponent.h"
#include "ImageComponent.h"
#include "CanvasComponent.h"

ButtonComponent::ButtonComponent(GameObject* owner, bool active) : Component(owner, ComponentType::BUTTON) 
{
	mImage = static_cast<ImageComponent*>(mOwner->GetComponent(ComponentType::IMAGE));
	mTransform2D = static_cast<Transform2DComponent*>(mOwner->GetComponent(ComponentType::TRANSFORM2D));
}

ButtonComponent::ButtonComponent(GameObject* owner) : Component(owner, ComponentType::BUTTON) 
{
	mImage = static_cast<ImageComponent*>(mOwner->GetComponent(ComponentType::IMAGE));
	mTransform2D = static_cast<Transform2DComponent*>(mOwner->GetComponent(ComponentType::TRANSFORM2D));
}

ButtonComponent::ButtonComponent(const ButtonComponent& component, GameObject* owner) : Component(owner, ComponentType::BUTTON)
{
	this->mImage = component.mImage;
	this->mTransform2D = component.mTransform2D;

    for (int i = 0; i < (int)EventType::COUNT; ++i)
    {
        for (const auto& handler : component.mEventHandlers[i]) 
        {
            mEventHandlers[i].push_back(handler);
        }
    }
}

ButtonComponent:: ~ButtonComponent() 
{
    for (int i = 0; i < (int)EventType::COUNT; i++)
    {
        for (int j = 0; j < mEventHandlers->size(); j++) 
        {
            mEventHandlers[i][j] = nullptr;
        }
        mEventHandlers[i].clear();
    }
    mEventHandlers->clear();
}

void ButtonComponent::Update()
{
	float2 mouseAbsolute(App->GetInput()->GetGlobalMousePosition());

	float normalizedX = -1.0f + 2.0f * static_cast<float>(mouseAbsolute.x - App->GetWindow()->GetGameWindowsPosition().x) / static_cast<float>(App->GetWindow()->GetGameWindowsSize().x);
	float normalizedY = 1.0f - 2.0f * static_cast<float>(mouseAbsolute.y - App->GetWindow()->GetGameWindowsPosition().y) / static_cast<float>(App->GetWindow()->GetGameWindowsSize().y);

	float mouseX = normalizedX;
	float mouseY = normalizedY;

	float2 canvasSize = static_cast<CanvasComponent*>(mImage->FindCanvasOnParents(mImage->GetOwner())->GetComponent(ComponentType::CANVAS))->GetSize();
	float2 minImagePoint = mTransform2D->GetGlobalMatrix().TranslatePart().xy().Mul(float2(2.0f, 2.0f)).Div(canvasSize) - mTransform2D->GetSize().Div(canvasSize);
	float2 maxImagePoint = mTransform2D->GetGlobalMatrix().TranslatePart().xy().Mul(float2(2.0f, 2.0f)).Div(canvasSize) + mTransform2D->GetSize().Div(canvasSize);
	
	if (mouseX >= minImagePoint.x && mouseY >= minImagePoint.y && mouseX <= maxImagePoint.x && mouseY <= maxImagePoint.y)
	{
		KeyState mouseButtonLeftState = App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT);

		if (mouseButtonLeftState == KeyState::KEY_UP && mCurrentEvent == EventType::PRESS)
		{
			mCurrentEvent = EventType::CLICK;
			TriggerEvent(EventType::CLICK);
		}
		else if (mouseButtonLeftState == KeyState::KEY_DOWN || mouseButtonLeftState == KeyState::KEY_REPEAT)
		{
			mCurrentEvent = EventType::PRESS;
			TriggerEvent(EventType::PRESS);
		}
		else
		{
			if (mCurrentEvent == EventType::HOVER) return;
			mCurrentEvent = EventType::HOVER;
			TriggerEvent(EventType::HOVER);
		}
		return;
	}

	if (mCurrentEvent == EventType::HOVER || mCurrentEvent == EventType::PRESS)
	{
		mCurrentEvent = EventType::HOVEROFF;
		TriggerEvent(EventType::HOVEROFF);
	}
	else if (mCurrentEvent != EventType::NONE)
	{
		mCurrentEvent = EventType::NONE;
	}
}

Component* ButtonComponent::Clone(GameObject* owner) const
{
    return new ButtonComponent(*this, owner);
}

void ButtonComponent::AddEventHandler(EventType eventType, std::function<void()>* handler) 
{
    mEventHandlers[static_cast<int>(eventType)].push_back(handler);
}

void ButtonComponent::TriggerEvent(EventType eventType) 
{
	switch (eventType) {
	case EventType::CLICK : LOG("Event: CLICK"); break;
	case EventType::HOVER: LOG("Event: HOVER"); break;
	case EventType::PRESS: LOG("Event: PRESS"); break;
	case EventType::HOVEROFF: LOG("Event: HOVEROFF"); break;
	}

    for (auto& handler : mEventHandlers[static_cast<int>(eventType)]) 
    {
        (*handler)();
    }
}

void ButtonComponent::Save(JsonObject& obj) const 
{
    Component::Save(obj);
}

void ButtonComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    Component::Load(data, uidPointerMap);
}



