#include "pch.h"
#include "Target.h"

#include <utility>
#include <limits.h>
#include "GameObject.h"

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
    // If the target health is less than or equal to 0, destroy the target
    if (mTargetHealth <= 0)
    {
        LOG("Target Destroyed");
    }
}

void Target::Start()
{
    // Set the target health to 100
    mTargetHealth = 100.0f;
}

// Function to take damage
void Target::TakeDamage(float damage)
{
    // Subtract the damage from the target health
    mTargetHealth -= damage;
    //LOG THE TARGET HEALTH
    LOG("Target Health: %f", mTargetHealth);
    if (mTargetHealth <= 0)
    {
        LOG("Target Destroyed");
    }
}

