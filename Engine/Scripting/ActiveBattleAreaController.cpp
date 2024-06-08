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



void ActiveBattleAreaController::Start()
{
    if(mBattleAreaGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mBattleAreaGO->GetComponent(ComponentType::SCRIPT));
        mBattleArea = static_cast<BattleArea*>(script->GetScriptInstance());
    }
}

void ActiveBattleAreaController::OnCollisionEnter(CollisionData* collisionData)
{
    collisionData->collidedWith->GetTag()->GetID()
}


void ActiveBattleAreaController::SetActiveBattleArea() const
{
    GameManager::GetInstance()->SetActiveBattleArea(mBattleArea);
}