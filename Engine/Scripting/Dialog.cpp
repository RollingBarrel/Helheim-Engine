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
#include "Transform2DComponent.h"

CREATE(Dialog)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mDialogGO);

    MEMBER(MemberType::GAMEOBJECT, mProtagonistGO);
    MEMBER(MemberType::GAMEOBJECT, mWifeGO);

    MEMBER(MemberType::GAMEOBJECT, mTextGO);

    MEMBER(MemberType::GAMEOBJECT, mNextBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mSkipBtnGO);

    MEMBER(MemberType::GAMEOBJECT, mIntroGO);
    MEMBER(MemberType::GAMEOBJECT, mOutroGO);

    MEMBER(MemberType::GAMEOBJECT, mCompletedGO);

    MEMBER(MemberType::GAMEOBJECT, mTopGO);
    MEMBER(MemberType::GAMEOBJECT, mBotGO);
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
    if (mNextBtnGO)
    {
        mNextButton = static_cast<ButtonComponent*>(mNextBtnGO->GetComponent(ComponentType::BUTTON));
        mNextButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Dialog::OnNextClick, this)));
    }
    if (mSkipBtnGO)
    {
        mSkipButton = static_cast<ButtonComponent*>(mSkipBtnGO->GetComponent(ComponentType::BUTTON));
        mSkipButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Dialog::OnSkipClick, this)));
    }

    if (mTextGO) mText = static_cast<TextComponent*>(mTextGO->GetComponent(ComponentType::TEXT));
    if (mIntroGO) mIntroImage = static_cast<ImageComponent*>(mIntroGO->GetComponent(ComponentType::IMAGE));
    if (mOutroGO) mOutroImage = static_cast<ImageComponent*>(mOutroGO->GetComponent(ComponentType::IMAGE));
    if (mCompletedGO) mCompletedImage = static_cast<ImageComponent*>(mCompletedGO->GetComponent(ComponentType::IMAGE));

    if (mTopGO) mTopTransform = static_cast<Transform2DComponent*>(mTopGO->GetComponent(ComponentType::TRANSFORM2D));
    if (mBotGO) mBotTransform = static_cast<Transform2DComponent*>(mBotGO->GetComponent(ComponentType::TRANSFORM2D));


    // Disable images
    mTopGO->SetEnabled(false);
    mBotGO->SetEnabled(false);
    mIntroGO->SetEnabled(false);
    static_cast<ImageComponent*>(mGameObject->GetComponent(ComponentType::IMAGE))->SetEnable(false);

    // Initial preparations
    mPlayerStats = App->GetScene()->GetPlayerStats();
    mCurrentDialogSet = mPlayerStats->GetDialogIndex();
    mDialogBGM = GameManager::GetInstance()->GetAudio()->Play(SFX::DIALOG, mDialogBGM, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
    GameManager::GetInstance()->GetAudio()->Pause(SFX::DIALOG, mDialogBGM, true);
}

void Dialog::Update()
{
    if (mStartTimeout && !mStartTimer.DelayWithoutReset(3.0f)) return;
    else if (mStartTimeout) EnableDialog();

    if (mTimeout && mClickTimout.Delay(2.0f)) mTimeout = false;
 
    if (!mAnimationToMainDone && mAnimationTimer.DelayWithoutReset(1.5f)) StartAnimationToMain();
    if (mAnimationToIntro) AnimationToIntro();
    if (mAnimationToMain) AnimationToMain();

    if (mAnimationToOutro) AnimationToOutro();
    if (mAnimationToEnd && mAnimationTimer.DelayWithoutReset(0.75f)) AnimationToEnd();

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
    if (mTimeout) return;

    if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN ||
        App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
    {
        OnNextClick();
    }
	if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN ||
		App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN)
	{
		OnSkipClick();
	}
}

void Dialog::AnimationToMain()
{

    if (mTopTransform->GetPosition().y < 300.0f) mTopTransform->SetPosition(mTopTransform->GetPosition() + float3(0.0f, 1000.0f, 0.0f) * App->GetDt());
    if (mBotTransform->GetPosition().y > -300.0f) mBotTransform->SetPosition(mBotTransform->GetPosition() - float3(0.0f, 1000.0f, 0.0f) * App->GetDt());
    else 
    {
        mWifeGO->SetEnabled(true);
        mTextGO->SetEnabled(true);

        mCurrentCharIndex = 0;
        mTypingTimer = 0.0f;
        mIsTyping = true;
        mText->SetText("");

        mNextBtnGO->SetEnabled(true);
        mSkipBtnGO->SetEnabled(true);

        mAnimationToMain = false;
        mAnimationToMainDone = true;
    }
}

void Dialog::StartAnimationToMain()
{
    mAnimationToMain = true;

    mTopTransform->SetSize(float2(1078.0f, mTopTransform->GetSize().y));
    mBotTransform->SetSize(float2(1078.0f, mBotTransform->GetSize().y));

    mIntroGO->SetEnabled(false);
    mProtagonistGO->SetEnabled(false);

    mTextGO->SetEnabled(false);
    mNextBtnGO->SetEnabled(false);
    mSkipBtnGO->SetEnabled(false);
}

void Dialog::AnimationToIntro()
{
    if (mTopTransform->GetSize().x < 1078.0f) mTopTransform->SetSize(mTopTransform->GetSize() + float2(2000.0f, 0.0f) * App->GetDt());
    if (mBotTransform->GetSize().x < 1078.0f) mBotTransform->SetSize(mBotTransform->GetSize() + float2(2000.0f, 0.0f) * App->GetDt());
    else 
    {
        mIntroGO->SetEnabled(true);
        mAnimationToIntro = false;
    }
}

void Dialog::StartAnimationToIntro()
{
    mAnimationToIntro = true;

    mBotTransform->SetPosition(float3(0.0f, -55.0f, 0.0f));
    mTopTransform->SetPosition(float3(0.0f, 55.0f, 0.0f));

    mTopTransform->SetSize(float2(0.01f, mTopTransform->GetSize().y));
    mBotTransform->SetSize(float2(0.01f, mBotTransform->GetSize().y));

    mIntroGO->SetEnabled(false);
    mOutroGO->SetEnabled(false);
    mAnimationTimer.Reset();
    mAnimationToMainDone = false;
}

void Dialog::AnimationToOutro()
{
    if (mTopTransform->GetPosition().y > 55.0f) mTopTransform->SetPosition(mTopTransform->GetPosition() - float3(0.0f, 1000.0f, 0.0f) * App->GetDt());
    if (mBotTransform->GetPosition().y < -55.0f) mBotTransform->SetPosition(mBotTransform->GetPosition() + float3(0.0f, 1000.0f, 0.0f) * App->GetDt());
    else
    {
        mOutroGO->SetEnabled(true);
        StartAnimationToEnd();
        mAnimationToOutro = false;
    }
}

void Dialog::StartAnimationToOutro()
{
    if (mAnimationToOutro || mAnimationToEnd) return;
    mAnimationToOutro = true;

    mBotTransform->SetPosition(float3(0.0f, -300.0f, 0.0f));
    mTopTransform->SetPosition(float3(0.0f, 300.0f, 0.0f));

    mProtagonistGO->SetEnabled(false);
    mWifeGO->SetEnabled(false);
    mTextGO->SetEnabled(false);

    mNextBtnGO->SetEnabled(false);
    mSkipBtnGO->SetEnabled(false);
}

void Dialog::AnimationToEnd()
{
    mOutroGO->SetEnabled(false);
    if (mTopTransform->GetSize().x > 0.0f) mTopTransform->SetSize(mTopTransform->GetSize() - float2(2000.0f, 0.0f) * App->GetDt());
    if (mBotTransform->GetSize().x > 0.0f) mBotTransform->SetSize(mBotTransform->GetSize() - float2(2000.0f, 0.0f) * App->GetDt());
    else
    {
        mAnimationToEnd = false;
        FinishDialogue();
    }
}

void Dialog::StartAnimationToEnd()
{
    mAnimationTimer.Reset();
    mAnimationToEnd = true;
}

void Dialog::StartDialog()
{
    if (!mFirstTime)
    {
        mCompletedGO->SetEnabled(true);
    }
    else 
    {
        EnableDialog();
        return;
    }

    mStartTimeout = true;
    mStartTimer.Reset();

    mTopGO->SetEnabled(false);
    mBotGO->SetEnabled(false);
    mIntroGO->SetEnabled(false);
    static_cast<ImageComponent*>(mGameObject->GetComponent(ComponentType::IMAGE))->SetEnable(false);
}

void Dialog::EnableDialog()
{
    if (!GameManager::GetInstance()->IsDialogueActive()) 
    {
        FinishDialogue();
        return;
    }
    
    mCompletedGO->SetEnabled(false);
    mStartTimeout = false;

    mTopGO->SetEnabled(true);
    mBotGO->SetEnabled(true);
    mIntroGO->SetEnabled(true);
    static_cast<ImageComponent*>(mGameObject->GetComponent(ComponentType::IMAGE))->SetEnable(true);

    GameManager::GetInstance()->SetPaused(true, false);
    mCurrentDialog = 0;
    mTimeout = true;
    mClickTimout.Reset();
    StartAnimationToIntro();

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

void Dialog::OnNextClick()
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
        StartAnimationToOutro();

        return;
    }

    // Swap between Protagonist and Wife images
    if (!mWifeGO->IsEnabled())
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DIALOG_FEMALE);

        mProtagonistGO->SetEnabled(false);
        mWifeGO->SetEnabled(true);
    }
    else
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DIALOG_MALE);

        mProtagonistGO->SetEnabled(true);
        mWifeGO->SetEnabled(false);
    }


    mCurrentDialog++;
    UpdateDialog();
}

void Dialog::OnSkipClick()
{
    if (mTimeout) return;
    StartAnimationToOutro();
}

void Dialog::FinishDialogue()
{
    NextDialogSet();
    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
    if (mFirstTime) mFirstTime = false;
    else GameManager::GetInstance()->GetHud()->SetSanity();
    GameManager::GetInstance()->GetAudio()->Pause(SFX::DIALOG, mDialogBGM, true);
}