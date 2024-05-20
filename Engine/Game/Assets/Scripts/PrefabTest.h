#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(PrefabTest);
class PrefabTest : public Script
{
    FRIEND(PrefabTest)
public:
	PrefabTest(GameObject* owner) : Script(owner) {}
	~PrefabTest() {}
	void Awake() override;
	void Start() override;
	void Update() override;


};

