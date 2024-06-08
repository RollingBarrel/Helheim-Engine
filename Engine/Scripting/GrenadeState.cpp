#include "GrenadeState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

GrenadeState::GrenadeState(PlayerController* player) : State(player)
{
}

GrenadeState::~GrenadeState()
{
}

StateType GrenadeState::HandleInput()
{
    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        return StateType::AIM;
    }

	return StateType::GRENADE;
}

void GrenadeState::Update()
{
}

void GrenadeState::Enter()
{
}

void GrenadeState::Exit()
{
}

StateType GrenadeState::GetType()
{
    return StateType::GRENADE;
}

/*
    if (mAimingGrenade && !mThrowAwayGrenade)
    {
        if (mGrenadeAimAreaGO && mGrenadeExplotionPreviewAreaGO)
        {
            GrenadeAttack();
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_REPEAT && !mThrowAwayGrenade)
    {
        mCurrentState = PlayerState::ATTACK;
        mAimingGrenade = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        if (mGrenadeAimAreaGO)
        {
            mGrenadeAimAreaGO->SetEnabled(false);
        }
    }*/

    /*void PlayerController::UpdateGrenadeCooldown()
    {
        if (mThrowAwayGrenade)
        {
            if (mGrenadeCoolDownTimer <= 0.0f)
            {
                mGrenadeCoolDownTimer = mGrenadeCoolDown;
                mThrowAwayGrenade = false;
                GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(0.0f);
                return;
            }

            if (mGrenadeCoolDownTimer > 0.0f)
            {
                mGrenadeCoolDownTimer -= App->GetDt();
                LOG("Grenade cooldown, wait %f seconds", mGrenadeCoolDownTimer);
            }
            else
            {
                mGrenadeCoolDownTimer = 0.0f;
            }

            GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(mGrenadeCoolDownTimer / mGrenadeCoolDown);
        }
    }

    void PlayerController::GrenadeAttack()
    {
        AimGrenade();

        if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
        {
            //mCurrentState = PlayerState::IDLE;
            mAimingGrenade = false;
            mGrenadeAimAreaGO->SetEnabled(false);
            mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
        }
    }

    void PlayerController::AimGrenade()
    {
        // Initialize circle
        mGrenadeAimAreaGO->SetEnabled(true);
        mGrenadeAimAreaGO->SetScale(float3(mGrenadeRange, 0.5, mGrenadeRange));
        mGrenadeAimAreaGO->SetPosition(mGameObject->GetPosition());

        GrenadeTarget();
    }

    void PlayerController::GrenadeTarget()
    {
        Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetLocalMousePosition());
        Plane plane = Plane(mGrenadeAimAreaGO->GetWorldPosition(), float3::unitY);

        float distance;
        bool intersects = plane.Intersects(ray, &distance);
        float3 hitPoint = ray.GetPoint(distance);

        // Check if mouse hit inside circle
        // TODO: Check hit with physic
        float3 diff = hitPoint - mGameObject->GetWorldPosition();
        float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
        float radiusSquared = mGrenadeRange * mGrenadeRange;

        bool hit = distanceSquared <= radiusSquared;

        if (intersects)
        {
            float3 finalPosition;
            if (hit)
            {
                finalPosition = hitPoint;
            }
            else
            {
                // Project hitPoint to the edge of the circle
                float distanceToEdge = mGrenadeRange / sqrtf(distanceSquared);
                finalPosition = mGameObject->GetWorldPosition() + diff * distanceToEdge;
            }
            mGrenadeExplotionPreviewAreaGO->GetChildren()[0]->SetEnabled(true);
            mGrenadeExplotionPreviewAreaGO->GetChildren()[1]->SetEnabled(false);

            mGrenadeExplotionPreviewAreaGO->SetEnabled(true);
            mGrenadeExplotionPreviewAreaGO->SetScale(float3(mGrenade->GetGrenadeRadius(), 0.5f, mGrenade->GetGrenadeRadius()));
            mGrenadeExplotionPreviewAreaGO->SetPosition(float3(finalPosition.x, 0.3f, finalPosition.z));

            if ((App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)) {
                ThrowGrenade(finalPosition);
            }
        }
    }

    void PlayerController::ThrowGrenade(float3 target)
    {
        //mCurrentState = PlayerState::IDLE;
        mAimingGrenade = false;
        mGrenadeAimAreaGO->SetEnabled(false);

        mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
        mGrenadeExplotionPreviewAreaGO->SetEnabled(true);

        mGrenadeExplotionPreviewAreaGO->GetChildren()[0]->SetEnabled(false);
        mGrenadeExplotionPreviewAreaGO->GetChildren()[1]->SetEnabled(true);



        mThrowAwayGrenade = true;

        ScriptComponent* script = (ScriptComponent*)mGrenadeExplotionPreviewAreaGO->GetComponent(ComponentType::SCRIPT);
        Grenade* grenade = (Grenade*)script->GetScriptInstance();

        // TODO wait for thow animation time
        grenade->SetDestionation(target);
    }*/