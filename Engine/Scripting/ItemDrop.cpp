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
	END_CREATE;
}

ItemDrop::ItemDrop(GameObject* owner) : Script(owner) {}

void ItemDrop::Init()
{
    std::vector<Component*> components;
    mGameObject->GetComponentsInChildren(ComponentType::ANIMATION, components);

    if (!components.empty())
    {
        mAnimation = static_cast<AnimationComponent*>(*components.begin());
        if (mAnimation)
        {
            mAnimation->SetIsPlaying(true);
        }
    }

    mDespawnTimer = 60.0f;

}

void ItemDrop::Start()
{
    ModuleScene* scene = App->GetScene();
    mPlayer = GameManager::GetInstance()->GetPlayer();
    
    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ItemDrop::OnCollisionEnter, this, std::placeholders::_1)));
    }

    Init();
}

void ItemDrop::Update()
{
    mDespawnTimer -= App->GetDt();
    if (mDespawnTimer <= 0.0f)
    {
        mGameObject->SetEnabled(false);
    }
}

void ItemDrop::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag().compare("Player") == 0)
    {
       

        PlayerController* playerScript = static_cast<PlayerController*>(static_cast<ScriptComponent*>(mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (playerScript != nullptr)
        {
            switch (mDropId)
            {
            case 1:
                if (playerScript->GetShieldPercetage() != 100.0f)
                {
                    playerScript->RechargeShield(mHealthRecovered);
                    mGameObject->SetEnabled(false);
                }
                break;
            case 2:
                playerScript->RechargeBattery(EnergyType::BLUE);
                mGameObject->SetEnabled(false);
                break;
            case 3:
                playerScript->RechargeBattery(EnergyType::RED);
                mGameObject->SetEnabled(false);
                break;
            default:
                break;
            }
        }



    }
}