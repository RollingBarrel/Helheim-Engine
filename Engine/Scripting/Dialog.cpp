#include "Dialog.h"

#include "Application.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "ImageComponent.h"
#include "ButtonComponent.h"
#include "GameManager.h"
#include "PlayerController.h"
#include "HudController.h"

CREATE(Dialog)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mDialogGO);
    MEMBER(MemberType::GAMEOBJECT, mProtagonistGO);
    MEMBER(MemberType::GAMEOBJECT, mWifeGO);
    MEMBER(MemberType::GAMEOBJECT, mTextGO);
    END_CREATE;
}

Dialog::Dialog(GameObject* owner) : Script(owner)
{
}

Dialog::~Dialog()
{
}

void Dialog::Start()
{
    if (mDialogGO) mDialogButton = static_cast<ButtonComponent*>(mDialogGO->GetComponent(ComponentType::BUTTON));
    mDialogButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Dialog::OnClick, this)));
    if (mProtagonistGO) mProtagonistImage = static_cast<ImageComponent*>(mProtagonistGO->GetComponent(ComponentType::IMAGE));
    if (mWifeGO) mWifeImage = static_cast<ImageComponent*>(mWifeGO->GetComponent(ComponentType::IMAGE));
    if (mTextGO) mText = static_cast<TextComponent*>(mTextGO->GetComponent(ComponentType::TEXT));

}

void Dialog::Update()
{
    if (mTimeout && mClickTimout.Delay(2.0f)) mTimeout = false;
}

void Dialog::StartDialog()
{
    GameManager::GetInstance()->SetPaused(true, false);
    mCurrentDialog = 0;
    mTimeout = true;
    UpdateDialog();
}

void Dialog::UpdateDialog()
{
    mText->SetText(mDialog[mCurrentDialog]);
}

void Dialog::OnClick() 
{
    if (mTimeout) return;

    if (mCurrentDialog < 4) 
    {
        if (*mProtagonistImage->GetAlpha() == 0.5f) {
            mProtagonistImage->SetAlpha(1.0f);
            mWifeImage->SetAlpha(0.5f);
        }
        else 
        {
            mProtagonistImage->SetAlpha(0.5f);
            mWifeImage->SetAlpha(1.0f);
        }
        mCurrentDialog++;
        UpdateDialog();
    }
    else
    {
        mGameObject->SetEnabled(false);
        GameManager::GetInstance()->SetPaused(false, false);
        GameManager::GetInstance()->GetHud()->SetSanity();
    }
}

