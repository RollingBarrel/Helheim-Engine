#include "ItemDrop.h"
#include "PlayerController.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameManager.h"
#include "Math/MathFunc.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"

CREATE(ItemDrop)
{
	CLASS(owner);

    MEMBER(MemberType::INT, mDropId);
    MEMBER(MemberType::FLOAT, mEnergyRecovered);
    MEMBER(MemberType::FLOAT, mHealthRecovered);
    MEMBER(MemberType::FLOAT, mActivationRange);

	END_CREATE;
}

ItemDrop::ItemDrop(GameObject* owner) : Script(owner) {}

void ItemDrop::Start()
{
    ModuleScene* scene = App->GetScene();
    mPlayer = GameManager::GetInstance()->GetPlayer();


    std::vector<Component*> components;
    mGameObject->GetComponentsInChildren(ComponentType::ANIMATION, components);


    if (!components.empty())
    {
        mAnimation = reinterpret_cast<AnimationComponent*>(*components.begin());

        if (mAnimation)
        {
            mAnimation->SetIsPlaying(true);
        }
    }
    
}

void ItemDrop::Update()
{
    if (IsPlayerInRange(mActivationRange))
    {
        mGameObject->SetEnabled(false);

        PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
        if (playerScript != nullptr)
        {
            switch (mDropId)
            {
            case 1:
                playerScript->RechargeShield(mHealthRecovered);
                break;
            case 2:
                //playerScript->RechargeShield(mHealthRecovered); // upgrade to shotgun function here
                break;
            case 3:
                //playerScript->RechargeShield(mHealthRecovered); // upgrade to machine gun function here
                break;
            default:
                break;
            }
            
        }
    }
}

bool ItemDrop::IsPlayerInRange(float range)
{
    float distance = 0.0f;
    distance = (mPlayer) ? mGameObject->GetPosition().Distance(mPlayer->GetPosition()) : inf;

    return (distance <= range);
}