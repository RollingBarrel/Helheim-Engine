#include "Grenade.h"
#include "GameObject.h"

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

}

void Grenade::SetDestionation(float3 destination)
{
	mDestination = destination;
}
