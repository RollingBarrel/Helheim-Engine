#pragma once
#include <Script.h>
#include "Macros.h"
#include "Math/float3.h"


GENERATE_BODY(Target);
class Target : public Script
{
    FRIEND(Target)
    public:
        Target(GameObject* owner);
        void Start() override;
        void Update() override;

        //target health

        //target take damage
        void TakeDamage(float damage);

     private:
            float mTargetHealth = 100.0f;
};
