#include "FlickeringLight.h"
#include "Application.h"
#include "GameObject.h"
#include "GameManager.h"

#include "ScriptComponent.h"

#include "SpotLightComponent.h"
#include "PointLightComponent.h"

CREATE(FlickeringLight)
{
	CLASS(owner);
	//MEMBER(MemberType::FLOAT, mLoopDuration);
	END_CREATE;
}

FlickeringLight::FlickeringLight(GameObject* owner) : Script(owner) {}

void FlickeringLight::Start()
{
	//Check if the gameobject has some type of light
	mPointLight = mGameObject->GetComponent<PointLightComponent>();
	mSpotLight = mGameObject->GetComponent<SpotLightComponent>();

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
	mTimer += App->GetDt();

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
}

void FlickeringLight::UpdateLightState() 
{
	for (const auto& element : flickering) {
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
