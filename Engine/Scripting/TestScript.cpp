#include "pch.h" 
#include <utility>
#include <limits.h>
#include "TestScript.h"
#include <string>
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "NavMeshControllerComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"

#include "Keys.h"
#include "Geometry/AABB.h"


TestScript::TestScript(GameObject* owner) : Script(owner)
{

}

void TestScript::Start()
{
	LOG("El nombre de mi gameobject es: %s ", mGameObject->GetName().c_str());
	CameraComponent* component1 = (CameraComponent*)mGameObject->CreateComponent(ComponentType::CAMERA);
	NavMeshControllerComponent* component2 = (NavMeshControllerComponent*)mGameObject->CreateComponent(ComponentType::NAVMESHCONTROLLER);
	PointLightComponent* component3 = (PointLightComponent*)mGameObject->CreateComponent(ComponentType::POINTLIGHT);
	SpotLightComponent* component4 = (SpotLightComponent*)mGameObject->CreateComponent(ComponentType::SPOTLIGHT);

    gameObject = App->GetScene()->Find("Robot.gltf");
}

void TestScript::Update()
{
	if (App->GetInput()->GetKey(Keys::Keys_T) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(0, 0, -1) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(-1, 0, 0) * App->GetGameDt());
	}
	if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT) {
		mGameObject->SetPosition(mGameObject->GetPosition() + float3(1, 0, 0) * App->GetGameDt());
	}
	

    if (gameObject != nullptr) {
        if (movement >= height)
        {
            startCounter = true;
            movement = 0;
            up = up ? false : true;
        }



        if (startCounter)
        {
            timePassed += App->GetGameDt();

            if (timePassed >= coolDown)
            {
                timePassed = 0;
                startCounter = false;
            }
        }
        else
        {
            movement += speed * App->GetGameDt();
            if (up)
            {
                //transform.position = new Vector3(transform.position.x, transform.position.y + speed * App->GetGameDt(), transform.position.z);
                gameObject->SetPosition(float3(gameObject->GetPosition().x, gameObject->GetPosition().y + speed * App->GetGameDt(), gameObject->GetPosition().z));
            }
            else
            {
                //transform.position = new Vector3(transform.position.x, transform.position.y - speed * App->GetGameDt(), transform.position.z);
                gameObject->SetPosition(float3(gameObject->GetPosition().x, gameObject->GetPosition().y - speed * App->GetGameDt(), gameObject->GetPosition().z));
            }
        }

    }

  
    if (mGameObject->GetPosition().x > 10.0f) {

        LOG("YOU WIN");
    }
	
    
}

