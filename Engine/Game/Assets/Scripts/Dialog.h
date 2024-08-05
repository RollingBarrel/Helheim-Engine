#pragma once
#include <Script.h>
#include "Macros.h"
#include <vector>
#include "TimerScript.h"

class GameObject;
class TextComponent;
class ImageComponent;
class ButtonComponent;

GENERATE_BODY(Dialog);
class Dialog : public Script 
{
    FRIEND(Dialog)
public:
    Dialog(GameObject* owner);
    ~Dialog();

    void Start() override;
    void Update() override;

    void StartDialog();
private:
    void UpdateDialog();
    void OnClick();
    void Controls();

    GameObject* mDialogGO = nullptr;
    GameObject* mProtagonistGO = nullptr;
    GameObject* mWifeGO = nullptr;
    ImageComponent* mProtagonistImage = nullptr;
    ImageComponent* mWifeImage = nullptr;

    GameObject* mTextGO = nullptr;
    TextComponent* mText = nullptr;
    ButtonComponent* mDialogButton = nullptr;

    // Provisional
    std::string mDialog[5] = { "Knock, knock.", "Who's there?", "Tank.", "Tank who?", "You're welcome."};
    int mCurrentDialog = 0;

    TimerScript mClickTimout;
    bool mTimeout = false;
};
