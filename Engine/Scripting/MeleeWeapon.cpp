#include "MeleeWeapon.h"
#include "TrailComponent.h"

MeleeWeapon::MeleeWeapon() : Weapon()
{
	mType = WeaponType::MELEE;
}

MeleeWeapon::~MeleeWeapon()
{
}

void MeleeWeapon::IncreaseComboStep()
{
    mNextComboStep = mComboStep + 1;
}


/*float MeleeWeapon::GetAttackTime() override
{
    switch (mComboStep) 
    {
    case 1:
        return mComboMilestone1;
    case 2:
        return mComboMilestone2;
    case 3:
        return mComboDuration;
    }
    return mAttackTime;
}*/



void MeleeWeapon::MeleeHit() {

    /*mMeleeBaseComboTimer = 0.0f;
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
    /*
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
    }*/
}
