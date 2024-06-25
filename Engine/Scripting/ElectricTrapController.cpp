#include "ElectricTrapController.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
ElectricTrapController::ElectricTrapController(GameObject* owner) : Script(owner)
{
}

void ElectricTrapController::Update()
{
}

void ElectricTrapController::Start()
{
}

void ElectricTrapController::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag().compare("Player") == 0)
    {

    }

    if (collisionData->collidedWith->GetTag().compare("Enemy") == 0)
    {

    }
}
