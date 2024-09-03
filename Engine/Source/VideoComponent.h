#pragma once
#include "Component.h"

class CanvasComponent;
class Transform2DComponent;

struct AVFormatContext;
struct AVPacket;
struct AVFrame;
struct AVStream;
struct AVCodecContext;
struct AVCodecParameters;
struct SwsContext;

class ENGINE_API VideoComponent : public Component
{
public:
	explicit VideoComponent(GameObject* owner);
	VideoComponent(const VideoComponent& original) = delete;
	VideoComponent(const VideoComponent& original, GameObject* owner);
	~VideoComponent();

	void Update() override;
	void Draw();
	Component* Clone(GameObject* owner) const override;

	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

	void Play() { mIsPlaying = true; }
	void Pause() { mIsPlaying = false; }
	void Stop();

	bool GetLoop() { return mLoop; }
	void SetLoop(bool loop) { mLoop = loop; }

protected:

	void Reset() override;

private:

	void InitVBO();
	void InitVAO();


	void OpenVideo();
	void CloseVideo();
	void RestartVideo();

	void ReadNextFrame();
	int DecodePacket(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame);
	void Enable() override;
	void Disable() override;


	AVFormatContext* pFormatContext = nullptr;
	AVPacket* pPacket = nullptr;
	AVFrame* pFrame = nullptr;
	AVFrame* pFrameRGB = nullptr;
	AVCodecContext* pCodecContext = nullptr;
	AVCodecParameters* pCodecParameters = nullptr;
	AVStream* pVideoStream = nullptr;
	SwsContext* scalerCtx = nullptr;

	int mVideoStreamIndex = -1;


	float mElapsedTime = 0.0f;
	double mFrameTime = 0.0;

	unsigned int mTextureID = 0;
	unsigned int mQuadVBO = 0;
	unsigned int mQuadVAO = 0;

	CanvasComponent* mCanvas = nullptr;

	unsigned int mUIProgramID = 0;

	bool mIsPlaying = false;
	bool mLoop = false;
	
	//uint8_t* frameData = nullptr;

};

