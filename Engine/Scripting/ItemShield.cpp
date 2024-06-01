#include "ItemShield.h"
#include "PlayerController.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Math/MathFunc.h"
#include "ScriptComponent.h"

CREATE(ItemShield)
{
	CLASS(owner);

    MEMBER(MemberType::FLOAT, mHealthRecovered);
    MEMBER(MemberType::FLOAT, mActivationRange);

	END_CREATE;
}

ItemShield::ItemShield(GameObject* owner) : Script(owner) {}

void ItemShield::Start()
{
    ModuleScene* scene = App->GetScene();
    //mPlayer = scene->FindGameObjectWithTag(scene->GetTagByName("Player")->GetID());
}

void ItemShield::Update()
{
    if (IsPlayerInRange(mActivationRange))
    {
        mGameObject->SetEnabled(false);

        PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
        if (playerScript != nullptr)
        {
            playerScript->RechargeShield(mHealthRecovered);
        }
    }
}

bool ItemShield::IsPlayerInRange(float range)
{
    float distance = 0.0f;
    distance = (mPlayer) ? mGameObject->GetPosition().Distance(mPlayer->GetPosition()) : inf;

    return (distance <= range);
}