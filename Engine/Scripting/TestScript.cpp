#include "TestScript.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "Keys.h"

CREATE(TestScript)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mPlayerSpeed);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT3, mTestFloat3);
    SEPARATOR("GAME OBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mRobot);
    END_CREATE;
}

void TestScript::Start()
{
	LOG("El nombre de mi gameobject es: %s ", mGameObject->GetName().c_str());
	CameraComponent* component1 = (CameraComponent*)mGameObject->CreateComponent(ComponentType::CAMERA);
	PointLightComponent* component3 = (PointLightComponent*)mGameObject->CreateComponent(ComponentType::POINTLIGHT);
	SpotLightComponent* component4 = (SpotLightComponent*)mGameObject->CreateComponent(ComponentType::SPOTLIGHT);

    GameObject* juan = App->GetScene()->Find("Juan");

    // Create an instance from a prefab
    // We need the Prefab's filename
    Resource* resource = App->GetResource()->RequestResource("Assets/Prefabs/Zombunny.prfb"); //Bullet Prefab
    // Resource* resource = App->GetResource()->RequestResource("Assets/Prefabs/Bullet 1.prfb"); //using the name

    App->GetScene()->LoadPrefab("Assets/Prefabs/Zombunny.prfb", resource->GetUID(), juan);
}

void TestScript::Update()
{
	if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, 1) * App->GetDt() * mPlayerSpeed);
	}
	if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, -1) * App->GetDt() * mPlayerSpeed);
	}
	if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(-1, 0, 0) * App->GetDt() * mPlayerSpeed);
	}
	if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(1, 0, 0) * App->GetDt() * mPlayerSpeed);
	}
	

    if (mRobot != nullptr) {
        if (mMovement >= mHeight)
        {
            mStartCounter = true;
            mMovement = 0;
            mUp = mUp ? false : true;
        }



        if (mStartCounter)
        {
            mTimePassed += App->GetDt();

            if (mTimePassed >= mCoolDown)
            {
                mTimePassed = 0;
                mStartCounter = false;
            }
        }
        else
        {
            mMovement += mSpeed * App->GetDt();
            if (mUp)
            {
                mRobot->SetPosition(float3(mRobot->GetPosition().x, mRobot->GetPosition().y + mSpeed * App->GetDt(), mRobot->GetPosition().z));
            }
            else
            {
                mRobot->SetPosition(float3(mRobot->GetPosition().x, mRobot->GetPosition().y - mSpeed * App->GetDt(), mRobot->GetPosition().z));
            }
        }

    }

    if (mGameObject->GetPosition().x > 10.0f) {

        LOG("YOU WIN");
    }

    LOG("hola %s" , typeid(mTestFloat3).name());
}

void TestScript::OnButtonClick()
{
    LOG("Script Button Onclick");
}


