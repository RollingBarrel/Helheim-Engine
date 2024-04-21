#include "pch.h"
#include "Level1AManager.h"
#include "AudioSourceComponent.h"
#include "Application.h"
#include "GameObject.h"
#include "Keys.h"
#include "ModuleInput.h"
#include "ScriptComponent.h"
#include "PlayerController.h"

CREATE(Level1AManager)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLevel1AMainThemeHolder);
    MEMBER(MemberType::GAMEOBJECT, mPlayerControllerHolder);
    END_CREATE;
}

Level1AManager::Level1AManager(GameObject* owner) : Script(owner)
{
}

Level1AManager::~Level1AManager()
{
}

void Level1AManager::Start()
{
    if (mLevel1AMainThemeHolder != nullptr) {
        mLevel1AMainTheme = (AudioSourceComponent*)mLevel1AMainThemeHolder->GetComponent(ComponentType::AUDIOSOURCE);
        mLevel1AMainTheme->Play();
    }

    if (mPlayerControllerHolder != nullptr) {
        ScriptComponent* PlayerControllerScript = (ScriptComponent*)mPlayerControllerHolder->GetComponent(ComponentType::SCRIPT);
        mPlayerController = (PlayerController*) PlayerControllerScript->GetScriptInstance();
        mPlayerController->Hit(1.0f);
    }
}

void Level1AManager::Update()
{
    GodMode();
}

void Level1AManager::GodMode()
{
    // Key 1 - +HP 
    // Key 2 - -HP 
    if (App->GetInput()->GetKey(Keys::Keys_1) == KeyState::KEY_REPEAT)
    {
        
    }
    // Temporal solution, need to be implemented in the generic GameManagerController
}
