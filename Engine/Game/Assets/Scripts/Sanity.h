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

    void FinishSanity();

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

    void SetSpeedBoxes();
    void SetHealthBoxes();
    void SetDamageBoxes();
    void SetSpeedBoxesSelected(int value);
    void SetHealthBoxesSelected(int value);
    void SetDamageBoxesSelected(int value);

    void ResetSelected();
    void UpdateSelected();

    std::vector<std::vector<Buff>> mBuffSelection;
    std::vector<Buff> mCurrentBuffs;

    GameObject* mCard1GO = nullptr;
    TextComponent* mCard1Text = nullptr;
    ButtonComponent* mCard1Btn = nullptr;
    GameObject* mCard1Hover = nullptr;
    ImageComponent* mCard1Image = nullptr;
    ImageComponent* mCard1Icon = nullptr;
    GameObject* mCard1Glow = nullptr;
    GameObject* mCard1Selected = nullptr;
    bool mCard1Hovered = false;

    GameObject* mCard2GO = nullptr;
    TextComponent* mCard2Text = nullptr;
    ButtonComponent* mCard2Btn = nullptr;
    GameObject* mCard2Hover = nullptr;
    ImageComponent* mCard2Image = nullptr;
    ImageComponent* mCard2Icon = nullptr;
    GameObject* mCard2Glow = nullptr;
    GameObject* mCard2Selected = nullptr;
    bool mCard2Hovered = false;

    GameObject* mCard3GO = nullptr;
    TextComponent* mCard3Text = nullptr;
    ButtonComponent* mCard3Btn = nullptr;
    GameObject* mCard3Hover = nullptr;
    ImageComponent* mCard3Image = nullptr;
    ImageComponent* mCard3Icon = nullptr;
    GameObject* mCard3Glow = nullptr;
    GameObject* mCard3Selected = nullptr;
    bool mCard3Hovered = false;

    TimerScript mClickTimout;
    bool mTimeout = false;

    TimerScript mEndTimer;
    bool mEnd = false;

    int mCurrentBuff = 0;
    float mTimePassed = 0.0f;
    const float mDebounceTime = 0.2f; // 200 ms delay time

    GameObject* mSelected = nullptr;
    ImageComponent* mSelectedImage = nullptr;

    // Speed buff
    int mSpeedLvl = 0;
    GameObject* mSpeedBox1 = nullptr;
    GameObject* mSpeedBox2 = nullptr;
    GameObject* mSpeedBox3 = nullptr;
    GameObject* mSpeedBox4 = nullptr;
    GameObject* mSpeedBox5 = nullptr;
    GameObject* mSpeedBox6 = nullptr;
    GameObject* mSpeedBox7 = nullptr;
    GameObject* mSpeedBox1Selected = nullptr;
    GameObject* mSpeedBox2Selected = nullptr;
    GameObject* mSpeedBox3Selected = nullptr;
    GameObject* mSpeedBox4Selected = nullptr;
    GameObject* mSpeedBox5Selected = nullptr;
    GameObject* mSpeedBox6Selected = nullptr;
    GameObject* mSpeedBox7Selected = nullptr;

    // Health buff
    int mHealthLvl = 0;
    GameObject* mHealthBox1 = nullptr;
    GameObject* mHealthBox2 = nullptr;
    GameObject* mHealthBox3 = nullptr;
    GameObject* mHealthBox4 = nullptr;
    GameObject* mHealthBox5 = nullptr;
    GameObject* mHealthBox6 = nullptr;
    GameObject* mHealthBox7 = nullptr;
    GameObject* mHealthBox1Selected = nullptr;
    GameObject* mHealthBox2Selected = nullptr;
    GameObject* mHealthBox3Selected = nullptr;
    GameObject* mHealthBox4Selected = nullptr;
    GameObject* mHealthBox5Selected = nullptr;
    GameObject* mHealthBox6Selected = nullptr;
    GameObject* mHealthBox7Selected = nullptr;

    // Dmg buff
    int mDmgLvl = 0;
    GameObject* mDmgBox1 = nullptr;
    GameObject* mDmgBox2 = nullptr;
    GameObject* mDmgBox3 = nullptr;
    GameObject* mDmgBox4 = nullptr;
    GameObject* mDmgBox5 = nullptr;
    GameObject* mDmgBox6 = nullptr;
    GameObject* mDmgBox7 = nullptr;
    GameObject* mDmgBox1Selected = nullptr;
    GameObject* mDmgBox2Selected = nullptr;
    GameObject* mDmgBox3Selected = nullptr;
    GameObject* mDmgBox4Selected = nullptr;
    GameObject* mDmgBox5Selected = nullptr;
    GameObject* mDmgBox6Selected = nullptr;
    GameObject* mDmgBox7Selected = nullptr;
};
