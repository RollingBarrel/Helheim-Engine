#include "Sanity.h"

#include "Application.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "ImageComponent.h"
#include "ButtonComponent.h"
#include "GameManager.h"
#include "PlayerController.h"
#include "AudioManager.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "Transform2DComponent.h"
#include "ModuleScene.h"
#include "PlayerStats.h"

CREATE(Sanity)
{
    CLASS(owner);
    SEPARATOR("Cards");
    MEMBER(MemberType::GAMEOBJECT, mCard1GO);
    MEMBER(MemberType::GAMEOBJECT, mCard2GO);
    MEMBER(MemberType::GAMEOBJECT, mCard3GO);

    SEPARATOR("Speed Box");
    MEMBER(MemberType::GAMEOBJECT, mSpeedBox1);
    MEMBER(MemberType::GAMEOBJECT, mSpeedBox2);
    MEMBER(MemberType::GAMEOBJECT, mSpeedBox3);
    MEMBER(MemberType::GAMEOBJECT, mSpeedBox4);
    MEMBER(MemberType::GAMEOBJECT, mSpeedBox5);
    MEMBER(MemberType::GAMEOBJECT, mSpeedBox6);
    MEMBER(MemberType::GAMEOBJECT, mSpeedBox7);

    SEPARATOR("Health Box");
    MEMBER(MemberType::GAMEOBJECT, mHealthBox1);
    MEMBER(MemberType::GAMEOBJECT, mHealthBox2);
    MEMBER(MemberType::GAMEOBJECT, mHealthBox3);
    MEMBER(MemberType::GAMEOBJECT, mHealthBox4);
    MEMBER(MemberType::GAMEOBJECT, mHealthBox5);
    MEMBER(MemberType::GAMEOBJECT, mHealthBox6);
    MEMBER(MemberType::GAMEOBJECT, mHealthBox7);

    SEPARATOR("Damage Box");
    MEMBER(MemberType::GAMEOBJECT, mDmgBox1);
    MEMBER(MemberType::GAMEOBJECT, mDmgBox2);
    MEMBER(MemberType::GAMEOBJECT, mDmgBox3);
    MEMBER(MemberType::GAMEOBJECT, mDmgBox4);
    MEMBER(MemberType::GAMEOBJECT, mDmgBox5);
    MEMBER(MemberType::GAMEOBJECT, mDmgBox6);
    MEMBER(MemberType::GAMEOBJECT, mDmgBox7);

    END_CREATE;
}

Sanity::Sanity(GameObject* owner) : Script(owner)
{
    // Create buff selections
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.1f), Buff(Buff::StatType::MOVEMENT, 1.10f), Buff(Buff::StatType::HEALTH, 1.10f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.1f), Buff(Buff::StatType::MOVEMENT, 1.10f), Buff(Buff::StatType::HEALTH, 1.10f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.1f), Buff(Buff::StatType::MOVEMENT, 1.10f), Buff(Buff::StatType::HEALTH, 1.10f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.15f), Buff(Buff::StatType::MOVEMENT, 1.15f), Buff(Buff::StatType::HEALTH, 1.15f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.15f), Buff(Buff::StatType::MOVEMENT, 1.15f), Buff(Buff::StatType::HEALTH, 1.15f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.20f), Buff(Buff::StatType::MOVEMENT, 1.20f), Buff(Buff::StatType::HEALTH, 1.20f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.20f), Buff(Buff::StatType::MOVEMENT, 1.20f), Buff(Buff::StatType::HEALTH, 1.20f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.20f), Buff(Buff::StatType::MOVEMENT, 1.20f), Buff(Buff::StatType::HEALTH, 1.20f) });
}

Sanity::~Sanity()
{
}

void Sanity::Start()
{
    if (mCard1GO)
    {
        const std::vector<GameObject*> children1 = mCard1GO->GetChildren();
        mCard1Text = static_cast<TextComponent*>(children1[3]->GetComponent(ComponentType::TEXT));
        mCard1Btn = static_cast<ButtonComponent*>(children1[2]->GetComponent(ComponentType::BUTTON));
        mCard1Image = static_cast<ImageComponent*>(children1[2]->GetComponent(ComponentType::IMAGE));
        mCard1Hover = children1[0];
        mCard1Btn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Sanity::OnCard1Click, this)));
        mCard1Btn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&Sanity::OnCard1HoverOn, this)));
        mCard1Btn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&Sanity::OnCard1HoverOff, this)));
    }
    
    if (mCard2GO)
    {
        const std::vector<GameObject*> children2 = mCard2GO->GetChildren();
        mCard2Text = static_cast<TextComponent*>(children2[3]->GetComponent(ComponentType::TEXT));
        mCard2Btn = static_cast<ButtonComponent*>(children2[2]->GetComponent(ComponentType::BUTTON));
        mCard2Image = static_cast<ImageComponent*>(children2[2]->GetComponent(ComponentType::IMAGE));
        mCard2Hover = children2[0];
        mCard2Btn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Sanity::OnCard2Click, this)));
        mCard2Btn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&Sanity::OnCard2HoverOn, this)));
        mCard2Btn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&Sanity::OnCard2HoverOff, this)));
    }

    if (mCard3GO)
    {
        const std::vector<GameObject*> children3 = mCard3GO->GetChildren();
        mCard3Text = static_cast<TextComponent*>(children3[3]->GetComponent(ComponentType::TEXT));
        mCard3Btn = static_cast<ButtonComponent*>(children3[2]->GetComponent(ComponentType::BUTTON));
        mCard3Image = static_cast<ImageComponent*>(children3[2]->GetComponent(ComponentType::IMAGE));
        mCard3Hover = children3[0];
        mCard3Btn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Sanity::OnCard3Click, this)));
        mCard3Btn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&Sanity::OnCard3HoverOn, this)));
        mCard3Btn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&Sanity::OnCard3HoverOff, this)));
    }

    CreateSelection(0);

    mHealthLvl = App->GetScene()->GetPlayerStats()->GetHealthLvl();
    mSpeedLvl = App->GetScene()->GetPlayerStats()->GetSpeedLvl();
    mDmgLvl = App->GetScene()->GetPlayerStats()->GetDamageLvl();
    SetDamageBoxes();
    SetHealthBoxes();
    SetSpeedBoxes();
}

void Sanity::Update()
{
    Controls();

    if (mTimeout && mClickTimout.Delay(1.0f)) mTimeout = false;
    mTimePassed += App->GetDt();
}

void Sanity::Controls() 
{
    if (mTimeout) return;
    if (App->GetInput()->GetKey(Keys::Keys_RIGHT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == ButtonState::BUTTON_DOWN || ((App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) > 0.9f && mTimePassed >= mDebounceTime)))
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_SELECT);

        mTimePassed = 0.0f; // Restart debounce timer

        if (mCurrentBuff == 0)
        {
            mCurrentBuff = 2;
        }
        else
        {
            mCurrentBuff--;
        }
        CardHover();
    }

    if (App->GetInput()->GetKey(Keys::Keys_LEFT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT) == ButtonState::BUTTON_DOWN || ((App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) < -0.9f && mTimePassed >= mDebounceTime)))
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_SELECT);

        mTimePassed = 0.0f; // Restart debounce timer

        if (mCurrentBuff > 1)
        {
            mCurrentBuff = 0;
        }
        else
        {
            mCurrentBuff++;
        }
        CardHover();
    }

    if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN ||
        App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_OK);

        CardClick();
    }
}

void Sanity::CreateSelection(int arena)
{
    if (arena < 0 || arena >= mBuffSelection.size()) return;

    GameManager::GetInstance()->SetPaused(true, false);
    
    mTimeout = true;
    CardHover();

    mCurrentBuffs = mBuffSelection[arena];

    // Card 1
    if (!mCurrentBuffs.empty())
    {
        Buff buff1 = mCurrentBuffs[0];
        mCard1Text->SetText(GetBuffDescription(buff1));
    }

    // Card 2
    if (mCurrentBuffs.size() > 1)
    {
        Buff buff2 = mCurrentBuffs[1];
        mCard2Text->SetText(GetBuffDescription(buff2));
    }

    // Card 3
    if (mCurrentBuffs.size() > 2)
    {
        Buff buff3 = mCurrentBuffs[2];
        mCard3Text->SetText(GetBuffDescription(buff3));
    }
}

std::string Sanity::GetBuffDescription(const Buff& buff)
{
    switch (buff.getStatType())
    {
        case Buff::StatType::MOVEMENT:
            return "SPEED +" + std::to_string(((int)((buff.getValue()-1)*100))) + "%";
        case Buff::StatType::DAMAGE:
            return "DAMAGE +" + std::to_string(((int)((buff.getValue() - 1) * 100))) + "%";
        case Buff::StatType::HEALTH:
            return "HEALTH +" + std::to_string(((int)((buff.getValue() - 1) * 100))) + "%";
        default:
            return "";
    }
}

void Sanity::CardClick()
{
    switch (mCurrentBuff)
    {
        case 0:
            OnCard1Click();
            break;
        case 1:
            OnCard2Click();
            break;
        case 2:
            OnCard3Click();
            break;
        default:
            break;
    }
}

void Sanity::CardHover()
{
    switch (mCurrentBuff)
    {
    case 0:
        OnCard1HoverOn();
        break;
    case 1:
        OnCard2HoverOn();
        break;
    case 2:
        OnCard3HoverOn();
        break;
    default:
        break;
    }
}

void Buff::Consume()
{
    switch (stat) 
    {
        case Buff::StatType::MOVEMENT:
            GameManager::GetInstance()->GetPlayerController()->SetMovementSpeedStat(value);
            break;
        case Buff::StatType::DAMAGE:
            GameManager::GetInstance()->GetPlayerController()->SetWeaponDamage(value);
            break;
        case Buff::StatType::HEALTH:
            GameManager::GetInstance()->GetPlayerController()->SetMaxShield(value);
            break;
        default:
            break;
    }
}

void Sanity::OnCard1Click()
{
    if (mTimeout) return;

    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_OK);

    if (!mCurrentBuffs.empty())
        mCurrentBuffs.at(0).Consume();

    mDmgLvl++;
    SetDamageBoxes();
    App->GetScene()->GetPlayerStats()->SetDamageLvl(mDmgLvl);
    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
}

void Sanity::OnCard1HoverOn()
{
    if (mTimeout) return;
    if (mCard1Hovered) return;
    mCard1Hovered = true;

    OnCard1HoverOff();
    OnCard2HoverOff();
    OnCard3HoverOff();

    mCard1Image->SetAlpha(1.0f);
    mCard1Hover->SetEnabled(true);
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_SELECT);
}

void Sanity::OnCard1HoverOff()
{
    mCard1Hovered = false;
    mCard1Image->SetAlpha(0.75f);
    mCard1Hover->SetEnabled(false);
}

void Sanity::OnCard2Click()
{
    if (mTimeout) return;

    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_OK);

    if (mCurrentBuffs.size() > 1)
        mCurrentBuffs.at(1).Consume();
    
    mSpeedLvl++;
    SetSpeedBoxes();
    App->GetScene()->GetPlayerStats()->SetSpeedLvl(mSpeedLvl);
    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
}

void Sanity::OnCard2HoverOn()
{
    if (mTimeout) return;
    if (mCard2Hovered) return;
    mCard2Hovered = true;

    OnCard1HoverOff();
    OnCard2HoverOff();
    OnCard3HoverOff();

    mCard2Image->SetAlpha(1.0f);
    mCard2Hover->SetEnabled(true);
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_SELECT);
}

void Sanity::OnCard2HoverOff()
{
    mCard2Hovered = false;
    mCard2Image->SetAlpha(0.75f);
    mCard2Hover->SetEnabled(false);
}

void Sanity::OnCard3Click()
{
    if (mTimeout) return;

    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_OK);

    if (mCurrentBuffs.size() > 2)
        mCurrentBuffs.at(2).Consume();

    mHealthLvl++;
    SetHealthBoxes();
    App->GetScene()->GetPlayerStats()->SetHealthLvl(mHealthLvl);
    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
}

void Sanity::OnCard3HoverOn()
{
    if (mTimeout) return;
    if (mCard3Hovered) return;
    mCard3Hovered = true;

    OnCard1HoverOff();
    OnCard2HoverOff();
    OnCard3HoverOff();

    mCard3Image->SetAlpha(1.0f);
    mCard3Hover->SetEnabled(true);
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_SELECT);
}

void Sanity::OnCard3HoverOff()
{
    mCard3Image->SetAlpha(0.75f);
    mCard3Hovered = false;
    mCard3Hover->SetEnabled(false);
}

void Sanity::SetSpeedBoxes()
{
    switch (mSpeedLvl)
    {
        case 7:
            mSpeedBox7->SetEnabled(true);
            [[fallthrough]];
        case 6:
            mSpeedBox6->SetEnabled(true);
            [[fallthrough]];
        case 5:
            mSpeedBox5->SetEnabled(true);
            [[fallthrough]];
        case 4:
            mSpeedBox4->SetEnabled(true);
            [[fallthrough]];
        case 3:
            mSpeedBox3->SetEnabled(true);
            [[fallthrough]];
        case 2:
            mSpeedBox2->SetEnabled(true);
            [[fallthrough]];
        case 1:
            mSpeedBox1->SetEnabled(true);
            [[fallthrough]];
        default:
            break;
    }
}

void Sanity::SetHealthBoxes()
{
    switch (mHealthLvl)
    {
    case 7:
        mHealthBox7->SetEnabled(true);
        [[fallthrough]];
    case 6:
        mHealthBox6->SetEnabled(true);
        [[fallthrough]];
    case 5:
        mHealthBox5->SetEnabled(true);
        [[fallthrough]];
    case 4:
        mHealthBox4->SetEnabled(true);
        [[fallthrough]];
    case 3:
        mHealthBox3->SetEnabled(true);
        [[fallthrough]];
    case 2:
        mHealthBox2->SetEnabled(true);
        [[fallthrough]];
    case 1:
        mHealthBox1->SetEnabled(true);
        [[fallthrough]];
    default:
        break;
    }
}

void Sanity::SetDamageBoxes()
{
    switch (mDmgLvl)
    {
    case 7:
        mDmgBox7->SetEnabled(true);
        [[fallthrough]];
    case 6:
        mDmgBox6->SetEnabled(true);
        [[fallthrough]];
    case 5:
        mDmgBox5->SetEnabled(true);
        [[fallthrough]];
    case 4:
        mDmgBox4->SetEnabled(true);
        [[fallthrough]];
    case 3:
        mDmgBox3->SetEnabled(true);
        [[fallthrough]];
    case 2:
        mDmgBox2->SetEnabled(true);
        [[fallthrough]];
    case 1:
        mDmgBox1->SetEnabled(true);
        [[fallthrough]];
    default:
        break;
    }
}
