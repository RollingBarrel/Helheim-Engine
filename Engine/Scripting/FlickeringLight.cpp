#include <string>
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
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mLoopDuration);
	MEMBER(MemberType::FLOAT, mStartingTime);
	MEMBER(MemberType::FLOAT, mTimeOffset);
	SEPARATOR("Blackouts");
	SEPARATOR("1");
	MEMBER(MemberType::FLOAT, b1.mTime);
	MEMBER(MemberType::FLOAT, b1.mDuration);
	SEPARATOR("2");
	MEMBER(MemberType::FLOAT, b2.mTime);
	MEMBER(MemberType::FLOAT, b2.mDuration);
	SEPARATOR("3");
	MEMBER(MemberType::FLOAT, b3.mTime);
	MEMBER(MemberType::FLOAT, b3.mDuration);
	SEPARATOR("4");
	MEMBER(MemberType::FLOAT, b4.mTime);
	MEMBER(MemberType::FLOAT, b4.mDuration);
	SEPARATOR("5");
	MEMBER(MemberType::FLOAT, b5.mTime);
	MEMBER(MemberType::FLOAT, b5.mDuration);
	SEPARATOR("6");
	MEMBER(MemberType::FLOAT, b6.mTime);
	MEMBER(MemberType::FLOAT, b6.mDuration);
	SEPARATOR("7");
	MEMBER(MemberType::FLOAT, b7.mTime);
	MEMBER(MemberType::FLOAT, b7.mDuration);
	SEPARATOR("8");
	MEMBER(MemberType::FLOAT, b8.mTime);
	MEMBER(MemberType::FLOAT, b8.mDuration);
	SEPARATOR("9");
	MEMBER(MemberType::FLOAT, b9.mTime);
	MEMBER(MemberType::FLOAT, b9.mDuration);
	SEPARATOR("10");
	MEMBER(MemberType::FLOAT, b10.mTime);
	MEMBER(MemberType::FLOAT, b10.mDuration);
	SEPARATOR("11");
	MEMBER(MemberType::FLOAT, b11.mTime);
	MEMBER(MemberType::FLOAT, b11.mDuration);
	SEPARATOR("12");
	MEMBER(MemberType::FLOAT, b12.mTime);
	MEMBER(MemberType::FLOAT, b12.mDuration);
	SEPARATOR("13");
	MEMBER(MemberType::FLOAT, b13.mTime);
	MEMBER(MemberType::FLOAT, b13.mDuration);
	SEPARATOR("14");
	MEMBER(MemberType::FLOAT, b14.mTime);
	MEMBER(MemberType::FLOAT, b14.mDuration);
	SEPARATOR("15");
	MEMBER(MemberType::FLOAT, b15.mTime);
	MEMBER(MemberType::FLOAT, b15.mDuration);
	SEPARATOR("16");
	MEMBER(MemberType::FLOAT, b16.mTime);
	MEMBER(MemberType::FLOAT, b16.mDuration);
	SEPARATOR("17");
	MEMBER(MemberType::FLOAT, b17.mTime);
	MEMBER(MemberType::FLOAT, b17.mDuration);
	SEPARATOR("18");
	MEMBER(MemberType::FLOAT, b18.mTime);
	MEMBER(MemberType::FLOAT, b18.mDuration);
	SEPARATOR("19");
	MEMBER(MemberType::FLOAT, b19.mTime);
	MEMBER(MemberType::FLOAT, b19.mDuration);
	SEPARATOR("20");
	MEMBER(MemberType::FLOAT, b20.mTime);
	MEMBER(MemberType::FLOAT, b20.mDuration);

	END_CREATE;
}

FlickeringLight::FlickeringLight(GameObject* owner) : Script(owner) {}

void FlickeringLight::Start()
{
	//Checks range
	if (mTimeOffset<mStartingTime || mTimeOffset>mStartingTime + mLoopDuration) 
	{
		mTimeOffset = mStartingTime;
	}

	//Starting time setup
	mTimer = fmod(mStartingTime,mLoopDuration);

	//Check if the gameobject has some type of light and registers its intensity
	mPointLight = static_cast<PointLightComponent*>(mGameObject->GetComponent(ComponentType::POINTLIGHT));
	mSpotLight = static_cast<SpotLightComponent*>(mGameObject->GetComponent(ComponentType::SPOTLIGHT));

	//Initialize the blackouts
	flickering.push_back(b1);
	flickering.push_back(b2);
	flickering.push_back(b3);
	flickering.push_back(b4);
	flickering.push_back(b5);
	flickering.push_back(b6);
	flickering.push_back(b7);
	flickering.push_back(b8);
	flickering.push_back(b9);
	flickering.push_back(b10);
	flickering.push_back(b11);
	flickering.push_back(b12);
	flickering.push_back(b13);
	flickering.push_back(b14);
	flickering.push_back(b15);
	flickering.push_back(b16);
	flickering.push_back(b17);
	flickering.push_back(b18);
	flickering.push_back(b19);
	flickering.push_back(b20);
}

void FlickeringLight::Update()
{
	//Checks range of variables
	if (mSpeed <= 0)
	{
		mSpeed = 0;
	}
	if (mStartingTime < 0) 
	{
		mStartingTime = 0;
	}

	//Updates time
	mTimer += App->GetDt() * mSpeed;

	if (mPointLight) 
	{
		UpdateLightState();

		if (!mLightOn) 
		{
			mPointLight->SetEnable(false);
		}
		else 
		{
			mPointLight->SetEnable(true);
		}
	}
	if (mSpotLight) 
	{
		UpdateLightState();

		if (!mLightOn)
		{
			mSpotLight->SetEnable(false);
		}
		else
		{
			mSpotLight->SetEnable(true);
		}
	}

	if (mTimer - mStartingTime >= mLoopDuration) 
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
