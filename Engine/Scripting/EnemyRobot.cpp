#include "pch.h"
#include "EnemyRobot.h"
#include "ModuleScene.h"
#include "Application.h"
#include "PlayerController.h"

CREATE(EnemyRobot)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::INT, mHealth);
    MEMBER(MemberType::FLOAT, mEnemySpeed);
    MEMBER(MemberType::FLOAT, mEnemyRotationSpeed);
    MEMBER(MemberType::FLOAT, mActivationDistance);

    MEMBER(MemberType::BOOL, mRangeActive);
    MEMBER(MemberType::FLOAT, mRangeDistance);
    MEMBER(MemberType::INT, mRangeDamage);

    MEMBER(MemberType::BOOL, mMeleeActive);
    MEMBER(MemberType::FLOAT, mMeleeDistance);
    MEMBER(MemberType::INT, mMeeleDamage);
   
    SEPARATOR("GAME OBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mAnimationComponentHolder);
    END_CREATE;
}

EnemyRobot::EnemyRobot(GameObject* owner) : Enemy(owner)
{
    mCurrentState = EnemyState::Deploy;
    mPreviousState = mCurrentState;
    mHealth = 15;
    mActivationDistance = 12.0f;

    mRangeActive = true;
    mRangeDistance = 9.0f;
    mRangeDamage = 15;

    mMeleeActive = true;
    mMeleeDistance = 2.0f;
    mMeeleDamage = 10;
}

void EnemyRobot::Start()
{
    Enemy::Start();
}

void EnemyRobot::Update()
{
    Enemy::Update();
}

//Change actual animation state of the enemy
void EnemyRobot::ChangeState(EnemyState newState) {
    mCurrentState = newState;
    StateMachine();
}

//Shows actual animation state of the enemy
void EnemyRobot::StateMachine() {
    switch (mCurrentState) {
        //******************************************************
        //FOR TEST UNTIL AI WILL BE AVAILABLE (will be changed the system to call the State Machine)
    case EnemyState::Deploy:
        LOG("Deploy animation");
        break;
    case EnemyState::Forward:
        LOG("Forward animation");
        break;
    case EnemyState::Backward:
        LOG("Backward animation");
        break;
    case EnemyState::Left:
        LOG("Left animation");
        break;
    case EnemyState::Right:
        LOG("Right animation");
        break;
        //******************************************************
    case EnemyState::RangeAttack:
        RangeAttack();
        break;
    case EnemyState::MeleeAttack:
        MeleeAttack();
		break;
    case EnemyState::Death:
        Death();
        break;
    }
}

void EnemyRobot::SearchPlayer() {
    Enemy::SearchPlayer();

    if (OpponentDistance(mRangeDistance)) {
        mInAttackDistance = true;

        if (OpponentDistance(mMeleeDistance) && mMeleeActive)
        {   
            ChangeState(EnemyState::MeleeAttack);
        }
        else
        {
            if (mRangeActive) {
                ChangeState(EnemyState::RangeAttack);
            }
        }
    }
}

void EnemyRobot::SetEnemyDamage(int damage) {
    Enemy::SetEnemyDamage(damage);

    if (mHealth = 0) {
        ChangeState(EnemyState::Death);
    }
}

//Melee attack function
void EnemyRobot::MeleeAttack() {
	//LOG("Starting MeleeAttack");

    if (mIsMoving == false) {
		//LOG("Melee attack animation by Robot");
	}
    else
    {
        switch (mPreviousState) {
		case EnemyState::Forward:
			LOG("Robot is attacking while running forward");
			break;
		case EnemyState::Backward:
			LOG("Robot is attacking while running backward");
			break;
		case EnemyState::Left:
			LOG("Robot is attacking while running Left");
			break;
		case EnemyState::Right:
			LOG("Robot is attacking while running right");
			break;
		}
	}

    ModuleScene* scene = App->GetScene();
    std::vector<GameObject*> Players;

    scene->FindGameObjectsWithTag(scene->GetRoot(), scene->GetTagByName("Player")->GetID(), Players);
    //player position
    float3 enemyPosition = mGameObject->GetPosition();
    // Recorrer el vector de enemigos y comprobar si hay colisión con el jugador
    for (auto player : Players)
    {
        // Componente de malla del enemigo
        MeshRendererComponent* enemyMesh = (MeshRendererComponent*)player->GetComponent(ComponentType::MESHRENDERER);

        // Posición del enemigo
        float3 playerPosition = player->GetPosition();
        // Distancia entre el jugador y el enemigo
        float distanceToEnemy = (playerPosition - enemyPosition).Length();
        float3 enemyFrontNormalized = mGameObject->GetFront().Normalized();
        float3 playerToEnemy = (enemyPosition - playerPosition).Normalized();

        // Si el enemigo está frente al jugador y dentro del rango de ataque
        float3 enemyFront = float3(mGameObject->GetFront().x, mGameObject->GetFront().y, mGameObject->GetFront().z);
        float dotProduct = playerToEnemy.Dot(enemyFrontNormalized);

        if (distanceToEnemy < 2.0f && dotProduct < 0)
        {
            PlayerController* playerScript = (PlayerController*)((ScriptComponent*)player->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (playerScript != nullptr)
            {
                playerScript->SetPlayerDamage(mMeeleDamage);
            }

        }
    }
}

void EnemyRobot::RangeAttack() {
    LOG("Starting RangeAttack");

    if (mIsMoving == false) {
        LOG("Range attack animation by Robot");
    }
    else
    {
        switch (mPreviousState) {
        case EnemyState::Forward:
            LOG("Robot is shooting while running forward");
            break;
        case EnemyState::Backward:
            LOG("Robot is shooting while running backward");
            break;
        case EnemyState::Left:
            LOG("Robot is shooting while running Left");
            break;
        case EnemyState::Right:
            LOG("Robot is shooting while running right");
            break;
        }
    }

    Shoot();
}

void EnemyRobot::Shoot( )
{
    ShootLogic(mRangeDamage);
    /*// TO DO: Disparar cada cierto tiempo
    if (Delay(mChargeTime)) 
    {
        mIsReadyToShoot = true;
    }
    else 
    {
        mIsReadyToShoot = false;
    }
    if (mIsReadyToShoot) 
    {
        ShootLogic(mRangeDamage);
    }*/

}

void EnemyRobot::ShootLogic(int damage)
{
    LOG("Robot is shooting");
    std::map<float, GameObject*> hits;

    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    hits = Physics::Raycast(&ray);

    Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));
 
        //recorrer todos los hits y hacer daño a los objetos que tengan tag = target
        for (auto hit : hits) {
            if (hit.second->GetTag()->GetName() == "Player") {

                PlayerController* playerScript = (PlayerController*)((ScriptComponent*)hit.second->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                if (playerScript != nullptr)
                {
                    playerScript->SetPlayerDamage(damage);
                }
            }
        }
}

void EnemyRobot::Death() {
    mInAttackDistance = false;

    //*******************************
    mGameObject->SetEnabled(false);
    //*******************************

    LOG("Robot Death animation");
}

//************************************************************************
//FOR TEST UNTIL AI WILL BE AVAILABLE
void EnemyRobot::Test_Forward() {
    Enemy::Test_Forward();
    ChangeState(EnemyState::Forward);
}

void EnemyRobot::Test_Backward() {
    Enemy::Test_Backward();
    ChangeState(EnemyState::Backward);
}

void EnemyRobot::Test_Left() {
    Enemy::Test_Left();
    ChangeState(EnemyState::Left);
}

void EnemyRobot::Test_Right() {
    Enemy::Test_Right();
    ChangeState(EnemyState::Right);
}
//************************************************************************