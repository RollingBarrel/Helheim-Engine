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

GENERATE_BODY(AudioManager);
class AudioManager : public Script
{
	FRIEND(AudioManager)

public:
	AudioManager(GameObject* owner);
	~AudioManager();

	void Start();
	void Update();

	void Play(BGM bgm, int& instanceid);
	void Play(const std::string& audioName, int& instanceid);
	void PlayOneShot(const std::string& audioName, float3 audioPosition, const std::unordered_map<std::string, int>& parameters = {});


	void Release(int audioIndex);

private:
	std::string GetBGMName(BGM sfx);
	std::string GetSFXName(SFX sfx);
	AudioSourceComponentNew* mAudioSources = nullptr;

	std::unordered_map<int, FMOD::Studio::EventInstance*> mBackgroundAudioMap;
	std::vector<AudioUnit*> mSFXList;

	int mBackgroundAudioCounter = 0;

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