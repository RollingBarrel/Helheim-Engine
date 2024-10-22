#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

GENERATE_BODY(LoreCollectible);

class GameObject;
class BoxColliderComponent;
class TextComponent;
class ImageComponent;
class ResourceTexture;

struct CollisionData;


class LoreCollectible : public Script
{
	FRIEND(LoreCollectible)

public:
    LoreCollectible(GameObject* owner);
    ~LoreCollectible() {}

    void Init();
    void Start() override;
    void Update() override;
    void OnCollisionEnter(CollisionData* collisionData);
    void OnCollisionExit(CollisionData* collisionData);
    void CheckDistance();

private:

    bool mColliding = false;
    BoxColliderComponent* mCollider = nullptr;
    std::string* mLoreText = nullptr;
    std::string* mTitleText = nullptr;
    std::string* mSubtitleText = nullptr;

    //In case we want to use image
    ResourceTexture* mImageComp = nullptr;
    TimerScript mInteractTimer;
    TimerScript mColorTimer;

    bool mChange = false;
    float mColor = 255.0f;
    GameObject* mMeshGO = nullptr;

    int mLoreIndex = 0;

    std::vector<std::string> mTitleList = 
    {
    "Log #001",
    "Log #002",
    "Log #003",
    "Log #004",
    "Log #005",
    "Log #006",
    };
    std::vector<std::string> mSubtitleList = 
    {
        "Date: 07/06/3106\n ID: 01A - R1\n Written by : Eury",
        "Date: 25/06/3106\n ID: 01A - R1\n Written by : Eury",
        "Date: 04/10/3106\n ID: 01A - R1\n Written by : Eury",
        "Date: 15/01/3107\n ID: 01A - R1\n Written by : Eury",
        "Date: 14/03/3107\n ID: 01A - R1\n Written by : Eury",
        "Date: 15/03/3107\n ID: 01A - R1\n Written by : Eury",
    };
    std::vector<std::string> mTextList = 
    { 
       "The reactors are fully installed.Today marks the beginning of our experiments with energy extraction from the planet core. The energy flows like a thick, electric violet liquid?it almost seems alive. WeÅfre ready to see what it can do.",
       "The energy levels weÅfre drawing from the core are beyond expectations. ItÅfs far more efficient than solar power. This could be the breakthrough we need?especially with the sun nearing the end of its life. We might not need to rely on it anymore.",
       "Months of tests, and weÅfve begun using the energy to heal living cells. The results are astonishing. WeÅfve even been able to cure diseases. The energy potential seems limitless, but we must be careful.",
       "Side effects are emerging.Prolonged exposure to the energy is causing a symbiosis between machinery and living tissue.Hosts grow pale, start to mutate.The machines, too, act erratically and wonÅft shut down unless we cut their power source.SomethingÅfs wrong.",
       "Today, the core of the planet,what we now call Chrysalite, shook violently. The excessive extraction triggered a massive earthquake, damaging the facility and security systems. WeÅfve lost control of the situation.",
       "Since the beginning of this investigation, IÅfve been experiencing blackouts, losing more of my memory each day. I feel trapped, like IÅfm in a constant dream. ItÅfs the coreÅfs energy?Xyronite?consuming me. I hope I can shut it down before itÅfs too late."   
    };


};

