#include "pch.h"
#include "Dash.h"
#include "Application.h"
#include "GameObject.h"

void Dash::Start()
{

}

void Dash::Update()
{

   if (movement >= lenght)
   {
       startCounter = true;
       movement = 0;
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
       mGameObject->SetPosition(float3(mGameObject->GetPosition().x, mGameObject->GetPosition().y + speed * App->GetGameDt(), mGameObject->GetPosition().z));
       
   }

 
}
