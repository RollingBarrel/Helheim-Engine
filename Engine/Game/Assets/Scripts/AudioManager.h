#pragma once
#include <Script.h>
#include <string>
#include <unordered_map>
#include "Macros.h"
#include "float3.h"

enum class BGM 
{
	MAINMENU,
	LEVEL1,
	LEVEL2,
};

enum class SFX 
{
	// MAIN MENU
	MAINMENU_SELECT,
	MAINMENU_OK,

	// PLAYER
	PLAYER_FOOTSTEP,

	PLAYER_PISTOL,
	PLAYER_MACHINEGUN,
	PLAYER_SHOTGUN,

	PLAYER_MEELEE,

	PLAYER_ULTIMATE,

	// ENEMY
	ENEMY_ROBOT_FOOTSTEP,
	ENEMY_ROBOT_GUNFIRE,
};

namespace FMOD 
{
	namespace Studio 
	{
		class EventInstance;
		class EventDescription;
	}
}

class AudioUnit;
class GameObject;
class AudioSourceComponent;

GENERATE_BODY(AudioManager);
class AudioManager : public Script
{
	FRIEND(AudioManager)

public:
	AudioManager(GameObject* owner);
	~AudioManager();

	void Awake();
	void Start();
	void Update();

	int Play(BGM bgm, int id = -1, float3 position = { 0.0f, 0.0f, 0.0f });
	// Use Play(SFX) only if you want to ajust SFX later (like manually pause shot sound)
	// Other wise, call `PlayOneShot(SFX sfx)`
	int Play(SFX sfx, int id = -1, float3 position = { 0.0f, 0.0f, 0.0f });
	void PlayOneShot(SFX sfx, float3 position = { 0.0f, 0.0f, 0.0f }, const std::unordered_map <const char*, float>& parameters = {});

	void Pause(BGM bgm, int id, bool pause);
	void Pause(SFX sfx, int id, bool pause);

	int Release(BGM bgm, int id);
	int Release(SFX sfx, int id);

	void UpdateParameterValueByName(BGM bgm, int id, const char* name, const float value);
	void UpdateParameterValueByName(SFX sfx, int id, const char* name, const float value);

	void SetPosition(const FMOD::Studio::EventDescription* description, int id, float3 position);

	void AddAudioToASComponent(BGM bgm);
	void AddAudioToASComponent(SFX sfx);

private:
	std::string GetBGMName(BGM bgm);
	std::string GetSFXName(SFX sfx);
	const AudioUnit* GetAudioUnit(BGM bgm);
	const AudioUnit* GetAudioUnit(SFX sfx);

	const FMOD::Studio::EventDescription* GetEventDescription(BGM bgm);
	const FMOD::Studio::EventDescription* GetEventDescription(SFX sfx);
	AudioSourceComponent* mAudioSources = nullptr;

	// Audio - Name
	const std::unordered_map<BGM, std::string> mBGMToString
	{
		{BGM::MAINMENU, "event:/MC/main/Theme"},
		{BGM::LEVEL1, "event:/MC/lvl1/mc_lvl1"},
		{BGM::LEVEL2, "event:/MC/lvl2/mc_lvl2"}
	};

	const std::unordered_map<SFX, std::string> mSFXToString
	{
		// MAIN MENU
		{SFX::MAINMENU_SELECT, "Assets/FMOD/Audios/sfx/select2.wav"},
		{SFX::MAINMENU_OK, "Assets/FMOD/Audios/sfx/pick.wav"},

		// PLAYER
		{SFX::PLAYER_FOOTSTEP, "event:/Character/Player Footsteps"},

		{SFX::PLAYER_PISTOL, "event:/SFX/CHAR/main_normal"},
		{SFX::PLAYER_MACHINEGUN, "event:/SFX/CHAR/main_machinegun"},
		{SFX::PLAYER_SHOTGUN, "event:/SFX/CHAR/main_shotgun"},

		{SFX::PLAYER_ULTIMATE, "event:/SFX/CHAR/main_ultimate"}, // Draft audio


		// ENEMY
		{SFX::ENEMY_ROBOT_FOOTSTEP, "event:/Character/Enemy Footsteps"},
		{SFX::ENEMY_ROBOT_GUNFIRE, "event:/SFX/NPC/robot_distance"},
	};
};