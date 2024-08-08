#pragma once
#include "Script.h"
#include "Macros.h"

GENERATE_BODY(LoreCollectible);

class GameObject;
class BoxColliderComponent;
class CollisionData;
class TextComponent;
class ImageComponent;
class ResourceTexture;

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

private:

    BoxColliderComponent* mCollider = nullptr;
    std::string* mLoreText = nullptr;
    //In case we want to use image
    ResourceTexture* mImageComp = nullptr;

};

