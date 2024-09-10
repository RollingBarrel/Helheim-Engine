#include <string>
#include "FlickeringLight.h"
#include "Application.h"
#include "GameObject.h"
#include "GameManager.h"

#include "ScriptComponent.h"

#include "Component.h"
#include "MeshRendererComponent.h"

CREATE(FlickeringLight)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mStartingTime);
	MEMBER(MemberType::FLOAT, mLoopDuration);
	MEMBER(MemberType::FLOAT, mTimeOffset);
	SEPARATOR("Blackouts");
	SEPARATOR("1");
	MEMBER(MemberType::FLOAT, mBlackout1.mTime);
	MEMBER(MemberType::FLOAT, mBlackout1.mDuration);
	SEPARATOR("2");
	MEMBER(MemberType::FLOAT, mBlackout2.mTime);
	MEMBER(MemberType::FLOAT, mBlackout2.mDuration);
	SEPARATOR("3");
	MEMBER(MemberType::FLOAT, mBlackout3.mTime);
	MEMBER(MemberType::FLOAT, mBlackout3.mDuration);
	SEPARATOR("4");
	MEMBER(MemberType::FLOAT, mBlackout4.mTime);
	MEMBER(MemberType::FLOAT, mBlackout4.mDuration);
	SEPARATOR("5");
	MEMBER(MemberType::FLOAT, mBlackout5.mTime);
	MEMBER(MemberType::FLOAT, mBlackout5.mDuration);
	SEPARATOR("6");
	MEMBER(MemberType::FLOAT, mBlackout6.mTime);
	MEMBER(MemberType::FLOAT, mBlackout6.mDuration);
	SEPARATOR("7");
	MEMBER(MemberType::FLOAT, mBlackout7.mTime);
	MEMBER(MemberType::FLOAT, mBlackout7.mDuration);
	SEPARATOR("8");
	MEMBER(MemberType::FLOAT, mBlackout8.mTime);
	MEMBER(MemberType::FLOAT, mBlackout8.mDuration);
	SEPARATOR("9");
	MEMBER(MemberType::FLOAT, mBlackout9.mTime);
	MEMBER(MemberType::FLOAT, mBlackout9.mDuration);
	SEPARATOR("10");
	MEMBER(MemberType::FLOAT, mBlackout10.mTime);
	MEMBER(MemberType::FLOAT, mBlackout10.mDuration);
	SEPARATOR("11");
	MEMBER(MemberType::FLOAT, mBlackout11.mTime);
	MEMBER(MemberType::FLOAT, mBlackout11.mDuration);
	SEPARATOR("12");
	MEMBER(MemberType::FLOAT, mBlackout12.mTime);
	MEMBER(MemberType::FLOAT, mBlackout12.mDuration);
	SEPARATOR("13");
	MEMBER(MemberType::FLOAT, mBlackout13.mTime);
	MEMBER(MemberType::FLOAT, mBlackout13.mDuration);
	SEPARATOR("14");
	MEMBER(MemberType::FLOAT, mBlackout14.mTime);
	MEMBER(MemberType::FLOAT, mBlackout14.mDuration);
	SEPARATOR("15");
	MEMBER(MemberType::FLOAT, mBlackout15.mTime);
	MEMBER(MemberType::FLOAT, mBlackout15.mDuration);
	//SEPARATOR("16");
	//MEMBER(MemberType::FLOAT, mBlackout16.mTime);
	//MEMBER(MemberType::FLOAT, mBlackout16.mDuration);
	//SEPARATOR("17");
	//MEMBER(MemberType::FLOAT, mBlackout17.mTime);
	//MEMBER(MemberType::FLOAT, mBlackout17.mDuration);
	//SEPARATOR("18");
	//MEMBER(MemberType::FLOAT, mBlackout18.mTime);
	//MEMBER(MemberType::FLOAT, mBlackout18.mDuration);
	//SEPARATOR("19");
	//MEMBER(MemberType::FLOAT, mBlackout19.mTime);
	//MEMBER(MemberType::FLOAT, mBlackout19.mDuration);
	//SEPARATOR("20");
	//MEMBER(MemberType::FLOAT, mBlackout20.mTime);
	//MEMBER(MemberType::FLOAT, mBlackout20.mDuration);

	END_CREATE;
}

FlickeringLight::FlickeringLight(GameObject* owner) : Script(owner) {}

void FlickeringLight::Start()
{
	//Starting time setup
	mTimeOffset = fmod(mTimeOffset, mLoopDuration);
	mTimer = fmod(mStartingTime + mTimeOffset, mLoopDuration);

	//Checks if there is a spotlight or pointlight component
	mLightComp = mGameObject->GetComponent(ComponentType::SPOTLIGHT);
	if (!mLightComp)
	{
		mLightComp = mGameObject->GetComponent(ComponentType::POINTLIGHT);
	}

	//Checks if there is a mesh renderer attached to it
	mMeshRenderComp = static_cast<MeshRendererComponent*>(mGameObject->GetComponent(ComponentType::MESHRENDERER));
	if (mMeshRenderComp) 
	{
		mMeshRenderComp->CreateUniqueMaterial();
	}

	//Initialize the blackouts
	flickering.push_back(mBlackout1);
	flickering.push_back(mBlackout2);
	flickering.push_back(mBlackout3);
	flickering.push_back(mBlackout4);
	flickering.push_back(mBlackout5);
	flickering.push_back(mBlackout6);
	flickering.push_back(mBlackout7);
	flickering.push_back(mBlackout8);
	flickering.push_back(mBlackout9);
	flickering.push_back(mBlackout10);
	flickering.push_back(mBlackout11);
	flickering.push_back(mBlackout12);
	flickering.push_back(mBlackout13);
	flickering.push_back(mBlackout14);
	flickering.push_back(mBlackout15);
	//flickering.push_back(mBlackout16);
	//flickering.push_back(mBlackout17);
	//flickering.push_back(mBlackout18);
	//flickering.push_back(mBlackout19);
	//flickering.push_back(mBlackout20);
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
	if (mTimer - mStartingTime >= mLoopDuration) 
	{
		mTimer -= mLoopDuration;
	}

	UpdateLightState();

	//Updates lights and emissive
	if (mLightComp)
	{
		if (!mLightOn) 
		{
			mLightComp->SetEnable(false);
		}
		else 
		{
			mLightComp->SetEnable(true);
		}
	}
	if (mMeshRenderComp) 
	{
		if (!mLightOn)
		{
			mMeshRenderComp->SetEnableEmissiveTexture(false);
		}
		else
		{
			mMeshRenderComp->SetEnableEmissiveTexture(true);
		}
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
