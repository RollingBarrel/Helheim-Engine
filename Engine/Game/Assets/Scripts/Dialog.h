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
            "Elios!! Elios! Can you hear me, honey? Please!!! I need you to come here right now!",
            "Eury!! Are you alright babe?! Tell me what’s going on!!",
            "I-I have no idea… the door of my chamber is locked and my badge simply refuses to work!! I can see nothing but smoke and sparks and there’s people from the staff screaming in terror!!.",
            "Damn! Something went wrong with the surveillance system of your area! I have no image at all, for God’s sake, this doesn’t make any sense! Alright: I’m on my way! Just try your best to hold on babe! I’ll be there in a moment !"
        },
        { 
            "Elios! I can hear gunshots and roars from here! The personnel are completely out of control! What’s happening in God’s name?!", 
            "I don’t know what the hell is wrong… But something triggered the RCP !", 
            "The RCP?? You mean the Robot Quarantine Protocol?" ,
            "That’s the one, Robot Containing Protocol - or Internal Protocol “GORT”, Red Level. You remember from the training, right?",
            "I remember “Highest Priority” label… and the outcome implying all Synthetics, Robots, Androids from non critical halls can get instantly reprogrammed to… Oh my God!!…",
            "Correct. To prevent any form of life from entering the underneath Labs, and to compromise the integrity of power plant’s Key sectors… At any cost.",
            "Oh my God! Elios! IT MEANS those damned machines are gonna chase you down! I don’t even want to imagine…",
            "Eury don’t run in panic! You know me, right?! I’m fully equipped with the best tech ever and those rusty scrap heaps don’t stand a single chance against my skills! I’ll make my own way towards you, no matter the cost!"
        },
        {
            "What happened down there? Just this morning everything was fine.",
            "I'm not sure, something happened with the []. He went crazy all of a sudden, started breaking stuff and attacking people.",
            "That's it? Some sort of psychotic attack and now we all have to die?",
            "He released something, Elios. Something dangerous. You'll see when you get down here."
        },
        {
            "Hmmm… I think the elevator for the Lab Staff is broken… Do you have your personal ID card?",
            "Yes… I always have it on me.",
            "You need to use the service elevator to go to the containment zone, they should work even in an emergency state."
        },
        {
            "Hey… how do you think we are going to escape from here?… I'm scared that we might not be able to escape…",
            "Don’t worry, with my permission level, there is a route that we can use to escape, even with your permission levels we can escape through.",
            "Yeah, you are right, … it’s just… this situation is starting to make me anxious…"
        },
        {
            "Elios, you should get ready for what's down here.",
            "What do you mean?",
            "You won't find robots here, those have already been destroyed",
            "Destroyed? By who? Did the guards take back the floor.",
            "No... Well in a sense. Something down here has *affected* people somehow, changed them. I don't know what they will look like now, but remember, they are no longer humans Elios. And they are way more dangerous than the robots."
        },
        {
            "*Sight*, in a way we deserve what's happening you know. What we were up to down here, the things we did to them. We are just getting our fair comeuppance.",
            "Don't say that, none of this is your fault. This shit is just plain horrible.",
            "Maybe it's just human nature, humanity takes pride on being explorers, on going everywhere and unveiling everything. But maybe those are just delusions of grandeur to hide the fact that they are going where they do not belong, and harming things that should be left alone… It's just unchecked expansionism… like a virus.",
            "Honey? Are you… are you ok?",
            "…"
        },
        {
            "Done! I think that was the last of them. Next should be your room.",
            "Finally! I'll open the door for you, just get me out of here quickly please! Get me out!",
            "I'm almost there already honey. Just hold on for a moment.",
            "Yes, come get me out now. GET ME OUT!",
            "Calm down, the worst part is already over. We Will get out of here no problemo.",
            "GET ME OUT, GET ME OUT, GET ME OUT.",
            "Uuuuh, [NAME]? You sure you are ok? This is creeping me out even more than the monsters.",
            "GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT,GET ME OUT, GET ME OUT, GET ME OUT...",
            "…Shit."
        }
    };
    int mCurrentDialogSet = 0;  // Index of the current dialog set
    int mCurrentDialog = 0;      // Index of the current dialog within the set

    TimerScript mClickTimout;
    bool mTimeout = false;
    
    PlayerStats* mPlayerStats = nullptr;
};
