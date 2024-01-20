#pragma once
#include "Panel.h"
#define PAUSEPANEL "Pause##"

enum class GameState {
	PLAYING,
	PAUSED,
	RUN_ONCE,
	STOP
};
class PausePanel : public Panel
{
public:
	PausePanel();

	void Draw(int windowFlags) override;

private:
	void Play();
	void Pause();
	void Stop();
	GameState mState = GameState::PLAYING;
};