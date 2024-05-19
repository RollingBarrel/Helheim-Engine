#include "ItemHealth.h"
#include "PlayerController.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Math/MathFunc.h"
#include "ScriptComponent.h"

CREATE(ItemHealth)
{
	CLASS(owner);

	END_CREATE;
}

ItemHealth::ItemHealth(GameObject* owner) : Script(owner) {}

void ItemHealth::Start()
{
    ModuleScene* scene = App->GetScene();
    mPlayer = scene->FindGameObjectWithTag(scene->GetTagByName("Player")->GetID());
}

void ItemHealth::Update()
{
    if (IsPlayerInRange(mActivationRange))
    {
        mGameObject->SetEnabled(false);

        PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
        if (playerScript != nullptr)
        {
            playerScript->SetShield(15.0f);
        }
    }
}

bool ItemHealth::IsPlayerInRange(float range)
{
    float distance = 0.0f;
    distance = (mPlayer) ? mGameObject->GetPosition().Distance(mPlayer->GetPosition()) : inf;

    return (distance <= range);
}