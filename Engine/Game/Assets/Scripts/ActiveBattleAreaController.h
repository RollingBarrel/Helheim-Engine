#pragma once
#include <Script.h>
#include "Macros.h"
class BattleArea;
class GameObject;
class BoxColliderComponent;
struct CollisionData;
GENERATE_BODY(ActiveBattleAreaController);
class ActiveBattleAreaController : public Script
{
	FRIEND(ActiveBattleAreaController)
public:
	ActiveBattleAreaController(GameObject* owner);
	~ActiveBattleAreaController();
	void Start();
	void Update();
	void OnCollisionEnter(CollisionData* collisionData);

private:
	GameObject* mBattleAreaGO = nullptr;
	BattleArea* mBattleArea = nullptr;
	//Collider
	BoxColliderComponent* mCollider=nullptr;
};

