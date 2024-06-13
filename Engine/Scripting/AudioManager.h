#pragma once
#include <Script.h>
#include <string>
#include <unordered_map>
#include "Macros.h"
#include "float3.h"

enum class BGM {
	MAINMENU,
	LEVEL1
};

enum class SFX {
	// MAIN MENU
	MAINMENU_SELECT,
	MAINMENU_OK,

	// PLAYER
	PLAYER_FOOTSTEP,
	GUNFIRE,

	// ENEMY
	ENEMY_ROBOT_FOOTSTEP,
};

namespace FMOD {
	namespace Studio {
		class EventInstance;
		class EventDescription;
	}
}

class AudioSourceComponentNew;
class AudioUnit;
class GameObject;

GENERATE_BODY(AudioManager);
class AudioManager : public Script
{
	FRIEND(AudioManager)

public:
	AudioManager(GameObject* owner);
	~AudioManager();

	void Start();
	void Update();

	int PlayBGM(BGM bgm);
	// Use PlaySFX only if you want to ajust SFX later (like manually pause shot sound)
	// Other wise, call `PlayOneShot(SFX sfx)`
	int PlaySFX(SFX sfx);
	void PlayOneShot(SFX sfx);

	void PauseAudio(BGM bgm,int id, bool immediate);

	int Release(BGM bgm, int id);
	int Release(SFX sfx, int id);

	void UpdateParameterValueByName(BGM bgm, int id, const char* name, const float value);
	void UpdateParameterValueByName(SFX sfx, int id, const char* name, const float value);

	void SetPositionReference(SFX sfx, int id, const char* name, const float value);

private:
	std::string GetBGMName(BGM bgm);
	std::string GetSFXName(SFX sfx);
	const AudioUnit* GetAudioUnit(BGM bgm);
	const AudioUnit* GetAudioUnit(SFX sfx);

	const FMOD::Studio::EventDescription* GetEventDescription(BGM bgm);
	const FMOD::Studio::EventDescription* GetEventDescription(SFX sfx);
	AudioSourceComponentNew* mAudioSources = nullptr;

	// Audio - Name
	const std::unordered_map<BGM, std::string> mBGMToString
	{
		{BGM::MAINMENU, "event:/Music/Level 01"},
		{BGM::LEVEL1, "event:/Music/Level 02"}
	};

	const std::unordered_map<SFX, std::string> mSFXToString
	{
		// MAIN MENU
		{SFX::MAINMENU_SELECT, "event:/UI/Cancel"},
		{SFX::MAINMENU_OK, "event:/UI/Okay"},

		// PLAYER
		{SFX::PLAYER_FOOTSTEP, "event:/Character/Player Footsteps"},
		{SFX::GUNFIRE, "event:/Weapons/Pistol"},

		// ENEMY
		{SFX::ENEMY_ROBOT_FOOTSTEP, "event:/Character/Enemy Footsteps"},
	};
};