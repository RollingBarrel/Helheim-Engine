#include "VideoComponent.h"

#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"

#include "GameObject.h"
#include "CanvasComponent.h"
#include "Transform2DComponent.h"

#include "float2.h"
#include "float3.h"
#include "float4x4.h"
#include "Math/TransformOps.h"

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/imgutils.h"
	#include "libswscale/swscale.h"
}

VideoComponent::VideoComponent(GameObject* owner) : Component(owner, ComponentType::VIDEO)
{
	Init();
	OpenVideo("./Assets/Videos/una_rosa.mp4");
}

VideoComponent::VideoComponent(const VideoComponent& original, GameObject* owner) : Component(owner, ComponentType::VIDEO),
mName(original.mName),mFilePath(original.mFilePath), mCanvas(mCanvas), mLoop(original.mLoop)
{
	Init();
	OpenVideo(mFilePath.c_str());
}

VideoComponent::~VideoComponent()
{
	CloseVideo();
}

Component* VideoComponent::Clone(GameObject* owner) const
{
	return new VideoComponent(*this, owner);
}

void VideoComponent::Update()
{
	if (mIsPlaying)
	{
		mElapsedTime += App->GetDt();
		while (mElapsedTime > mFrameTime)
		{
			ReadNextFrame();
		}
	}
}

void VideoComponent::Draw()
{
	if (mCanvas && mTransform2D && mUIProgramID != 0 && mTextureID != 0)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Video Component");
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, mTextureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mCodecParameters->width, mCodecParameters->height, GL_RGB, GL_UNSIGNED_BYTE, mFrameRGB->data[0]);

		glUseProgram(mUIProgramID);
		glEnable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		float2 canvasSize = mCanvas->GetSize();
		
		float4x4 model = float4x4::Scale(1.0f / canvasSize.x * 2.0f, 1.0f / canvasSize.y * 2.0f, 0.0f) * mTransform2D->GetGlobalMatrix();
		float4x4 proj = float4x4::identity;
		float4x4 view = float4x4::identity;
		
		glEnable(GL_CULL_FACE);

		glBindVertexArray(mQuadVAO);
		glUniform4fv(glGetUniformLocation(mUIProgramID, "inputColor"), 1, float4(1.0f, 1.0f, 1.0f, 1.0f).ptr());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTextureID);
		glUniform1i(glGetUniformLocation(mUIProgramID, "Texture"), 0);

		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glPopDebugGroup();
	}
	else if (!mTransform2D)
	{
		mTransform2D = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
	}
}

void VideoComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
	//obj.AddString("Video Name", mName.c_str());
	obj.AddString("Video Path", mFilePath.c_str());
	obj.AddBool("Loop", mLoop);
}

void VideoComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
	//if (data.HasMember("Video Name")) mName = data.GetString("Video Name");
	if (data.HasMember("Video Path")) mFilePath = data.GetString("Video Path");
	if (data.HasMember("Loop")) mLoop = data.GetBool("Loop");

	OpenVideo(mFilePath.c_str());
}

void VideoComponent::Stop()
{
	Pause();
	RestartVideo();
	ReadNextFrame();
}

void VideoComponent::Reset()
{
	Stop();
}

void VideoComponent::Init()
{
	GameObject* currentObject = mOwner;
	while (currentObject)
	{
		if (currentObject->GetComponent(ComponentType::CANVAS)) break;
		currentObject = currentObject->GetParent();
	}
	if (currentObject) mCanvas = (CanvasComponent*)(currentObject->GetComponent(ComponentType::CANVAS));
	mTransform2D = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));

	mUIProgramID = App->GetOpenGL()->GetUIImageProgram();
	InitVBO();
	InitVAO();

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void VideoComponent::InitVBO()
{
	float vertices[] = {
		// texture coordinates
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		-0.5f, -0.5f,  0.0f,  1.0f,   // bottom-left vertex
		0.5f, -0.5f,  1.0f,  1.0f,   // bottom-right vertex
		0.5f,  0.5f,  1.0f,  0.0f,   // top-right vertex
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		0.5f, -0.5f,  1.0f,  1.0f    // bottom-right vertex
	};

	glGenBuffers(1, &mQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
}

void VideoComponent::InitVAO()
{
	glGenVertexArrays(1, &mQuadVAO);
	glBindVertexArray(mQuadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

void VideoComponent::OpenVideo(const char* filePath)
{
	CloseVideo();
	const char* name = strrchr(filePath, '/');
	mName = ++name;
	mFilePath = filePath;

	// Allocating memory for AVFormatContext
	mFormatContext = avformat_alloc_context();
	if (!mFormatContext)
	{
		assert(false && "ERROR could not allocate memory for Format Context");
		return;
	}

	// Open the file and read its header. The codecs are not opened.
	if (avformat_open_input(&mFormatContext, filePath, NULL, NULL) != 0)
	{
		assert(false && "ERROR could not open the file");
		return;
	}

	if (avformat_find_stream_info(mFormatContext, NULL) < 0)
	{
		LOG("ERROR could not get the stream info");
		return;
	}

	// the component that knows how to enCOde and DECode the stream
	// it's the codec (audio or video)
	const AVCodec* pCodec = NULL;

	// loop though all the streams and print its main information
	for (unsigned int i = 0; i < mFormatContext->nb_streams; i++)
	{
		AVCodecParameters* pLocalCodecParameters = NULL;
		pLocalCodecParameters = mFormatContext->streams[i]->codecpar;
		LOG("AVStream->time_base before open coded %d/%d", mFormatContext->streams[i]->time_base.num, mFormatContext->streams[i]->time_base.den);
		LOG("AVStream->r_frame_rate before open coded %d/%d", mFormatContext->streams[i]->r_frame_rate.num, mFormatContext->streams[i]->r_frame_rate.den);
		LOG("AVStream->start_time %" PRId64, mFormatContext->streams[i]->start_time);
		LOG("AVStream->duration %" PRId64, mFormatContext->streams[i]->duration);

		LOG("finding the proper decoder (CODEC)");

		const AVCodec* pLocalCodec = NULL;

		// finds the registered decoder for a codec ID
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
		pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

		if (pLocalCodec == NULL)
		{
			LOG("ERROR unsupported codec!");
			// In this example if the codec is not found we just skip it
			continue;
		}

		// when the stream is a video we store its index, codec parameters and codec
		if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if (mVideoStreamIndex == -1)
			{
				mVideoStreamIndex = i;
				pCodec = pLocalCodec;
				mCodecParameters = pLocalCodecParameters;
			}

			LOG("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
		}
		else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			//LOG("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);

		}

		// print its name, id and bitrate
		LOG("\tCodec %s ID %d bit_rate %lld", pLocalCodec->name, pLocalCodec->id, pLocalCodecParameters->bit_rate);
	}

	if (mVideoStreamIndex == -1)
	{
		LOG("File %s does not contain a video stream!", filePath);
		return;
	}

	mCodecContext = avcodec_alloc_context3(pCodec);
	if (!mCodecContext)
	{
		LOG("failed to allocated memory for AVCodecContext");
		return;
	}

	// Fill the codec context based on the values from the supplied codec parameters
	if (avcodec_parameters_to_context(mCodecContext, mCodecParameters) < 0)
	{
		LOG("failed to copy codec params to codec context");
		return;
	}

	// Initialize the AVCodecContext to use the given AVCodec.
	if (avcodec_open2(mCodecContext, pCodec, NULL) < 0)
	{
		LOG("failed to open codec through avcodec_open2");
		return;
	}

	mFrame = av_frame_alloc();
	if (!mFrame)
	{
		LOG("failed to allocate memory for AVFrame");
		return;
	}

	mFrameRGB = av_frame_alloc();
	if (!mFrame)
	{
		LOG("failed to allocate memory for AVFrame");
		return;
	}

	mPacket = av_packet_alloc();
	if (!mPacket)
	{
		LOG("failed to allocate memory for AVPacket");
		return;
	}

	mScalerCtx = sws_getContext(mCodecContext->width, mCodecContext->height, mCodecContext->pix_fmt, mCodecContext->width, mCodecContext->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
	mVideoStream = mFormatContext->streams[mVideoStreamIndex];
	av_image_alloc(mFrameRGB->data, mFrameRGB->linesize, mCodecContext->width, mCodecContext->height, AV_PIX_FMT_RGB24, 1);
	ReadNextFrame();

	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mCodecParameters->width, mCodecParameters->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}

void VideoComponent::CloseVideo()
{
	//LOG("releasing all video resources");

	avformat_close_input(&mFormatContext);
	av_packet_free(&mPacket);
	av_frame_free(&mFrame);
	av_frame_free(&mFrameRGB);
	avcodec_free_context(&mCodecContext);
	sws_freeContext(mScalerCtx);
	//av_freep(pFrameRGB->data[0]);

	mFormatContext = nullptr;
	mPacket = nullptr;
	mFrame = nullptr;
	mCodecContext = nullptr;

	mVideoStreamIndex = -1;
}

void VideoComponent::RestartVideo()
{
	mElapsedTime = 0.0f;
	mFrameTime = 0.0;
	av_seek_frame(mFormatContext, mVideoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(mCodecContext);
}

void VideoComponent::ReadNextFrame()
{
	int response = 0;

	while ((response = av_read_frame(mFormatContext, mPacket)) >= 0)
	{
		if (mPacket->stream_index == mVideoStreamIndex)
		{
			LOG("AVPacket->pts %" PRId64, mPacket->pts);
			response = DecodePacket(mPacket, mCodecContext, mFrame);
			if (response < 0) break;
		}
		av_packet_unref(mPacket);
	}	
	
	if (response == AVERROR_EOF)
	{
		if (mLoop) RestartVideo();
		else Stop();
		return;
	}
	
    sws_scale(mScalerCtx, mFrame->data, mFrame->linesize, 0, mCodecContext->height, mFrameRGB->data, mFrameRGB->linesize);
	mFrameTime = mFrame->pts * av_q2d(mVideoStream->time_base);
}

int VideoComponent::DecodePacket(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame)
{ 
	int response = avcodec_send_packet(pCodecContext, pPacket);
	if (response < 0)
	{
		LOG("Error while sending a packet to the decoder: %s", "error"); // av_err2str(response)
		return response;
	}

	response = avcodec_receive_frame(pCodecContext, pFrame);
	if (response == AVERROR(EAGAIN))
	{
		return 0;
	}
	else if (response < 0)
	{
		LOG("Error while receiving a frame from the decoder: %s", "Error"); //av_err2str(response)
		return response;
	}
	if (response >= 0)
	{
		//LOG(
		//	"Frame %d (type=%c, size=%d bytes, format=%d) pts %d key_frame %d [DTS %d]",
		//	pCodecContext->frame_num,
		//	av_get_picture_type_char(pFrame->pict_type),
		//	pFrame->pkt_size,
		//	pFrame->format,
		//	pFrame->pts,
		//	pFrame->key_frame
		//);

		return -1;
	}

	return 0;
}

void VideoComponent::Disable()
{
	Pause();
}
