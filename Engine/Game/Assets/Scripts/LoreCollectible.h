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
    //In case we want to use image
    ResourceTexture* mImageComp = nullptr;
    TimerScript mInteractTimer;
    TimerScript mColorTimer;

    bool mChange = false;
    float mColor = 255.0f;
    GameObject* mMeshGO = nullptr;

    GameObject* mTitleTextGO = nullptr;
    std::string* mTitleText = nullptr;
    GameObject* mSubtitleTextGO = nullptr;
    std::string* mSubtitleText = nullptr;
};

