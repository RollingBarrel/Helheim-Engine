#include "HealVFX.h"
#include "Application.h"
#include "GameObject.h"

#include "PointLightComponent.h"
#include "ScriptComponent.h"

CREATE(HealVFX)
{
    CLASS(owner);
    MEMBER(MemberType::FLOAT, mDeactivateTime);
    MEMBER(MemberType::FLOAT, mFadeingTime);
    END_CREATE;
}

HealVFX::HealVFX(GameObject* owner) : Script(owner) 
{
}

void HealVFX::Start()
{
    mPointLightTop = reinterpret_cast<PointLightComponent*>(mGameObject->GetChildren()[0]->GetComponent(ComponentType::POINTLIGHT));
    mPointLightBot = reinterpret_cast<PointLightComponent*>(mGameObject->GetChildren()[1]->GetComponent(ComponentType::POINTLIGHT));
    mIntensity = mPointLightTop->GetIntensity();
    mPointLightBot->SetIntensity(mIntensity);
}

void HealVFX::Update()
{
    if (mTimer.Delay(mFadeingTime))
    {
        mPointLightTop->SetIntensity(mIntensity * (mTimer.GetTimePassed() - mFadeingTime) / (mDeactivateTime - mFadeingTime));
        mPointLightBot->SetIntensity(mIntensity * (mTimer.GetTimePassed() - mFadeingTime) / (mDeactivateTime - mFadeingTime));
    }
    if (mTimer.Delay(mDeactivateTime))
    {
        mPointLightTop->SetIntensity(mIntensity);
        mPointLightBot->SetIntensity(mIntensity);
        mGameObject->SetEnabled(false);
    }
		
}
