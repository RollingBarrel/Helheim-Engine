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
    MEMBER(MemberType::GAMEOBJECT, mSkipBtnGO);
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
    if (mDialogGO) 
    {
        mDialogButton = static_cast<ButtonComponent*>(mDialogGO->GetComponent(ComponentType::BUTTON));
        mDialogButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Dialog::OnClick, this)));
    }
    if (mSkipBtnGO)
    {
        mSkipButton = static_cast<ButtonComponent*>(mSkipBtnGO->GetComponent(ComponentType::BUTTON));
        mSkipButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Dialog::OnSkipClick, this)));
    }
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

    // Check if we are currently typing
    if (mIsTyping)
    {
        // Update the timer
        mTypingTimer += App->GetDt();

        // If enough time has passed, reveal the next character
        if (mTypingTimer >= mTypingSpeed && mCurrentCharIndex < mFullText.size())
        {
            // Append one character to the text component
            mText->SetText(*mText->GetText() + mFullText[mCurrentCharIndex]);

            // Move to the next character and reset the timer
            mCurrentCharIndex++;
            mTypingTimer = 0.0f;
        }

        // If we've reached the end of the text, stop typing
        if (mCurrentCharIndex >= mFullText.size())
        {
            mIsTyping = false;
        }
    }
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
    mProtagonistImage->SetEnable(false);
    mWifeImage->SetEnable(true);
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
    // Ensure there is text to display and the dialog set is valid
    if (mText && !mDialogList.empty() && mCurrentDialogSet < mDialogList.size() &&
        mCurrentDialog < mDialogList[mCurrentDialogSet].size())
    {
        // Store the full text of the current dialog
        mFullText = mDialogList[mCurrentDialogSet][mCurrentDialog];

        // Reset the typing parameters
        mCurrentCharIndex = 0;
        mTypingTimer = 0.0f;
        mIsTyping = true;

        // Start with an empty text and gradually reveal the full text
        mText->SetText("");
    }
}

void Dialog::OnClick()
{
    if (mTimeout) return;

    // If typing is still in progress, finish the text instantly
    if (mIsTyping)
    {
        mText->SetText(mFullText);
        mIsTyping = false;
        return;
    }

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
    if (!mWifeImage->IsEnabled())
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DIALOG_FEMALE);

        mProtagonistImage->SetEnable(false);
        mWifeImage->SetEnable(true);
    }
    else
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DIALOG_MALE);

        mProtagonistImage->SetEnable(true);
        mWifeImage->SetEnable(false);
    }


    mCurrentDialog++;
    UpdateDialog();
}

void Dialog::OnSkipClick()
{
    if (mTimeout) return;

    NextDialogSet();
    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
    if (firstTime) firstTime = false;
    else GameManager::GetInstance()->GetHud()->SetSanity();
    GameManager::GetInstance()->GetAudio()->Pause(SFX::DIALOG, mDialogBGM, true);
}
