#include "AudioManager.h"
#include "ScriptComponent.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "AudioSourceComponentNew.h"

AudioManager* AudioManager::mInstance = nullptr;

AudioManager* AudioManager::GetInstance()
{
    if (mInstance == nullptr) {
        LOG("AudioManager instance has not been initialized.");
        throw std::runtime_error("GameManager instance has not been initialized.");
    }
    return mInstance;
}

void AudioManager::Start()
{
}

void AudioManager::Update()
{
}

AudioManager::AudioManager(GameObject* owner) : Script(owner) 
{
    mAudioSources = reinterpret_cast<AudioSourceComponentNew*>(mGameObject->GetComponent(ComponentType::AUDIOSOURCENEW));
}

AudioManager::~AudioManager()
{
	mInstance = nullptr;
}

int AudioManager::Play(std::string audioName, float3 audioPosition, std::unordered_map<std::string, int> parameters)
{

    return mBackgroundAudioCounter++;
}

void AudioManager::PlayOneShot(std::string audioName, float3 audioPosition, std::unordered_map<std::string, int> parameters)
{
}




