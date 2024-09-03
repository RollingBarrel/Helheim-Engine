#pragma once
#include "Component.h"

class CanvasComponent;
class Transform2DComponent;

struct AVFormatContext;
struct AVPacket;
struct AVFrame;
struct AVCodecContext;
struct AVCodecParameters;
struct SwsContext;

class VideoComponent : public Component
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

protected:

	void Reset() override;

private:

	void InitVBO();
	void InitVAO();


	void OpenVideo();
	void CloseVideo();

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
	SwsContext* scalerCtx = nullptr;

	int mVideoStreamIndex = -1;


	unsigned int mTextureID = 0;
	unsigned int mQuadVBO = 0;
	unsigned int mQuadVAO = 0;

	CanvasComponent* mCanvas = nullptr;

	unsigned int mUIProgramID = 0;

	
	uint8_t* frameData = nullptr;

};

