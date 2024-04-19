#include "pch.h"
#include "Target.h"

CREATE(Target)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mTargetHealth);
    END_CREATE;
}


//inicialize the target 
Target::Target(GameObject* owner) : Script(owner)
{
	
}
void Target::Update()
{
}

void Target::Start()
{
    // Set the target health to 100
    mTargetHealth = 100.0f;
}

// Function to take damage
void Target::TakeDamage(float damage)
{
    if (mTargetHealth <= 0)
    {
		return;
	}
    // Subtract the damage from the target health
    mTargetHealth -= damage;
    //LOG THE TARGET HEALTH
    LOG("Target Health: %f", mTargetHealth);

}

