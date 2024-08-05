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
#include "ModuleInput.h"
#include "Keys.h"

CREATE(Sanity)
{
    CLASS(owner);
    SEPARATOR("Cards");
    MEMBER(MemberType::GAMEOBJECT, mCard1GO);
    MEMBER(MemberType::GAMEOBJECT, mCard2GO);
    MEMBER(MemberType::GAMEOBJECT, mCard3GO);
    END_CREATE;
}

Sanity::Sanity(GameObject* owner) : Script(owner)
{
    // Create buff selections
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.1f), Buff(Buff::StatType::MOVEMENT, 1.10f), Buff(Buff::StatType::HEALTH, 1.10f) });
    mBuffSelection.push_back({ Buff(Buff::StatType::DAMAGE, 1.15f), Buff(Buff::StatType::MOVEMENT, 1.15f), Buff(Buff::StatType::HEALTH, 1.15f) });
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
        mCard1Image = static_cast<ImageComponent*>(mCard1GO->GetComponent(ComponentType::IMAGE));
        mCard1Image->SetAlpha(0.7f);
        mCard1TitleText = static_cast<TextComponent*>(children1[0]->GetComponent(ComponentType::TEXT));
        mCard1BuffImage = static_cast<ImageComponent*>(children1[1]->GetComponent(ComponentType::IMAGE));
        mCard1Text = static_cast<TextComponent*>(children1[2]->GetComponent(ComponentType::TEXT));
        mCard1Btn = static_cast<ButtonComponent*>(mCard1GO->GetComponent(ComponentType::BUTTON));
        mCard1Btn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Sanity::OnCard1Click, this)));
        mCard1Btn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&Sanity::OnCard1HoverOn, this)));
        mCard1Btn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&Sanity::OnCard1HoverOff, this)));
    }
    
    if (mCard2GO)
    {
        const std::vector<GameObject*> children2 = mCard2GO->GetChildren();
        mCard2Image = static_cast<ImageComponent*>(mCard2GO->GetComponent(ComponentType::IMAGE));
        mCard2Image->SetAlpha(0.7f);
        mCard2TitleText = static_cast<TextComponent*>(children2[0]->GetComponent(ComponentType::TEXT));
        mCard2BuffImage = static_cast<ImageComponent*>(children2[1]->GetComponent(ComponentType::IMAGE));
        mCard2Text = static_cast<TextComponent*>(children2[2]->GetComponent(ComponentType::TEXT));
        mCard2Btn = static_cast<ButtonComponent*>(mCard2GO->GetComponent(ComponentType::BUTTON));
        mCard2Btn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Sanity::OnCard2Click, this)));
        mCard2Btn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&Sanity::OnCard2HoverOn, this)));
        mCard2Btn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&Sanity::OnCard2HoverOff, this)));
    }

    if (mCard1GO)
    {
        const std::vector<GameObject*> children3 = mCard3GO->GetChildren();
        mCard3Image = static_cast<ImageComponent*>(mCard3GO->GetComponent(ComponentType::IMAGE));
        mCard3Image->SetAlpha(0.7f);
        mCard3TitleText = static_cast<TextComponent*>(children3[0]->GetComponent(ComponentType::TEXT));
        mCard3BuffImage = static_cast<ImageComponent*>(children3[1]->GetComponent(ComponentType::IMAGE));
        mCard3Text = static_cast<TextComponent*>(children3[2]->GetComponent(ComponentType::TEXT));
        mCard3Btn = static_cast<ButtonComponent*>(mCard3GO->GetComponent(ComponentType::BUTTON));
        mCard3Btn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&Sanity::OnCard3Click, this)));
        mCard3Btn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&Sanity::OnCard3HoverOn, this)));
        mCard3Btn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&Sanity::OnCard3HoverOff, this)));
    }

    CreateSelection(0);
}

void Sanity::Update()
{
    Controls();

    if (mTimeout && mClickTimout.Delay(1.0f)) mTimeout = false;
}

void Sanity::Controls() 
{
    if (App->GetInput()->GetKey(Keys::Keys_RIGHT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == ButtonState::BUTTON_DOWN)
    {
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

    if (App->GetInput()->GetKey(Keys::Keys_LEFT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT) == ButtonState::BUTTON_DOWN)
    {
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

    if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN ||
        App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
    {
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
        mCard1TitleText->SetText(GetBuffTitle(buff1));
        mCard1Text->SetText(GetBuffDescription(buff1));
        //mCard1BuffImage->SetImage(GetImage(buff1));
    }

    // Card 2
    if (mCurrentBuffs.size() > 1)
    {
        Buff buff2 = mCurrentBuffs[1];
        mCard2TitleText->SetText(GetBuffTitle(buff2));
        mCard2Text->SetText(GetBuffDescription(buff2));
        //mCard2BuffImage->SetImage(GetImage(buff2));
    }

    // Card 3
    if (mCurrentBuffs.size() > 2)
    {
        Buff buff3 = mCurrentBuffs[2];
        mCard3TitleText->SetText(GetBuffTitle(buff3));
        mCard3Text->SetText(GetBuffDescription(buff3));
        //mCard3BuffImage->SetImage(GetImage(buff3));
    }
}

std::string Sanity::GetBuffTitle(const Buff& buff)
{
    switch (buff.getStatType()) 
    {
        case Buff::StatType::MOVEMENT:
            return "Movement Buff";
        case Buff::StatType::DAMAGE:
            return "Damage Buff";
        case Buff::StatType::HEALTH:
            return "Health Buff";
        default:
            return "";
    }
}

std::string Sanity::GetBuffDescription(const Buff& buff)
{
    switch (buff.getStatType())
    {
        case Buff::StatType::MOVEMENT:
            return "Increases movement speed by " + std::to_string(((int)((buff.getValue()-1)*100))) + "%";
        case Buff::StatType::DAMAGE:
            return "Increases damage by " + std::to_string(((int)((buff.getValue() - 1) * 100))) + "%";
        case Buff::StatType::HEALTH:
            return "Increases health by " + std::to_string(((int)((buff.getValue() - 1) * 100))) + "%";
        default:
            return "";
    }
}

unsigned int Sanity::GetImage(const Buff& buff)
{
    switch (buff.getStatType()) 
    {
        case Buff::StatType::MOVEMENT:
            return App->GetResource()->Find("ui-speed-buff.png");
        case Buff::StatType::DAMAGE:
            return App->GetResource()->Find("ui-image-pistol.png");
        case Buff::StatType::HEALTH:
            return App->GetResource()->Find("ui-health-buff.png");
        default:
            return 0;
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
    OnCard1HoverOff();
    OnCard2HoverOff();
    OnCard3HoverOff();

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
            GameManager::GetInstance()->GetPlayerController()->SetMovementSpeed(value);
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

    if (!mCurrentBuffs.empty())
        mCurrentBuffs.at(0).Consume();

    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
}

void Sanity::OnCard1HoverOn()
{
    if (mCard1Image)
        mCard1Image->SetAlpha(0.95f);
}

void Sanity::OnCard1HoverOff()
{
    if (mCard1Image)
        mCard1Image->SetAlpha(0.6f);
}

void Sanity::OnCard2Click()
{
    if (mTimeout) return;

    if (mCurrentBuffs.size() > 1)
        mCurrentBuffs.at(1).Consume();

    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
}

void Sanity::OnCard2HoverOn()
{
    if (mCard2Image)
        mCard2Image->SetAlpha(0.95f);
}

void Sanity::OnCard2HoverOff()
{
    if (mCard2Image)
        mCard2Image->SetAlpha(0.6f);
}

void Sanity::OnCard3Click()
{
    if (mTimeout) return;

    if (mCurrentBuffs.size() > 2)
        mCurrentBuffs.at(2).Consume();

    mGameObject->SetEnabled(false);
    GameManager::GetInstance()->SetPaused(false, false);
}

void Sanity::OnCard3HoverOn()
{
    if (mCard3Image)
        mCard3Image->SetAlpha(0.95f);
}

void Sanity::OnCard3HoverOff()
{
    if (mCard3Image)
        mCard3Image->SetAlpha(0.6f);
}