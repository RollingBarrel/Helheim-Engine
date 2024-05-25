#include "Grenade.h"
#include "GameObject.h"
#include "Application.h"

CREATE(Grenade)
{
	CLASS(owner);
	END_CREATE;
}

Grenade::Grenade(GameObject* owner) : Script(owner)
{
}

Grenade::Grenade(GameObject* owner, float dps, float duration, float area) : Script(owner)
{
	// Just if player upgraded
	mGrenadeDPS = dps;
	mGrenadeDuration = duration;
	mGrenadeArea = area;
}

Grenade::~Grenade()
{
}

void Grenade::Start()
{
	
} 

void Grenade::Update()
{
	Explotion();
}

void Grenade::Explotion()
{
    if (mExplotionStart)
    {
        if (mGrenadeCurrentTime > 0)
        {
            mGrenadeCurrentTime -= App->GetDt();
            if (mGrenadeCurrentTime <= 0)
            {
                EndExplotion();  
            }
        }
    }
}

void Grenade::EndExplotion()
{
    mGameObject->SetEnabled(false);
    mGrenadeCurrentTime = mGrenadeDuration;
    mExplotionStart = false;
}

void Grenade::SetDestionation(float3 destination)
{
	mDestination = destination;

	mExplotionStart = true;
}
