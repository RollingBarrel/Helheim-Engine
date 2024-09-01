#include "LoreCollectible.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameObject.h"
#include "GameManager.h"
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
		mLoreText = static_cast<TextComponent*>(mGameObject->GetComponent(ComponentType::TEXT))->GetText();

	if (mMeshGO && mMeshGO->GetComponent(ComponentType::MESHRENDERER)) 
		mMesh = static_cast<MeshRendererComponent*>(mMeshGO->GetComponent(ComponentType::MESHRENDERER));
	
}

void LoreCollectible::Update()
{
	/*if (!isColliding)
	{
		GameManager::GetInstance()->GetHud()->SetInteract(false);
	}*/

	if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
	{
		GameManager::GetInstance()->GetHud()->DisableCollectible();
	}

	if (mUsed|| mInteractTimer.Delay(5.0f))
		GameManager::GetInstance()->GetHud()->SetInteract(false);

	if (mInteractTimer.Delay(10.0f))
		mUsed = false;

	if (mMesh) ColorChange();
}

void LoreCollectible::OnCollisionEnter(CollisionData* collisionData)
{

	GameObject* collisionGO = collisionData->collidedWith;

	//Make prompt appear 
	if (!mUsed && collisionGO->GetTag() == "Player")
	{
		GameManager::GetInstance()->GetHud()->SetInteract(true);
	
		if ( App->GetInput()->GetKey(Keys::Keys_F) == KeyState::KEY_DOWN ||
			App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN) {

			if (GameManager::GetInstance()->GetHud())
			{
				GameManager::GetInstance()->SetPaused(true, false);
				if (mLoreText)GameManager::GetInstance()->GetHud()->SetCollectibleText(mLoreText->data());
				GameManager::GetInstance()->GetHud()->SetScreen(SCREEN::COLLECTIBLE, true);
				GameManager::GetInstance()->GetHud()->SetInteract(false);
				mUsed = true;
			}
		}
	}
}

void LoreCollectible::OnCollisionExit(CollisionData* collisionData)
{
	
	GameManager::GetInstance()->GetHud()->SetInteract(false);
	
}

void LoreCollectible::ColorChange()
{
	Clamp01(mColor);

	if (!mChange) 
	{
		mColor = Lerp(mColor, 0.0f, App->GetDt()*1.4);
		mMesh->SetBaseColorFactor(float4(mColor, mColor, mColor, mColor));
		if (mColor < 0.1f) mChange = true;
	}
	else
	{
		mColor = Lerp(mColor, 1.0f, App->GetDt() * 1.4);
		mMesh->SetBaseColorFactor(float4(mColor, mColor, mColor, mColor));
		if (mColor > 0.9f)  mChange = false;
	}

}
