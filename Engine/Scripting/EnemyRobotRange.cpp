#include "EnemyRobotRange.h"
#include "GameObject.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "AIAGentComponent.h"
#include "BoxColliderComponent.h"
#include "GameManager.h"
#include "PoolManager.h"
#include "AudioManager.h"
#include "Bullet.h"

#include "ColorGradient.h"

CREATE(EnemyRobotRange) 
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mRangeDamage);
    MEMBER(MemberType::FLOAT, mBulletSpeed);
    MEMBER(MemberType::GAMEOBJECT, mBulletOrigin);
    MEMBER(MemberType::FLOAT, mAttackCoolDown);
    SEPARATOR("STATES");
    MEMBER(MemberType::FLOAT, mAttackDistance);

    END_CREATE;
}

void EnemyRobotRange::Start()
{
    Enemy::Start();
    mDisengageTime = 0.5f;

    // COLLIDER
    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyRobotRange::OnCollisionEnter, this, std::placeholders::_1)));
    }

}

void EnemyRobotRange::Attack()
{
    Enemy::Attack();

    float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
    direction.y = 0;
    direction.Normalize();
    float angle = std::atan2(direction.x, direction.z);

    if (mGameObject->GetWorldRotation().y != angle)
    {
        mGameObject->SetWorldRotation(float3(0, angle, 0));

    }
    if (mAttackCoolDownTimer.Delay(mAttackCoolDown)) 
    {
        mAnimationComponent->OnRestart();
        RangeAttack();
        if (IsPlayerInRange(mAttackDistance / 2.0f))
        {
            mCurrentState = EnemyState::FLEE;
        }
    }
}

void EnemyRobotRange::RangeAttack()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_GUNFIRE, mGameObject->GetWorldPosition());

    float3 bulletOriginPosition = mBulletOrigin->GetWorldPosition();
    GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
    bulletGO->SetWorldPosition(bulletOriginPosition);
    bulletGO->SetWorldRotation(mGameObject->GetWorldRotation());
    Bullet* bulletScript= static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
    bulletScript->Init(bulletOriginPosition, mGameObject->GetFront(),mBulletSpeed,1.0f, &gradient,mRangeDamage);
}

void EnemyRobotRange::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag() == "Door")
    {
        mEnemyCollisionDirection = collisionData->collisionNormal;
        //LOG("HOLA")
    }
}