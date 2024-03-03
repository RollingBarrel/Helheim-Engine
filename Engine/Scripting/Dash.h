#pragma once
#include <Script.h>
class Dash : public Script
{
public:

	void Start() override;
	void Update() override;

private:
    float speed = 1;
    float coolDown = 2;
    float lenght = 3;
    float movement = 0;
    bool up = false, startCounter = false;
    float timePassed = 0;



};

