#include "AudioTester.h"

AudioTester::AudioTester(GameObject* owner) : Script(owner)
{
	//**
	// 1 = BGM::Background music Level1 (play/release)
	// 2 = BGM::Background music Level1 - another instance (play/release)
	// 3 = BGM::Background music mainmenu
	// 4 = SFX::Gunfire audio (play/release)
	// AD = Oneshot SFX::Footstep simulaion (play)
	// L/R Arrow = change BGM 1 audio parameter
}

AudioTester::~AudioTester()
{
}

void AudioTester::Start()
{
}

void AudioTester::Update()
{
}
