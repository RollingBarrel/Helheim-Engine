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

	void OpenVideo(const char* filePath);
	void Play() { mIsPlaying = true; }
	void Pause() { mIsPlaying = false; }
	void Stop();

	const char* GetName() { return mName.c_str(); }

	bool GetLoop() { return mLoop; }
	void SetLoop(bool loop) { mLoop = loop; }

protected:

	void Reset() override;

private:

	void InitVBO();
	void InitVAO();


	void CloseVideo();
	void RestartVideo();

	void ReadNextFrame();
	int DecodePacket(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame);
	void Enable() override;
	void Disable() override;


	AVFormatContext* mFormatContext = nullptr;
	AVPacket* mPacket = nullptr;
	AVFrame* mFrame = nullptr;
	AVFrame* mFrameRGB = nullptr;
	AVCodecContext* mCodecContext = nullptr;
	AVCodecParameters* mCodecParameters = nullptr;
	AVStream* mVideoStream = nullptr;
	SwsContext* mScalerCtx = nullptr;

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
	
	std::string mName;

	//uint8_t* frameData = nullptr;

};

