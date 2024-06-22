#include "ActiveBattleAreaController.h"
#include "BattleArea.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "Physics.h"
#include "BoxColliderComponent.h"
CREATE(ActiveBattleAreaController)
{
    CLASS(owner);
    SEPARATOR("Battle Area");
    MEMBER(MemberType::GAMEOBJECT, mBattleAreaGO);
    END_CREATE;
}
ActiveBattleAreaController::ActiveBattleAreaController(GameObject* owner): Script(owner)
{
}

ActiveBattleAreaController::~ActiveBattleAreaController()
{
}



void ActiveBattleAreaController::Start()
{
    if(mBattleAreaGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mBattleAreaGO->GetComponent(ComponentType::SCRIPT));
        mBattleArea = static_cast<BattleArea*>(script->GetScriptInstance());
    }
    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ActiveBattleAreaController::OnCollisionEnter, this, std::placeholders::_1)));
    }
}

void ActiveBattleAreaController::Update()
{
}

void ActiveBattleAreaController::OnCollisionEnter(CollisionData* collisionData)
{
    
    if (mBattleArea && collisionData->collidedWith->GetTag().compare("Player") == 0)
    {
        GameManager::GetInstance()->SetActiveBattleArea(mBattleArea);
        mBattleArea->ActivateArea(true); //maybe has to be false idk
        LOG("PLAYER COLLISION");
        mBattleArea = nullptr;
    }
}


