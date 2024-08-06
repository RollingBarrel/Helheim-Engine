#include "LoreCollectible.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameObject.h"


#include "ScriptComponent.h"
#include "BoxColliderComponent.h"

CREATE(LoreCollectible)
{
	CLASS(owner);
	END_CREATE;
}

LoreCollectible::LoreCollectible(GameObject* owner) : Script(owner) {}

void LoreCollectible::Init()
{
}

void LoreCollectible::Start()
{

	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&LoreCollectible::OnCollisionEnter, this, std::placeholders::_1)));
		mCollider->SetColliderType(ColliderType::STATIC);
	}

}

void LoreCollectible::Update()
{



}

void LoreCollectible::OnCollisionEnter(CollisionData* collisionData)
{

	GameObject* collisionGO = collisionData->collidedWith;

	//Make prompt appear 

	if (collisionGO->GetTag() == "Player" && App->GetInput()->GetKey(Keys::Keys_F) == KeyState::KEY_DOWN ||
		App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN) {

		//show text 

		LOG("Item interact");
	}


}
