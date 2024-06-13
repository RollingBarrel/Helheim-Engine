#pragma once
#include <Script.h>
#include "Macros.h"

class GameObject;

class Buff {
    enum StatType
    {
        MOVEMENT,
        DAMAGE,
        HEALTH
    };

    StatType stat;
    int value;
};

GENERATE_BODY(Sanity);
class Sanity : public Script
{
    FRIEND(Sanity)
public:
    Sanity(GameObject* owner);
    ~Sanity();

    void Start() override;
    void Update() override;
private:

    // Maps of possible buffs

    // Selection of buffs

    // Visuals cards
    GameObject* mCardGO1;
    GameObject* mCardGO2;
    GameObject* mCardGO3;
};