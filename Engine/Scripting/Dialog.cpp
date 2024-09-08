#include "Dialog.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "ImageComponent.h"
#include "ButtonComponent.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"
#include "HudController.h"
#include "Keys.h"
#include "ModuleInput.h"
#include "PlayerStats.h"
#include "ModuleScene.h"

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
    mPlayerStats = App->GetScene()->GetPlayerStats();
    mCurrentDialogSet = mPlayerStats->GetDialogIndex();
    mDialogBGM = GameManager::GetInstance()->GetAudio()->Play(SFX::DIALOG, mDialogBGM, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
    GameManager::GetInstance()->GetAudio()->Pause(SFX::DIALOG, mDialogBGM, true);
}

void Dialog::Update()
{
    if (mTimeout && mClickTimout.Delay(1.0f)) mTimeout = false;
    Controls();
}

void Dialog::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN ||
        App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
    {
        OnClick();
    }
}

void Dialog::StartDialog()
{
    GameManager::GetInstance()->SetPaused(true, false);
    mCurrentDialog = 0;
    mTimeout = true;
    UpdateDialog();
    GameManager::GetInstance()->GetAudio()->Pause(SFX::DIALOG, mDialogBGM, false);
    GameManager::GetInstance()->GetAudio()->SetPosition(SFX::DIALOG, mDialogBGM, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());

}

void Dialog::NextDialogSet()
{
    if (mCurrentDialogSet < mDialogList.size() - 1)
    {
        mCurrentDialogSet++; 
        mPlayerStats->SetDialogIndex(mCurrentDialogSet);
    }
}

void Dialog::UpdateDialog()
{
    if (mText && !mDialogList.empty() && mCurrentDialogSet < mDialogList.size() &&
        mCurrentDialog < mDialogList[mCurrentDialogSet].size())
    {
        mText->SetText(mDialogList[mCurrentDialogSet][mCurrentDialog]);
    }
}

void Dialog::OnClick()
{
    if (mTimeout) return;

    // Check if the current dialog set is over
    if (mCurrentDialog == mDialogList[mCurrentDialogSet].size() - 1)
    {
        NextDialogSet();
        mGameObject->SetEnabled(false);
        GameManager::GetInstance()->SetPaused(false, false);
        if (firstTime) firstTime = false;
        else GameManager::GetInstance()->GetHud()->SetSanity();
        GameManager::GetInstance()->GetAudio()->Pause(SFX::DIALOG, mDialogBGM, true);

        return;
    }

    // Swap between Protagonist and Wife images
    if (!mProtagonistImage->IsEnabled())
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DIALOG_MALE);

        mProtagonistImage->SetEnable(true);
        mWifeImage->SetEnable(false);
    }
    else
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DIALOG_FEMALE);

        mProtagonistImage->SetEnable(false);
        mWifeImage->SetEnable(true);
    }

    mCurrentDialog++;
    UpdateDialog();
}
