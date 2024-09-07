#include "FlickeringLight.h"
#include "Application.h"
#include "GameObject.h"
#include "GameManager.h"

#include "ScriptComponent.h"

#include "SpotLightComponent.h"
#include "PointLightComponent.h"

CREATE(FlickeringLight)
{
	CLASS(owner);MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mLoopDuration);
	MEMBER(MemberType::FLOAT, mStartingTime);
	MEMBER(MemberType::FLOAT, mSpeed);
	END_CREATE;
}

FlickeringLight::FlickeringLight(GameObject* owner) : Script(owner) {}

void FlickeringLight::Start()
{
	//Starting time setup
	mTimer = fmod(mStartingTime,mLoopDuration);

	//Check if the gameobject has some type of light and registers its intensity
	mPointLight = static_cast<PointLightComponent*>(mGameObject->GetComponent(ComponentType::POINTLIGHT));
	mSpotLight = static_cast<SpotLightComponent*>(mGameObject->GetComponent(ComponentType::SPOTLIGHT));

	//This script doen't work as intended if both types of lights are in the same go
	if (mPointLight)
	{
		mLightIntensity = mPointLight->GetIntensity();
	}
	if (mSpotLight)
	{
		mLightIntensity = mSpotLight->GetIntensity();
	}

	//Initialize the blackouts
	flickering.push_back(Blackout(0.5f, 0.05f));
	flickering.push_back(Blackout(0.6f, 0.05f));
	flickering.push_back(Blackout(0.8f, 0.1f));
	flickering.push_back(Blackout(1.7f, 0.6f));
	flickering.push_back(Blackout(4.0f, 0.25f));
	flickering.push_back(Blackout(6.0f, 0.1f));
	flickering.push_back(Blackout(6.4f, 0.1f));
	flickering.push_back(Blackout(8.0f, 0.6f));
}

void FlickeringLight::Update()
{
	mTimer += App->GetDt() * mSpeed;

	if (mPointLight) 
	{
		UpdateLightState();

		if (!mLightOn) 
		{
			mPointLight->SetIntensity(0.0f);
		}
		else 
		{
			mPointLight->SetIntensity(mLightIntensity);
		}
	}
	if (mSpotLight) 
	{
		UpdateLightState();

		if (!mLightOn)
		{
			mSpotLight->SetIntensity(0.0f);
		}
		else
		{
			mSpotLight->SetIntensity(mLightIntensity);
		}
	}

	if (mTimer >= mLoopDuration) 
	{
		mTimer -= mLoopDuration;
	}
}

void FlickeringLight::UpdateLightState()
{
	for (const auto& element : flickering) 
	{
		if (element.mTime < mTimer && mTimer < element.mTime + element.mDuration)
		{
			mLightOn = false;
			break;
		}
		else
		{
			mLightOn = true;
		}
	}
}
