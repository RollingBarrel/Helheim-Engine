#include "Level1Manager.h"
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

CREATE(Level1Manager)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLevel1MainThemeHolder);
    MEMBER(MemberType::GAMEOBJECT, mPlayerControllerHolder);
    MEMBER(MemberType::GAMEOBJECT, mEnemyFootStepHolder);
    MEMBER(MemberType::GAMEOBJECT, mStangeBackgroudSoundHolder);
    END_CREATE;
}

Level1Manager::~Level1Manager()
{
}

Level1Manager::Level1Manager(GameObject* owner) : Script(owner)
{
}

void Level1Manager::Start()
{

    if (mLevel1MainThemeHolder != nullptr)
    {
        mLevel1MainTheme = (AudioSourceComponent*)mLevel1MainThemeHolder->GetComponent(ComponentType::AUDIOSOURCE);
        mLevel1MainTheme->PlayWithVolume(0.4f);
    }
    if (mPlayerControllerHolder != nullptr)
    {
        ScriptComponent* PlayerControllerScript = (ScriptComponent*)mPlayerControllerHolder->GetComponent(ComponentType::SCRIPT);
        mPlayerController = (PlayerController*)PlayerControllerScript->GetScriptInstance();
    }
    if (mEnemyFootStepHolder != nullptr)
    {
        mEnemyFootStep = (AudioSourceComponent*)mEnemyFootStepHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }
    if (mStangeBackgroudSoundHolder != nullptr)
    {
        mStrangeBackgroundSound = (AudioSourceComponent*)mStangeBackgroudSoundHolder->GetComponent(ComponentType::AUDIOSOURCE);
        mStrangeBackgroundSound->SetPositionWithGameObject(false);
        mStrangeBackgroundSound->PlayWithVolume(0.3f);
    }
}

void Level1Manager::Update() {
    UpdateBackgroundMusic();
    UpdateEnemyFootStepMusic();
    UpdateBackgroundStrangeMusic();
}

void Level1Manager::UpdateBackgroundMusic()
{
    BattleSituation currentSituation = mPlayerController->GetBattleSituation();

    if (currentSituation == BattleSituation::IDLE_HIGHT_HP) 
    {
        mLevel1MainTheme->SmoothUpdateParameterValueByName("Area", 0, 5);
    }

    if (currentSituation == BattleSituation::BATTLE_LOW_HP) 
    {
        mLevel1MainTheme->SmoothUpdateParameterValueByName("Area", 30, 5);
    }

    if (currentSituation == BattleSituation::BATTLE_HIGHT_HP) 
    {
        mLevel1MainTheme->SmoothUpdateParameterValueByName("Area", 60, 5);
    }

    if (currentSituation == BattleSituation::IDLE_LOW_HP) 
    {
        mLevel1MainTheme->SmoothUpdateParameterValueByName("Area", 80, 5);
    }

    if (currentSituation == BattleSituation::DEATH) 
    {
        mLevel1MainTheme->Stop(true);
        mLevel1MainTheme->Play();
    }
}

void Level1Manager::UpdateEnemyFootStepMusic()
{
    ModuleScene* scene = App->GetScene();

    std::vector<GameObject*> Enemies;
    scene->FindGameObjectsWithTag(scene->GetTagByName("Enemy")->GetID(), Enemies);

    if (!mReadyToStep) {
        mStepTimePassed += App->GetDt();
        if (mStepTimePassed >= mStepCoolDown) {
            mStepTimePassed = 0;
            mReadyToStep = true;
        }
    }
    else {
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

void Level1Manager::UpdateBackgroundStrangeMusic() {
    ModuleScene* scene = App->GetScene();

    mStrangeBackgroundSound->UpdatePosition(GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
}