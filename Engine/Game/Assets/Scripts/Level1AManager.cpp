#include "pch.h"
#include "Level1AManager.h"
#include "AudioSourceComponent.h"
#include "Application.h"
#include "GameObject.h"

CREATE(Level1AManager)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLevel1AMainThemeHolder);
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
}

void Level1AManager::Update()
{
}
