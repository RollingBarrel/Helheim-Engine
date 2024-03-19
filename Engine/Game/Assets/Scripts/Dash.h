#pragma once
#include <Script.h>
#include "Macros.h"

class Dash : public Script
{
public:
    Dash(GameObject* owner);
	void Start() override;
	void Update() override;
    float speed = 20;
    float coolDown = 0;
    float lenght = 5;
    float movement = 0;

private:
    bool up = false, startCounter = false;
    float timePassed = 0;

    bool isDashActive = false;
};

extern "C" SCRIPTING_API Script * CreateDash(GameObject * owner, std::vector<ScriptVariable*> data)
{ 
    Dash* script = new Dash(owner);

    data.push_back(new ScriptVariable("speed", VariableType::FLOAT, &script->speed));
    data.push_back(new ScriptVariable("coolDown", VariableType::FLOAT, &script->coolDown));
    data.push_back(new ScriptVariable("lenght", VariableType::FLOAT, &script->lenght));
    data.push_back(new ScriptVariable("movement", VariableType::FLOAT, &script->movement));

    return script;

}