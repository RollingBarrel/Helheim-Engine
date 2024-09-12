#include "ItemDrop.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "BoxColliderComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"
#include "Math/MathFunc.h"
#include "Keys.h"
#include "ModuleInput.h"
#include "HudController.h"

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
    mAlreadyUsed = false;
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
        if (playerScript != nullptr && !mAlreadyUsed)
        {
            switch (mDropId)
            {
            case 1:
                if (playerScript->GetShieldPercetage() != 100.0f)
                {
                    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_PICK, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
                    playerScript->RechargeShield(mHealthRecovered);
                    mAlreadyUsed = true;
                    mGameObject->SetEnabled(false);
                }
                break;
            case 2:
                if (playerScript->GetEnergyType() == EnergyType::RED)
                {
                    GameManager::GetInstance()->GetHud()->SetPickupPrompt(true);
                    if (App->GetInput()->GetKey(Keys::Keys_F) == KeyState::KEY_DOWN ||
                        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN)
                    {
                        GameManager::GetInstance()->GetHud()->SetPickupPrompt(false);
                        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_PICK, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
                        playerScript->RechargeBattery(EnergyType::BLUE);
                        mAlreadyUsed = true;
                        mGameObject->SetEnabled(false);
                    }
                }
                else
                {
                    GameManager::GetInstance()->GetHud()->SetPickupPrompt(false);
                    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_PICK, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
                    playerScript->RechargeBattery(EnergyType::BLUE);
                    mAlreadyUsed = true;
                    mGameObject->SetEnabled(false);
                }
                break;
            case 3:             
                if (playerScript->GetEnergyType() == EnergyType::BLUE)
                {
                    GameManager::GetInstance()->GetHud()->SetPickupPrompt(true);
                    if (App->GetInput()->GetKey(Keys::Keys_F) == KeyState::KEY_DOWN ||
                        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN)
                    {
                        GameManager::GetInstance()->GetHud()->SetPickupPrompt(false);
                        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_PICK, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
                        playerScript->RechargeBattery(EnergyType::RED);
                        mAlreadyUsed = true;
                        mGameObject->SetEnabled(false);
                    }
                }
                else
                {
                    GameManager::GetInstance()->GetHud()->SetPickupPrompt(false);
                    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_PICK, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
                    playerScript->RechargeBattery(EnergyType::RED);
                    mAlreadyUsed = true;
                    mGameObject->SetEnabled(false);
                }
                break;
            default:
                break;
            }
        }



    }
}