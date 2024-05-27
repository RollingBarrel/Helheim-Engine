#include "Level1AManager.h"
#include "AudioSourceComponent.h"
#include "Application.h"
#include "GameObject.h"
#include "Keys.h"
#include "ModuleInput.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "Enemy.h"
#include "GameManager.h"
#include "ModuleScene.h"

CREATE(Level1AManager)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLevel1AMainThemeHolder);
    MEMBER(MemberType::GAMEOBJECT, mPlayerControllerHolder);
    MEMBER(MemberType::GAMEOBJECT, mEnemyFootStepHolder);
    END_CREATE;
}

Level1AManager::Level1AManager(GameObject* owner) : Script(owner)
{
}

Level1AManager::~Level1AManager()
{
}

void Level1AManager::Start()
{

    if (mLevel1AMainThemeHolder != nullptr) 
    {
        mLevel1AMainTheme = (AudioSourceComponent*)mLevel1AMainThemeHolder->GetComponent(ComponentType::AUDIOSOURCE);
        mLevel1AMainTheme->Play();
    }

    if (mPlayerControllerHolder != nullptr) 
    {
        ScriptComponent* PlayerControllerScript = (ScriptComponent*)mPlayerControllerHolder->GetComponent(ComponentType::SCRIPT);
        mPlayerController = (PlayerController*) PlayerControllerScript->GetScriptInstance();
    }


}

void Level1AManager::Update() {
    //UpdateBackgroundMusic();
    ModuleScene* scene = App->GetScene();

    if (mEnemyFootStepHolder != nullptr) {
        mEnemyFootStep = (AudioSourceComponent*)mEnemyFootStepHolder->GetComponent(ComponentType::AUDIOSOURCE);

        std::vector<GameObject*> Enemies;
        scene->FindGameObjectsWithTag(scene->GetTagByName("Enemy")->GetID(), Enemies);

        if (!mReadyToStep) {
            mStepTimePassed += App->GetDt();
            if (mStepTimePassed >= mStepCoolDown) {
                mStepTimePassed = 0;
                mReadyToStep = true;
            }
        } else {
            for (auto* e : Enemies) {
                float dist = e->GetWorldPosition().Distance(GameManager::GetInstance()->GetPlayer()->GetWorldPosition());

                ScriptComponent* enemyscript = (ScriptComponent*)e->GetComponent(ComponentType::SCRIPT);
                Enemy* enemy = (Enemy*)enemyscript->GetScriptInstance();

                if (dist < 10 && enemy->IsMoving()) {
                    mEnemyFootStep->PlayOneShotPosition(e->GetPosition());
                }
            }
            mReadyToStep = false;
        }
    }
}

void Level1AManager::UpdateBackgroundMusic()
{
    BattleSituation currentSituation = mPlayerController->GetBattleSituation();

    if (currentSituation == BattleSituation::IDLE_HIGHT_HP) 
    {
        mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 0, 5);
    }

    if (currentSituation == BattleSituation::BATTLE_LOW_HP) 
    {
        mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 30, 5);
    }

    if (currentSituation == BattleSituation::BATTLE_HIGHT_HP) 
    {
        mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 60, 5);
    }

    if (currentSituation == BattleSituation::IDLE_LOW_HP) 
    {
        mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 80, 5);
    }

    if (currentSituation == BattleSituation::DEATH) 
    {
        mLevel1AMainTheme->Stop(true);
    }
}
