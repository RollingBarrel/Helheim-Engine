#include "LoreCollectible.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameObject.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "HudController.h"

#include "MathFunc.h"

#include "ScriptComponent.h"
#include "BoxColliderComponent.h"
#include "TextComponent.h"
#include "MeshRendererComponent.h"

CREATE(LoreCollectible)
{
	CLASS(owner);
	MEMBER(MemberType::GAMEOBJECT, mMeshGO);
	MEMBER(MemberType::INT, mLoreIndex);
	END_CREATE;
}

LoreCollectible::LoreCollectible(GameObject* owner) : Script(owner) {}

void LoreCollectible::Init()
{
};

void LoreCollectible::Start()
{
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&LoreCollectible::OnCollisionEnter, this, std::placeholders::_1)));
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_EXIT, new std::function<void(CollisionData*)>(std::bind(&LoreCollectible::OnCollisionExit, this, std::placeholders::_1)));
		mCollider->SetColliderType(ColliderType::STATIC);
	}
		
	mLoreText = &(mTextList[mLoreIndex]);
	mTitleText = &(mTitleList[mLoreIndex]);
	mSubtitleText = &(mSubtitleList[mLoreIndex]);
}

void LoreCollectible::Update()
{
	if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN ||
		App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
	{
		GameManager::GetInstance()->GetHud()->DisableCollectible();

	}

	if (mColliding) CheckDistance();
}

void LoreCollectible::OnCollisionEnter(CollisionData* collisionData)
{

	GameObject* collisionGO = collisionData->collidedWith;

	//Make prompt appear 
	if (collisionGO->GetTag() == "Player")
	{
		GameManager::GetInstance()->GetHud()->SetInteract(true);
		mColliding = true;
	
		if ( App->GetInput()->GetKey(Keys::Keys_F) == KeyState::KEY_DOWN ||
			App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN) {

			if (GameManager::GetInstance()->GetHud())
			{
				GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_INTERACT, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
				GameManager::GetInstance()->SetPaused(true, false);
				if (mLoreText)GameManager::GetInstance()->GetHud()->SetCollectibleText(mLoreText->data(), mTitleText->data(), mSubtitleText->data());
				GameManager::GetInstance()->GetHud()->SetScreen(SCREEN::COLLECTIBLE, true);
			}
		}
	}
}

void LoreCollectible::OnCollisionExit(CollisionData* collisionData)
{
	
	GameManager::GetInstance()->GetHud()->SetInteract(false);
	
}

void LoreCollectible::CheckDistance()
{
	float3 playerPosition = GameManager::GetInstance()->GetPlayer()->GetWorldPosition();
	float distanceToCollectible = (playerPosition - mGameObject->GetWorldPosition()).Length();
	//float3 playerToCollectible = (mGameObject->GetWorldPosition() - playerPosition).Normalized();
	if (distanceToCollectible > 2.0f) 
	{
		GameManager::GetInstance()->GetHud()->SetInteract(false);
		mColliding = false;
	}
}


