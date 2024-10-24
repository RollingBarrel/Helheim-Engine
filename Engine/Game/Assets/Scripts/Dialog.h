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
    void EnableDialog();
    void OnNextClick();
    void OnSkipClick();
    void Controls();

    void AnimationToMain();
    void StartAnimationToMain();
    void AnimationToIntro();
    void StartAnimationToIntro();
    void AnimationToOutro();
    void StartAnimationToOutro();
    void StartAnimationToEnd();
    void AnimationToEnd();

    void FinishDialogue();

    GameObject* mDialogGO = nullptr;

    GameObject* mProtagonistGO = nullptr;
    GameObject* mWifeGO = nullptr;

    GameObject* mTextGO = nullptr;
    TextComponent* mText = nullptr;

    GameObject* mNextBtnGO = nullptr;
    ButtonComponent* mNextButton = nullptr;

    GameObject* mSkipBtnGO = nullptr;
    ButtonComponent* mSkipButton = nullptr;

    GameObject* mIntroGO = nullptr;
    ImageComponent* mIntroImage = nullptr;
    GameObject* mOutroGO = nullptr;
    ImageComponent* mOutroImage = nullptr;

    GameObject* mCompletedGO = nullptr;
    ImageComponent* mCompletedImage = nullptr;

    GameObject* mTopGO = nullptr;
    Transform2DComponent* mTopTransform = nullptr;
    GameObject* mBotGO = nullptr;
    Transform2DComponent* mBotTransform = nullptr;

    std::vector<std::vector<std::string>> mDialogList = {
        { 
            "Elios!! Elios! Can you hear me? Please!!!",
            "Eury!! Are you alright?! Tell me what's going on!!",
            "I have no idea... the door of my chamber is locked and my badge simply refuses to work!! I can only heard people the staff screaming in terror!!.",
            "Damn! Something went wrong with the surveillance system of your area! I have no image at all, this doesn't make any sense! I'll go there directly, be safe!"
        },
        { 
            "Elios! The personnel are completely out of control! What's happening?!", 
            "I don't know what the hell is wrong... But something triggered the RQP !", 
            "The RQP?? You mean the Robot Quarantine Protocol?" ,
            "That's the one. You remember, right?",
            "Yes, it has the \"Highest Priority\" label. You need to be careful, not even your permissions can't stop them now.",
            "And they are going to try to eliminate any lifeform before they can leave the building, including us.",
            "I know you are better than them but stay on your guard.",
        },
        {
            "I'm not sure, what could have happened. Everyone went crazy all of a sudden and started attacking people.",
            "That's it? Some sort of psychotic attack and now we all have to die?",
            "No, I don't think it was something as simple and unfortunate as that."
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

            "Did you manage to beat them all?",
            "Yeah, they were tough but I managed, however I don't think anyone else have survived...",
            "It's a pity, I guess, but you need to come here, I need you Elios"
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
            "Finally! Just get me out of here quickly please! Get me out!",
            "I'm almost there already. Just hold on for a moment.",
            "Yes, come get me out now. GET ME OUT!",
            "Calm down, the worst part is already over. We Will get out of here.",
            "GET ME OUT, GET ME OUT, GET ME OUT.",
            "Uuuuh, Eury? You sure you are ok? This is creeping me out even more than the monsters.",
            "GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT,  GET ME OUT, GET ME OUT...",
            "..."
        }
    };
    int mCurrentDialogSet = 0; 
    int mCurrentDialog = 0;

    int mDialogBGM = -1;

    float mTypingSpeed = 0.02f;  
    float mTypingTimer = 0.0f;   
    int mCurrentCharIndex = 0;   
    bool mIsTyping = false;    
    std::string mFullText;       

    TimerScript mClickTimout;
    bool mTimeout = false;

    TimerScript mStartTimer;
    bool mStartTimeout = false;
    
    PlayerStats* mPlayerStats = nullptr;

    bool mFirstTime = true;

    // Animations 
    TimerScript mAnimationTimer;
    bool mAnimationToIntro = false;

    bool mAnimationToMain = false;
    bool mAnimationToMainDone = false;
    bool mAnimationToEnd = false;
    bool mAnimationToOutro = false;
};
