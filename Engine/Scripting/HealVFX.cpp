#include "HealVFX.h"
#include "Application.h"
#include "GameObject.h"


#include "ScriptComponent.h"
#include "ImageComponent.h"

CREATE(HealVFX)
{
    CLASS(owner);
    
    END_CREATE;
}

HealVFX::HealVFX(GameObject* owner) : Script(owner) {}

void HealVFX::Init()
{
}

void HealVFX::Start()
{
    mSpriteSheet = reinterpret_cast<ImageComponent*>(mGameObject->GetComponent(ComponentType::IMAGE));
    if (mSpriteSheet) mSpriteSheet->PlayAnimation();
}

void HealVFX::Update()
{
    if (mTimer.Delay(2.0f)) {
        mGameObject->SetEnabled(false);
        mSpriteSheet->StopAnimation();
    }
		
}
