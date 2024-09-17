#pragma once
#include <Script.h>
#include "Macros.h"
#include <vector>
#include "TimerScript.h"

class GameObject;
class TextComponent;
class ImageComponent;
class ButtonComponent;
class PlayerStats;

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
    void NextDialogSet(); // Move to the next dialog set

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

    std::vector<std::vector<std::string>> mDialogList = {
        { 
            "Elios!! Elios! Can you hear me? Please!!!",
            "Eury!! Are you alright?! Tell me what's going on!!",
            "I have no idea... the door of my chamber is locked and my badge simply refuses to work!! I can only heard people the staff screaming in terror!!.",
            "Damn! Something went wrong with the surveillance system of your area! I have no image at all, this doesn't make any sense! I'll go there directly, be safe!"
        },
        { 
            "Elios! The personnel are completely out of control! What's happening?!", 
            "I don't know what the hell is wrong... But something triggered the RCP !", 
            "The RCP?? You mean the Robot Quarantine Protocol?" ,
            "That's the one, Robot Containing Protocol - or Internal Protocol \"GORT\", Red Level. You remember, right?",
            "Yes, it has the \"Highest Priority\" label. You need to be careful, not even your permissions can't stop them now.",
            "And they are going to try to eliminate any lifeform before they can leave the building, including us.",
            "I know you are better than them but stay on your guard.",
        },
        {
            "What happened down there? Just this morning everything was fine.",
            "I'm not sure, something happened with the others. They went crazy all of a sudden and start attacking people.",
            "That's it? Some sort of psychotic attack and now we all have to die?",
            "He released something, Elios. Something dangerous. You'll see when you get down here."
        },
        {
            "Hmmm... I think the elevator for the Lab Staff is broken... Do you have your personal ID card?",
            "Yes... I always have it on me.",
            "You need to use the service elevator to go to the containment zone, they should work even in an emergency state."
        },
        {
            "Hey... how do you think we are going to escape from here?... I'm scared that we might not be able to escape...",
            "Don't worry, with my permission level, there is a route that we can use to escape, even with your permission levels we can escape through.",
            "Yeah, you are right, ... it's just... this situation is starting to make me anxious..."
        },
        {
            "Elios, you should get ready for what's down here.",
            "What do you mean?",
            "Everyone is already dead",
            "Everyone? Did the robots did it?.",
            "No... Something down here has *affected* the staff, changed them. They are no longer humans Elios. And they are way more dangerous than the robots."
        },
        {
            "*Sight*, in a way we deserve what's happening you know. What we were up to down here, the things we did to them. We are just getting our fair comeuppance.",
            "Don't say that, none of this is your fault. This shit is just plain horrible.",
            "Maybe it's just human nature, humanity takes pride on being explorers, on going everywhere and unveiling everything.",
            "Eury... are you ok?",
            "But maybe those are just delusions of grandeur to hide the fact that they are going where they do not belong, and harming things that should be left alone...",
            "..."
        },
        {
            "I think that was the last of them. Next should be your room.",
            "Finally! Just get me out of here quickly please! Get me out!",
            "I'm almost there already honey. Just hold on for a moment.",
            "Yes, come get me out now. GET ME OUT!",
            "Calm down, the worst part is already over. We Will get out of here.",
            "GET ME OUT, GET ME OUT, GET ME OUT.",
            "Uuuuh, Eury? You sure you are ok? This is creeping me out even more than the monsters.",
            "GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT,  GET ME OUT, GET ME OUT...",
            "...Shit."
        }
    };
    int mCurrentDialogSet = 0;  // Index of the current dialog set
    int mCurrentDialog = 0;      // Index of the current dialog within the set

    int mDialogBGM = -1;

    TimerScript mClickTimout;
    bool mTimeout = false;
    
    PlayerStats* mPlayerStats = nullptr;

    bool firstTime = true;
};
