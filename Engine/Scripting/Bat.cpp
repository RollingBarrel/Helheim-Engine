#include "Bat.h"
#include "float3.h"
#include <vector>
#include "TrailComponent.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Application.h"

Bat::Bat() : MeleeWeapon()
{
    mAttackTime = 3.0f;
    mDamage = 7.0f;

}

Bat::~Bat()
{
}

void Bat::Enter()
{
    mTrail->Enable();
    mDamage = 2.0f;
}

void Bat::Update(float deltaTime)
{


}

void Bat::DealDamage(GameObject* enemy) {

    Enemy* enemyScript = dynamic_cast<Enemy*>(enemy->GetComponent(ComponentType::SCRIPT));
    if (enemyScript) {
        enemyScript->TakeDamage(mDamage);
    }
}


void Bat::Attack()
{
    // - Calculate hitbox and intersect it with every object to spawn sparks
    // - Damage to enemies inside the hitbox
    LOG("MELEE ATTACK!!")
    mComboTimer = 0.0f;

    bool firstHit = false;
    if (!mIsComboActive)
    {
        // First hit
        // mAnimationComponent->SendTrigger("tMelee", 0.2f);
        // DealDamage();
        mIsComboActive = true;
        mComboCurrentTime = 0.0f;
        mComboStep = 2;
        firstHit = true;

    }
    if (!mIsNextComboStep)
    {
        mIsNextComboStep = !firstHit;
        mBreakCombo = 0.0f;


    }
    mComboCurrentTime += App->GetDt();
    if (mComboCurrentTime > mComboMilestone1 && mComboCurrentTime < mComboMilestone2 && mIsNextComboStep && mComboStep == 2)
    {
        // Second hit
        // mAnimationComponent->SendTrigger("tMelee", 0.2f);
        // DealDamage();
        mComboStep = 3;
        mIsNextComboStep = false;
        mBreakCombo = 0.0f;


    }
    else if (mComboCurrentTime > mComboMilestone2 && mComboCurrentTime < mComboDuration && mComboStep == 3 && mIsNextComboStep)
    {
        // Dashing
        //mAnimationComponent->SendTrigger("tMelee", 0.2f);
        // DealDamage();
        float meleeSpeed = mMoveRange / mMoveDuration;
       // float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * meleeSpeed * App->GetDt());
       // mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        mComboTimer = 0.0f;

    }
    else if (mComboCurrentTime > mComboDuration && mComboStep == 3 && mIsNextComboStep)
    {
        //End combo
        //mAnimationComponent->SendTrigger("tIdle", 0.2f);
        mComboCurrentTime = false;
        mComboCurrentTime = 0.0f;
        mComboStep = 0; //this variable is weird, refactor how it works in next updates
        mIsNextComboStep = false;
        mBreakCombo = 0.0f;

    }
    else if (!mIsNextComboStep)
    {
        mBreakCombo += App->GetDt();
    }

    float breakTime = 10.0f;
    switch (mComboStep)
    {
    case 1:
        breakTime = 10.0f;
        break;
    case 2:
        breakTime = 1.1f;
        break;
    case 3:
        breakTime = 0.9f;
        break;
    default:
        break;
    }

    if (mBreakCombo > breakTime)
    {
        //mAnimationComponent->SendTrigger("tIdle", 0.2f);
        mIsComboActive = false;
        mComboCurrentTime = 0.0f;
        //mCurrentState = PlayerState::IDLE;
        mComboStep = 1; //this variable is weird, refactor how it works in next updates
        mIsNextComboStep = false;
        mBreakCombo = 0.0f;

    }

    if (mComboCurrentTime > mComboDuration + 2.0f)
    {
        mComboCurrentTime = 0.0f;
    }

}

void Bat::Exit()
{
    mTrail->Disable();
}

/*//Melee
    bool mLeftMouseButtonPressed = false;

    //Melee Base Attack
    float mMeleeBaseDamage = 2.0f;
    float mMeleeBaseRange = 1.0f;

    //Combo
    int mMeleeBaseComboStep = 1;
    float mMeleeBaseComboTimer = 0.0f;
    const float mMeleeBaseMaxComboInterval = 1.0f;
    float mMeleeComboDuration = 2.6f;
    float mMeleeComboMilestone1 = 1.1f;
    float mMeleeComboMilestone2 = 2.0f;
    float mMeleComboCurrentTime = 0.0f;
    float mBreakMeleeCombo = 0.0f;
    bool mNextComboStep = false;
    bool mIsMeleeBaseComboActive = false;


    //Final Attack
    const float mMeleeBaseFinalAttackDuration = 0.5f;
    float mMeleeBaseFinalAttackTimer = 0.0f;
    float mMeleeBaseMoveDuration = 0.5f;
    float mMeleeBaseMoveRange = 8.0f;

    //Melee Special Attack
    float mMeleeSpecialTimer = 0.0f;
    const float mMeleeSpecialAttackDuration = 2.0f;
    float mMeleeSpecialDamage = 4.0f;
    float mMeleeSpecialRange = 2.0f;

    //Cooldown
    bool mIsMeleeSpecialCoolDownActive = false;
    float mMeleeSpecialCoolDownTimer = 0.0f;
    float mMeleeSpecialCoolDown = 4.0f;*/

/*if (mIsMeleeBaseComboActive)
    {
        mMeleeBaseComboTimer += App->GetDt();
        if (mMeleeBaseComboTimer >= mMeleeBaseMaxComboInterval)
        {
            mMeleeBaseComboStep = 1;
            mIsMeleeBaseComboActive = false;
            mMeleeBaseComboTimer = 0.0f;
            mCurrentState = PlayerState::IDLE;
        }
    }

    if (mIsMeleeSpecialCoolDownActive)
    {
        mMeleeSpecialCoolDownTimer += App->GetDt();
        if (mMeleeSpecialCoolDownTimer >= mMeleeSpecialCoolDown)
        {
            mMeleeSpecialCoolDownTimer = 0.0f;
            mIsMeleeSpecialCoolDownActive = false;
            mCurrentState = PlayerState::IDLE;
        }
    }*/

/*void PlayerController::MeleeBaseCombo()
{
    mMeleeBaseComboTimer = 0.0f;
    bool firstHit = false;
    if (!mIsMeleeBaseComboActive)
    {
        // First hit
        mAnimationComponent->SendTrigger("tMelee", 0.2f);
        mIsMeleeBaseComboActive = true;
        mMeleComboCurrentTime = 0.0f;
        MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mMeleeBaseComboStep = 2;
        firstHit = true;

    }
    if (mLeftMouseButtonPressed && !mNextComboStep)
    {
        mNextComboStep = !firstHit;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;


    }
    mMeleComboCurrentTime += App->GetDt();
    if (mMeleComboCurrentTime > mMeleeComboMilestone1 && mMeleComboCurrentTime < mMeleeComboMilestone2 && mNextComboStep && mMeleeBaseComboStep == 2)
    {
        // Second hit
        MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mMeleeBaseComboStep = 3;
        mNextComboStep = false;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;


    }
    else if (mMeleComboCurrentTime > mMeleeComboMilestone2 && mMeleComboCurrentTime < mMeleeComboDuration && mMeleeBaseComboStep == 3 && mNextComboStep)
    {
        // Dashing
        float meleeSpeed = mMeleeBaseMoveRange / mMeleeBaseMoveDuration;
        float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * meleeSpeed * App->GetDt());
        mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        mBreakMeleeCombo = 0.0f;

    }
    else if (mMeleComboCurrentTime > mMeleeComboDuration && mMeleeBaseComboStep == 3 && mNextComboStep)
    {
        //End combo
        MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mAnimationComponent->SendTrigger("tIdle", 0.2f);
        mIsMeleeBaseComboActive = false;
        mMeleComboCurrentTime = 0.0f;
        //mCurrentState = PlayerState::IDLE;
        mMeleeBaseComboStep = 4; //this variable is weird, refactor how it works in next updates
        mNextComboStep = false;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;

    }
    else if (!mNextComboStep)
    {
        mBreakMeleeCombo += App->GetDt();
    }

    float breakTime = 10.0f;
    switch (mMeleeBaseComboStep)
    {
    case 1:
        breakTime = 10.0f;
        break;
    case 2:
        breakTime = 1.1f;
        break;
    case 3:
        breakTime = 0.9f;
        break;
    default:
        break;
    }

    if (mBreakMeleeCombo > breakTime)
    {
        mAnimationComponent->SendTrigger("tIdle", 0.2f);
        mIsMeleeBaseComboActive = false;
        mMeleComboCurrentTime = 0.0f;
        //mCurrentState = PlayerState::IDLE;
        mMeleeBaseComboStep = 1; //this variable is weird, refactor how it works in next updates
        mNextComboStep = false;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;

    }

    if (mMeleComboCurrentTime > mMeleeComboDuration + 2.0f)
    {
        mMeleComboCurrentTime = 0.0f;
    }
}

void PlayerController::MeleeSpecialCombo() {

    mIsMeleeSpecialCoolDownActive = true;
    mMeleeSpecialTimer += App->GetDt();
    if (mMeleeSpecialTimer >= mMeleeSpecialAttackDuration)
    {
        mIsMeleeSpecialCoolDownActive = false;
        mMeleeSpecialTimer = 0.0f;
        //mCurrentState = PlayerState::IDLE;
    }
    else
    {
        //TODO: Implement special attack animation
        mGameObject->SetPosition(mGameObject->GetPosition());
        mGameObject->SetRotation(mGameObject->GetRotation());
        MeleeHit(mMeleeSpecialRange, mMeleeSpecialDamage);
        //mCurrentState = PlayerState::ATTACK;
    }

}

void PlayerController::MeleeHit(float AttackRange, float AttackDamage) {

    ModuleScene* scene = App->GetScene();
    std::vector<GameObject*> Enemies;

    scene->FindGameObjectsWithTag(scene->GetTagByName("Enemy")->GetID(), Enemies);
    float3 playerPosition = mGameObject->GetPosition();

    // Recorrer el vector de enemigos y comprobar si hay colisión con el jugador
    for (auto enemy : Enemies)
    {
        MeshRendererComponent* enemyMesh = (MeshRendererComponent*)enemy->GetComponent(ComponentType::MESHRENDERER);
        float3 enemyPosition = enemy->GetPosition();
        float distanceToEnemy = (enemyPosition - playerPosition).Length();
        float3 enemyToPlayer = (playerPosition - enemyPosition).Normalized();

        // Si el enemigo está frente al jugador y dentro del rango de ataque
        float3 playerFrontNormalized = mGameObject->GetFront().Normalized();
        float dotProduct = enemyToPlayer.Dot(playerFrontNormalized);

        if (distanceToEnemy < AttackRange && dotProduct < 0)
        {
            Enemy* enemyScript = (Enemy*)((ScriptComponent*)enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (enemyScript) {
                enemyScript->TakeDamage(AttackDamage);
                enemyScript->PushBack();
            }
        }
    }
}
*/