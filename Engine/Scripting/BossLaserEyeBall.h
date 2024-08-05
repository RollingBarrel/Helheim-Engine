#include "ScriptComponent.h"

class GameObject;

class BossLaserEyeBall : public Script {
public:
    BossLaserEyeBall(GameObject* owner);

    void Start() override;
    void Update() override;
    void SetPhase(bool isActive);

private:
    void RotateLaser();
    GameObject* mGameObject = nullptr;

    float mCurrentAngle;
    float mRotationDirection;
    float mRotationSpeed;
    bool mIsActive;
};

