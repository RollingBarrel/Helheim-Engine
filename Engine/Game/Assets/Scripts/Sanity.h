#pragma once
#include <Script.h>
#include "Macros.h"
#include <vector>
#include "TimerScript.h"

class GameObject;
class TextComponent;
class ImageComponent;
class ButtonComponent;
class Transform2DComponent;

class Buff {
public:
    enum StatType {
        MOVEMENT,
        DAMAGE,
        HEALTH
    };

    Buff(StatType type, float percentage)
        : stat(type), value(percentage) {}

    StatType getStatType() const { return stat; }
    float getValue() const { return value; }

    void Consume();

private:
    StatType stat;
    float value;
};

GENERATE_BODY(Sanity);
class Sanity : public Script {
    FRIEND(Sanity)
public:
    Sanity(GameObject* owner);
    ~Sanity();

    void Start() override;
    void CreateSelection(int arena);
    void Update() override;

private:
    void Controls();
    std::string GetBuffDescription(const Buff& buff);

    void CardClick();
    void CardHover();

    void OnCard1Click();
    void OnCard1HoverOn();
    void OnCard1HoverOff();
    void OnCard2Click();
    void OnCard2HoverOn();
    void OnCard2HoverOff();
    void OnCard3Click();
    void OnCard3HoverOn();
    void OnCard3HoverOff();

    std::vector<std::vector<Buff>> mBuffSelection;
    std::vector<Buff> mCurrentBuffs;

    GameObject* mCard1GO = nullptr;
    ImageComponent* mCard1Image = nullptr;
    TextComponent* mCard1Text = nullptr;
    ButtonComponent* mCard1Btn = nullptr;
    Transform2DComponent* mCard1Transform = nullptr;

    GameObject* mCard2GO = nullptr;
    ImageComponent* mCard2Image = nullptr;
    TextComponent* mCard2Text = nullptr;
    ButtonComponent* mCard2Btn = nullptr;
    Transform2DComponent* mCard2Transform = nullptr;

    GameObject* mCard3GO = nullptr;
    ImageComponent* mCard3Image = nullptr;
    TextComponent* mCard3Text = nullptr;
    ButtonComponent* mCard3Btn = nullptr;
    Transform2DComponent* mCard3Transform = nullptr;

    TimerScript mClickTimout;
    bool mTimeout = false;

    int mCurrentBuff = 0;
};
