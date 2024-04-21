#pragma once
#include <Script.h>
#include "Macros.h"

class AudioSourceComponent;

GENERATE_BODY(Level1AManager);

class Level1AManager :public Script
{
	// Level 1 ABOVE

	FRIEND(Level1AManager)

public:
	Level1AManager(GameObject* owner);
	~Level1AManager();
	void Start() override;
	void Update() override;

private:
	GameObject* mLevel1AMainThemeHolder = nullptr;
	AudioSourceComponent* mLevel1AMainTheme = nullptr;
};

