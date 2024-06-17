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
    MEMBER(MemberType::GAMEOBJECT, mStangeBackgroudSoundHolder);
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

    //if (mLevel1AMainThemeHolder != nullptr)
    //{
    //    mLevel1AMainTheme = (AudioSourceComponent*)mLevel1AMainThemeHolder->GetComponent(ComponentType::AUDIOSOURCE);
    //    mLevel1AMainTheme->PlayWithVolume(0.4f);
    //}

    //if (mPlayerControllerHolder != nullptr)
    //{
    //    ScriptComponent* PlayerControllerScript = (ScriptComponent*)mPlayerControllerHolder->GetComponent(ComponentType::SCRIPT);
    //    mPlayerController = (PlayerController*)PlayerControllerScript->GetScriptInstance();
    //}

    //if (mEnemyFootStepHolder != nullptr)
    //{
    //    mEnemyFootStep = (AudioSourceComponent*)mEnemyFootStepHolder->GetComponent(ComponentType::AUDIOSOURCE);
    //}

    //if (mStangeBackgroudSoundHolder != nullptr)
    //{
    //    mStrangeBackgroundSound = (AudioSourceComponent*)mStangeBackgroudSoundHolder->GetComponent(ComponentType::AUDIOSOURCE);
    //    mStrangeBackgroundSound->SetPositionWithGameObject(false);
    //    mStrangeBackgroundSound->PlayWithVolume(0.3f);
    //}
}

void Level1AManager::Update() {
    UpdateBackgroundMusic();
    UpdateEnemyFootStepMusic();
    UpdateBackgroundStrangeMusic();
}

void Level1AManager::UpdateBackgroundMusic()
{
    //BattleSituation currentSituation = mPlayerController->GetBattleSituation();

    //if (currentSituation == BattleSituation::IDLE_HIGHT_HP) 
    //{
    //    mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 0, 5);
    //}

    //if (currentSituation == BattleSituation::BATTLE_LOW_HP) 
    //{
    //    mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 30, 5);
    //}

    //if (currentSituation == BattleSituation::BATTLE_HIGHT_HP) 
    //{
    //    mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 60, 5);
    //}

    //if (currentSituation == BattleSituation::IDLE_LOW_HP) 
    //{
    //    mLevel1AMainTheme->SmoothUpdateParameterValueByName("Area", 80, 5);
    //}

    //if (currentSituation == BattleSituation::DEATH) 
    //{
    //    mLevel1AMainTheme->Stop(true);
    //    mLevel1AMainTheme->Play();
    //}
}

void Level1AManager::UpdateEnemyFootStepMusic()
{
    //ModuleScene* scene = App->GetScene();

    //const std::vector<GameObject*>& Enemies = scene->FindGameObjectsWithTag("Enemy");
    //

    //if (!mReadyToStep) {
    //    mStepTimePassed += App->GetDt();
    //    if (mStepTimePassed >= mStepCoolDown) {
    //        mStepTimePassed = 0;
    //        mReadyToStep = true;
    //    }
    //}
    //else {
    //    for (auto* e : Enemies) {
    //        float dist = e->GetPosition().Distance(GameManager::GetInstance()->GetPlayer()->GetPosition());

    //        ScriptComponent* enemyscript = (ScriptComponent*)e->GetComponent(ComponentType::SCRIPT);
    //        Enemy* enemy = (Enemy*)enemyscript->GetScriptInstance();

    //        if (dist < 10 && enemy->IsMoving()) {
    //            mEnemyFootStep->PlayOneShotPosition(e->GetPosition());
    //        }
    //    }
    //    mReadyToStep = false;
    //}
}

void Level1AManager::UpdateBackgroundStrangeMusic() {
    //ModuleScene* scene = App->GetScene();

    //mStrangeBackgroundSound->UpdatePosition(GameManager::GetInstance()->GetPlayer()->GetPosition());
}
