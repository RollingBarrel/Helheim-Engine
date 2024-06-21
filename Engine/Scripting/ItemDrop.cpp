#include "ItemDrop.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "BoxColliderComponent.h"

#include "GameManager.h"
#include "PlayerController.h"
#include "Math/MathFunc.h"

CREATE(ItemDrop)
{
	CLASS(owner);

    MEMBER(MemberType::INT, mDropId);
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
    
    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ItemDrop::OnCollisionEnter, this, std::placeholders::_1)));
    }

}

void ItemDrop::Update()
{
}

void ItemDrop::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag().compare("Player") == 0)
    {
        mGameObject->SetEnabled(false);

        PlayerController* playerScript =  reinterpret_cast<PlayerController*>(reinterpret_cast<ScriptComponent*>(mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (playerScript != nullptr)
        {
            switch (mDropId)
            {
            case 1:
                playerScript->RechargeShield(mHealthRecovered);
                break;
            case 2:
                playerScript->RechargeBattery(EnergyType::BLUE);
                break;
            case 3:
                playerScript->RechargeBattery(EnergyType::RED);
                break;
            default:
                break;
            }
        }
    }
}