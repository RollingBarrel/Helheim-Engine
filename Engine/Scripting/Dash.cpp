#include "pch.h"
#include "Dash.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "Keys.h"


Dash::Dash(GameObject* owner) : Script(owner)
{
}
void Dash::Start()
{

}

void Dash::Update()
{


    if (App->GetInput()->GetKey(Keys::Keys_T) == KeyState::KEY_DOWN && !startCounter)
    {
        isDashActive = true;
    }

    if (isDashActive) {

        if (movement >= lenght)
        {
            startCounter = true;
            movement = 0;
            isDashActive = false;
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
            mGameObject->SetPosition(mGameObject->GetPosition() + mGameObject->GetFront() * speed * App->GetGameDt());

        }
    }
       

    
}
