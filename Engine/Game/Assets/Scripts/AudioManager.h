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

	BOSS,

	GAMEOVER,

	INTRO_VIDEO
};

enum class SFX
{
	// MAIN MENU
	MAINMENU_SELECT,
	MAINMENU_OK,
	MAINMENU_START,
	MAINMENU_CANCEL,

	// PLAYER
	PLAYER_FOOTSTEP_FLOOR1,
	PLAYER_FOOTSTEP_FLOOR2,
	PLAYER_FOOTSTEP_FLOOR3,
	PLAYER_FOOTSTEP_FLOOR4,

	PLAYER_FOOTSTEP_METAL1,
	PLAYER_FOOTSTEP_METAL2,
	PLAYER_FOOTSTEP_METAL3,
	PLAYER_FOOTSTEP_METAL4,

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

	PLAYER_DASH1,
	PLAYER_DASH2,
	PLAYER_DASH3,
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
	ELECTRICAL_TRAP1,
	ELECTRICAL_TRAP2,
	ELECTRICAL_TRAP3,
	ELEVATOR_OPEN_CLOSE,
	ELEVATOR,

	// ENEMY
	ENEMY_DEATH,
	ENEMY_ROBOT_FOOTSTEP,
	ENEMY_ROBOT_GUNFIRE,
	ENEMY_ROBOT_SLASH1,
	ENEMY_ROBOT_SLASH2,
	ENEMY_ROBOT_HIT,
	ENEMY_ROBOT_DEATH,

	ENEMY_CREATURE_CHARGE,
	ENEMY_CREATURE_CHARGE_ATTACK,
	ENEMY_CREATURE_LASER,
	ENEMY_CREATURE_HIT,
	ENEMY_CREATURE_DEATH,

	ENEMY_RAT_DEATH,

	ENEMY_EXPLOSIVE_STEPS,
	ENEMY_EXPLOSIVE_PREEXPLOSION,
	ENEMY_EXPLOSIVE_EXPLOSION,

	BOSS_SCREAM,
	BOSS_AWAKE,
	BOSS_FIRE,
	BOSS_LASER,
	BOSS_ROAR_BULLET,
	BOSS_ROAR_ERUPTION,
	BOSS_ROAR_LASER,
	BOSS_HIT,
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
	void Restart(SFX sfx, int id);

	int Release(BGM bgm, int id);
	int Release(SFX sfx, int id);

	void ReleaseAllAudio();

	void UpdateParameterValueByName(BGM bgm, int id, const char* name, const float value);
	void UpdateParameterValueByName(SFX sfx, int id, const char* name, const float value);

	void SetPosition(SFX sfx, int id, float3 position);
	void SetPosition(const FMOD::Studio::EventDescription* description, int id, float3 position);

	void SetLoop(BGM bgm, int id, bool isLoop);

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
		{BGM::LEVEL2, "event:/MC/lvl1/mc_lvl1"},
		//{BGM::LEVEL2, "event:/MC/lvl2/mc_lvl2"},

		{BGM::BOSS, "event:/MC/boss/boss_lvl"},
		//{BGM::BOSS, "event:/MC/main/Theme"},

		{BGM::GAMEOVER, "Assets/FMOD/Audios/bgm/die.mp3"},

		{BGM::INTRO_VIDEO, "Assets/FMOD/Audios/bgm/introvideo.mp3"},

	};

	const std::unordered_map<SFX, std::string> mSFXToString
	{
		// MAIN MENU
		{SFX::MAINMENU_SELECT, "Assets/FMOD/Audios/sfx/select2.wav"},
		{SFX::MAINMENU_OK, "Assets/FMOD/Audios/sfx/ok.wav"},
		{SFX::MAINMENU_CANCEL, "Assets/FMOD/Audios/sfx/cancel.wav"},
		{SFX::MAINMENU_START, "Assets/FMOD/Audios/sfx/gamestart.wav"},

		// PLAYER
		{SFX::PLAYER_FOOTSTEP_FLOOR1, "Assets/FMOD/Audios/sfx/Footstep Concrete Walking 1_01.wav"}, // [X]
		{SFX::PLAYER_FOOTSTEP_FLOOR2, "Assets/FMOD/Audios/sfx/Footstep Concrete Walking 1_02.wav"}, // [X]
		{SFX::PLAYER_FOOTSTEP_FLOOR3, "Assets/FMOD/Audios/sfx/Footstep Concrete Walking 1_03.wav"}, // [X]
		{SFX::PLAYER_FOOTSTEP_FLOOR4, "Assets/FMOD/Audios/sfx/Footstep Concrete Walking 1_04.wav"}, // [X]

		{SFX::PLAYER_FOOTSTEP_METAL1, "Assets/FMOD/Audios/sfx/Footstep Metal Waking 1_01.wav"}, // [X]
		{SFX::PLAYER_FOOTSTEP_METAL2, "Assets/FMOD/Audios/sfx/Footstep Metal Waking 1_02.wav"}, // [X]
		{SFX::PLAYER_FOOTSTEP_METAL3, "Assets/FMOD/Audios/sfx/Footstep Metal Waking 1_03.wav"}, // [X]
		{SFX::PLAYER_FOOTSTEP_METAL4, "Assets/FMOD/Audios/sfx/Footstep Metal Waking 1_04.wav"}, // [X]

		{SFX::PLAYER_PISTOL, "event:/SFX/CHAR/main_normal"},
		{SFX::PLAYER_MACHINEGUN, "event:/SFX/CHAR/main_machinegun"},
		{SFX::PLAYER_SHOTGUN, "event:/SFX/CHAR/main_shotgun"},

		{SFX::PLAYER_ULTIMATE, "event:/SFX/CHAR/main_ultimate"},

		{SFX::PLAYER_HIT, "Assets/FMOD/Audios/sfx/generalhit.wav"},
		{SFX::PLAYER_BROKEN, "Assets/FMOD/Audios/sfx/broken.wav"},
		{SFX::PLAYER_DANGER, "Assets/FMOD/Audios/sfx/danger.wav"},

		{SFX::PLAYER_DASH1, "Assets/FMOD/Audios/sfx/dash.wav"},
		{SFX::PLAYER_DASH2, "Assets/FMOD/Audios/sfx/dash2.wav"},
		{SFX::PLAYER_DASH3, "Assets/FMOD/Audios/sfx/dash3.wav"},

		{SFX::PLAYER_PICK, "Assets/FMOD/Audios/sfx/pick.wav"},
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
		{SFX::ELECTRICAL_TRAP1, "Assets/FMOD/Audios/sfx/electric.wav"},
		{SFX::ELECTRICAL_TRAP2, "Assets/FMOD/Audios/sfx/electric2.wav"},
		{SFX::ELECTRICAL_TRAP3, "Assets/FMOD/Audios/sfx/electric3.wav"},
		{SFX::ELEVATOR, "Assets/FMOD/Audios/sfx/elevator.wav"},
		{SFX::ELEVATOR_OPEN_CLOSE, "Assets/FMOD/Audios/sfx/elevatorstop.wav"},

		// ENEMY
		{SFX::ENEMY_DEATH, "Assets/FMOD/Audios/sfx/generalenemydeath.wav"}, // [X]
		{SFX::ENEMY_ROBOT_FOOTSTEP, "Assets/FMOD/Audios/sfx/robotfootstep.wav"}, // [X]
		{SFX::ENEMY_ROBOT_GUNFIRE, "event:/SFX/NPC/robot_distance"},
		{SFX::ENEMY_ROBOT_SLASH1, "Assets/FMOD/Audios/sfx/slash.wav"},
		{SFX::ENEMY_ROBOT_SLASH2, "Assets/FMOD/Audios/sfx/slash2.wav"},
		{SFX::ENEMY_ROBOT_HIT, "Assets/FMOD/Audios/sfx/enemyrobothit.wav"},
		{SFX::ENEMY_ROBOT_DEATH, "Assets/FMOD/Audios/sfx/robotdeath.wav"}, // [X]

		{SFX::ENEMY_CREATURE_CHARGE, "Assets/FMOD/Audios/sfx/creaturecharge.mp3"},
		{SFX::ENEMY_CREATURE_CHARGE_ATTACK, "Assets/FMOD/Audios/sfx/creatureattack.wav"},
		{SFX::ENEMY_CREATURE_LASER, "Assets/FMOD/Audios/sfx/laser.wav"},
		{SFX::ENEMY_CREATURE_HIT, "Assets/FMOD/Audios/sfx/enemycreaturehit.wav"},
		{SFX::ENEMY_CREATURE_DEATH, "Assets/FMOD/Audios/sfx/creaturedeath.wav"},

		{SFX::ENEMY_RAT_DEATH, "Assets/FMOD/Audios/sfx/ratdeath.mp3"},

		{SFX::ENEMY_EXPLOSIVE_STEPS, "Assets/FMOD/Audios/sfx/explosivestep.wav"},
		{SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, "Assets/FMOD/Audios/sfx/explosivecharge.wav"},
		{SFX::ENEMY_EXPLOSIVE_EXPLOSION, "Assets/FMOD/Audios/sfx/explosiveexplosion.wav"},

		{SFX::BOSS_SCREAM, "Assets/FMOD/Audios/sfx/bossscream.wav"},
		{SFX::BOSS_AWAKE, "Assets/FMOD/Audios/sfx/bossawake.wav"},
		{SFX::BOSS_FIRE, "event:/SFX/BOSS/boss_fire"},
		{SFX::BOSS_LASER, "event:/SFX/BOSS/boss_laser"},

		{SFX::BOSS_ROAR_BULLET, "Assets/FMOD/Audios/sfx/bossbullet.wav"},
		{SFX::BOSS_ROAR_ERUPTION, "Assets/FMOD/Audios/sfx/bosseruption.wav"},
		{SFX::BOSS_ROAR_LASER, "Assets/FMOD/Audios/sfx/bosslaser.wav"},
		{SFX::BOSS_HIT, "Assets/FMOD/Audios/sfx/bosshit.wav"},

	};
};