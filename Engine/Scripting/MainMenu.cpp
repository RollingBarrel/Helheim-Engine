#include "pch.h"
#include "MainMenu.h"

CREATE(MainMenu)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mTest);
    END_CREATE;
}


//inicialize the target 
MainMenu::MainMenu(GameObject* owner) : Script(owner)
{

}
void MainMenu::Update()
{
}

void MainMenu::Start()
{
    // Set the target health to 100
    mTest = 100.0f;
}



