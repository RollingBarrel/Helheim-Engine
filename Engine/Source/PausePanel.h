#pragma once
#include "Panel.h"
#define PAUSEPANEL "Pause##"

enum class GameState {
	PLAYING,
	PAUSED,
	RUN_ONCE
};
class PausePanel : public Panel
{
public:
	PausePanel();

	void Draw(int windowFlags) override;

private:
	void Play();
	void Pause();
	GameState mState = GameState::PLAYING;
};