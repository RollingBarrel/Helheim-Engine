#include "LoreCollectible.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameObject.h"
#include "GameManager.h"
#include "HudController.h"


#include "ScriptComponent.h"
#include "BoxColliderComponent.h"
#include "TextComponent.h"

CREATE(LoreCollectible)
{
	CLASS(owner);

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
	if (mGameObject->GetComponent(ComponentType::TEXT))
	{
		mLoreText = static_cast<TextComponent*>(mGameObject->GetComponent(ComponentType::TEXT))->GetText();
	}

}

void LoreCollectible::Update()
{
	if (!GameManager::GetInstance()->IsPaused() && mInteractUsed) {
		mInteractUsed = false;
	}
}

void LoreCollectible::OnCollisionEnter(CollisionData* collisionData)
{

	GameObject* collisionGO = collisionData->collidedWith;

	//Make prompt appear 
	if (!mInteractUsed)
	{
		GameManager::GetInstance()->GetHud()->SetInteract(true);
	}
	

	if (collisionGO->GetTag() == "Player" && App->GetInput()->GetKey(Keys::Keys_F) == KeyState::KEY_DOWN ||
		App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN) {

		if (GameManager::GetInstance()->GetHud())
		{
			GameManager::GetInstance()->SetPaused(true, false);
			if (mLoreText)GameManager::GetInstance()->GetHud()->SetCollectibleText(mLoreText->data());
			GameManager::GetInstance()->GetHud()->SetScreen(SCREEN::COLLECTIBLE, true);
			GameManager::GetInstance()->GetHud()->SetInteract(false);
			mInteractUsed = true;
		}

	}


}

void LoreCollectible::OnCollisionExit(CollisionData* collisionData)
{
	
	GameManager::GetInstance()->GetHud()->SetInteract(false);
	
}
