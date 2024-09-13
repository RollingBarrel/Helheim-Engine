#pragma once
#include <Script.h>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include "Macros.h"
#include "float3.h"

enum class BGM 
{
	MAINMENU,
	LEVEL1,
	LEVEL2,

	GAMEOVER
};

enum class SFX 
{
	// MAIN MENU
	MAINMENU_SELECT,
	MAINMENU_OK,
	MAINMENU_START,
	MAINMENU_CANCEL,

	// PLAYER
	PLAYER_FOOTSTEP,

	PLAYER_PISTOL,
	PLAYER_MACHINEGUN,
	PLAYER_SHOTGUN,

	PLAYER_MEELEE,
	PLAYER_KATANA,
	PLAYER_HAMMER,

	PLAYER_ULTIMATE,

	PLAYER_HIT,
	PLAYER_BROKEN,
	PLAYER_DANGER,

	PLAYER_DASH,
	PLAYER_INTERACT,
	PLAYER_PICK,
	PLAYER_HEAL,
	PLAYER_THROW,
	PLAYER_BLACKHOLE1,
	PLAYER_BLACKHOLE2,

	PLAYER_DEATH1,
	PLAYER_DEATH2,

	// DIALOG
	DIALOG,
	DIALOG_MALE,
	DIALOG_FEMALE,

	// SCEENE
	DOOR,

	// ENEMY
	ENEMY_DEATH,
	ENEMY_ROBOT_FOOTSTEP,
	ENEMY_ROBOT_GUNFIRE,
	ENEMY_ROBOT_SLASH,
	ENEMY_ROBOT_HIT,

	ENEMY_CREATURE_CHARGE,
	ENEMY_CREATURE_LASER,

	ENEMY_EXPLOSIVE_STEPS,
	ENEMY_EXPLOSIVE_PREEXPLOSION,
	ENEMY_EXPLOSIVE_EXPLOSION,
};

namespace FMOD 
{
	namespace Studio 
	{
		class EventInstance;
		class EventDescription;
	}
	class Channel;
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

	void ReleaseAllAudio();

	void UpdateParameterValueByName(BGM bgm, int id, const char* name, const float value);
	void UpdateParameterValueByName(SFX sfx, int id, const char* name, const float value);

	void SetPosition(SFX sfx, int id, float3 position);
	void SetPosition(const FMOD::Studio::EventDescription* description, int id, float3 position);

	void AddAudioToASComponent(BGM bgm);
	void AddAudioToASComponent(SFX sfx);
	void AddAudioToASComponent();
private:
	std::string GetBGMName(BGM bgm);
	std::string GetSFXName(SFX sfx);
	const AudioUnit* GetAudioUnit(BGM bgm);
	const AudioUnit* GetAudioUnit(SFX sfx);

	const FMOD::Studio::EventDescription* GetEventDescription(BGM bgm);
	const FMOD::Studio::EventDescription* GetEventDescription(SFX sfx);

	bool IsPlayeble(SFX sfx);

	// Handle Custom Audio
	int PlayBGM(const std::string& fileName);
	int PlaySFX(const std::string& fileName, float3 position = { 0.0f, 0.0f, 0.0f });
	void SetPosition(int id, float3 position);

	void Pause(int id, bool state);
	void PlayOneShot(const std::string& fileName, float3 eventPosition);

	int Release(int id);

	AudioSourceComponent* mAudioSources = nullptr;

	std::vector<FMOD::Channel*> mStreamAudios;
	std::unordered_map<SFX, std::chrono::steady_clock::time_point> mLastPlayedTime;

	// Audio - Name
	const std::unordered_map<BGM, std::string> mBGMToString
	{
		{BGM::MAINMENU, "event:/MC/main/Theme"},
		{BGM::LEVEL1, "event:/MC/lvl1/mc_lvl1"},
		{BGM::LEVEL2, "event:/MC/lvl2/mc_lvl2"},

		{BGM::GAMEOVER, "Assets/FMOD/Audios/bgm/die.mp3"},

	};

	const std::unordered_map<SFX, std::string> mSFXToString
	{
		// MAIN MENU
		{SFX::MAINMENU_SELECT, "Assets/FMOD/Audios/sfx/select2.wav"},
		{SFX::MAINMENU_OK, "Assets/FMOD/Audios/sfx/ok.wav"},
		{SFX::MAINMENU_CANCEL, "Assets/FMOD/Audios/sfx/cancel.wav"},
		{SFX::MAINMENU_START, "Assets/FMOD/Audios/sfx/gamestart.wav"},

		// PLAYER
		{SFX::PLAYER_FOOTSTEP, "Assets/FMOD/Audios/sfx/footstep.wav"}, // [X]

		{SFX::PLAYER_PISTOL, "event:/SFX/CHAR/main_normal"},
		{SFX::PLAYER_MACHINEGUN, "event:/SFX/CHAR/main_machinegun"},
		{SFX::PLAYER_SHOTGUN, "event:/SFX/CHAR/main_shotgun"},

		{SFX::PLAYER_ULTIMATE, "event:/SFX/CHAR/main_ultimate"},

		{SFX::PLAYER_HIT, "Assets/FMOD/Audios/sfx/generalhit.wav"},
		{SFX::PLAYER_BROKEN, "Assets/FMOD/Audios/sfx/broken.wav"},
		{SFX::PLAYER_DANGER, "Assets/FMOD/Audios/sfx/danger.wav"},

		{SFX::PLAYER_PICK, "Assets/FMOD/Audios/sfx/pick.wav"},
		{SFX::PLAYER_DASH, "Assets/FMOD/Audios/sfx/dash.wav"},
		{SFX::PLAYER_INTERACT, "Assets/FMOD/Audios/sfx/pick.wav"},
		{SFX::PLAYER_HEAL, "Assets/FMOD/Audios/sfx/heal.wav"},

		{SFX::PLAYER_THROW, "Assets/FMOD/Audios/sfx/throw.wav"},
		{SFX::PLAYER_BLACKHOLE1, "Assets/FMOD/Audios/sfx/grenade_preexplosion.wav"},
		{SFX::PLAYER_BLACKHOLE2, "Assets/FMOD/Audios/sfx/grenade_explosion.wav"},

		{SFX::PLAYER_DEATH1, "Assets/FMOD/Audios/sfx/die.wav"},
		{SFX::PLAYER_DEATH2, "Assets/FMOD/Audios/sfx/die2.wav"},


		// DIALOG
		{SFX::DIALOG, "Assets/FMOD/Audios/bgm/dialog2.wav"},
		{SFX::DIALOG_FEMALE, "Assets/FMOD/Audios/sfx/female.wav"}, // [X]
		{SFX::DIALOG_MALE, "Assets/FMOD/Audios/sfx/male.wav"}, // [X]

		// SCENE
		{SFX::DOOR, "Assets/FMOD/Audios/sfx/door.wav"},

		// ENEMY
		{SFX::ENEMY_DEATH, "Assets/FMOD/Audios/sfx/generalenemydeath.wav"}, // [X]
		{SFX::ENEMY_ROBOT_FOOTSTEP, "Assets/FMOD/Audios/sfx/robotfootstep.wav"}, // [X]
		{SFX::ENEMY_ROBOT_GUNFIRE, "event:/SFX/NPC/robot_distance"},
		{SFX::ENEMY_ROBOT_SLASH, "Assets/FMOD/Audios/sfx/slash.wav"},
		{SFX::ENEMY_ROBOT_HIT, "Assets/FMOD/Audios/sfx/enemyrobothit.wav"}, // [X]

		{SFX::ENEMY_CREATURE_CHARGE, "Assets/FMOD/Audios/sfx/creaturecharge.wav"},
		{SFX::ENEMY_CREATURE_LASER, "Assets/FMOD/Audios/sfx/laser.wav"},

		{SFX::ENEMY_EXPLOSIVE_STEPS, "Assets/FMOD/Audios/sfx/explosivestep.wav"},
		{SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, "Assets/FMOD/Audios/sfx/explosivecharge.wav"},
		{SFX::ENEMY_EXPLOSIVE_EXPLOSION, "Assets/FMOD/Audios/sfx/explosiveexplosion.wav"},
	};
};