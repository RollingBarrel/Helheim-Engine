#include "ElectricTrapGroupController.h"
#include "ElectricTrapController.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ScriptComponent.h"

#include <vector>

CREATE(ElectricTrapGroupController)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mTraps);
    END_CREATE;
}

ElectricTrapGroupController::ElectricTrapGroupController(GameObject* owner) : Script(owner)
{
}

ElectricTrapGroupController::~ElectricTrapGroupController()
{
}

void ElectricTrapGroupController::Start()
{
    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ElectricTrapGroupController::OnCollisionEnter, this, std::placeholders::_1)));
    }

    std::vector<GameObject*> traps = mTraps->GetChildren();
    for (int i = 0; i < traps.size(); ++i) 
    {
        const ScriptComponent* script = static_cast<ScriptComponent*>(traps[i]->GetComponent(ComponentType::SCRIPT));
        ElectricTrapController* electricTrap = static_cast<ElectricTrapController*>(script->GetScriptInstance());

        if (i % 3 == 0)
        {
            electricTrap->SetTrapSound(SFX::ELECTRICAL_TRAP1);
        }
        else if (i % 3 == 1)
        {
            electricTrap->SetTrapSound(SFX::ELECTRICAL_TRAP2);
        }
        else if (i % 3 == 2)
        {
            electricTrap->SetTrapSound(SFX::ELECTRICAL_TRAP3);
        }
    }
}

void ElectricTrapGroupController::Update()
{
}

void ElectricTrapGroupController::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collision = collisionData->collidedWith;
    if (collision->GetTag().compare("Player") == 0)
    {
        std::vector<GameObject*> traps = mTraps->GetChildren();

        for (GameObject* trap : traps)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(trap->GetComponent(ComponentType::SCRIPT));
            ElectricTrapController* electricTrap = static_cast<ElectricTrapController*>(script->GetScriptInstance());
            electricTrap->SetAwake(true);
        }
    }
}
