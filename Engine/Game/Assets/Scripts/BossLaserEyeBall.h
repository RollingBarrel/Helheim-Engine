#include <Script.h>
#include "Macros.h"

class GameObject;

GENERATE_BODY(BossLaserEyeBall);

class BossLaserEyeBall : public Script 
{
    FRIEND(BossLaserEyeBall)

public:
    BossLaserEyeBall(GameObject* owner);

    void Start() override;
    void Update() override;
    void Init(float damage, float distance, float duration, float rotationSpeed);

private:

    void RotateLaser();

    float mDamage = 3.0f;
    float mDistance = 10.0f;
    float mDuration = 5.0f;
    float mRotationSpeed = 2.0f;
    float mCurrentRotation = 0.0f;
    bool mRotatingRight = true;

    float mElapsedTime = 0.0f;

    GameObject* mLaserOrigin = nullptr;
    GameObject* mLaserTrail = nullptr;
    GameObject* mLaserEnd = nullptr;

    //Laser Trail WorkAround
    bool mMoveTrail = false;
};

