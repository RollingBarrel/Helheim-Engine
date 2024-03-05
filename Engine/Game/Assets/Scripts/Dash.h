#pragma once
#include <Script.h>
#include "Macros.h"

class Dash : public Script
{
public:
    Dash(GameObject* owner);
	void Start() override;
	void Update() override;

private:
    float speed = 20;
    float coolDown = 0;
    float lenght = 5;
    float movement = 0;
    bool up = false, startCounter = false;
    float timePassed = 0;

    bool isDashActive = false;

};

extern "C" SCRIPTING_API Script * CreateDash(GameObject * owner) { return new Dash(owner); }