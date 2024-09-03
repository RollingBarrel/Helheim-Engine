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
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

VideoComponent::VideoComponent(GameObject* owner) : Component(owner, ComponentType::VIDEO)
{
	mUIProgramID = App->GetOpenGL()->GetUIImageProgram();
	InitVBO();
	InitVAO();
	OpenVideo();
	ReadNextFrame();
	GameObject* currentObject = owner;
	while (currentObject)
	{
		if (currentObject->GetComponent(ComponentType::CANVAS)) break;
		currentObject = currentObject->GetParent();
	}
	if (currentObject) mCanvas = (CanvasComponent*)(currentObject->GetComponent(ComponentType::CANVAS));

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

VideoComponent::VideoComponent(const VideoComponent& original, GameObject* owner) : Component(owner, ComponentType::VIDEO)
{
}

VideoComponent::~VideoComponent()
{
	CloseVideo();
}

void VideoComponent::Update()
{
	Draw();
}

void VideoComponent::Draw()
{
	if (mCanvas && mUIProgramID != 0 && mTextureID != 0)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Video Component");
		//glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glActiveTexture(GL_TEXTURE0);

		int frameWidth = pCodecParameters->width;
		int frameHeight = pCodecParameters->height;

		//int frameWidth = pFrame->width;
		//int frameHeight = pFrame->height;

		// allocate memory and set texture data
		glBindTexture(GL_TEXTURE_2D, mTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);

		glUseProgram(mUIProgramID);
		glEnable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		float4x4 proj = float4x4::identity;
		float4x4 model = float4x4::identity;
		float4x4 view = float4x4::identity;

		Transform2DComponent* transform2d = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		if (transform2d)
		{
			model = transform2d->GetGlobalMatrix();
			float2 canvasSize = mCanvas->GetSize();
			model = float4x4::Scale(1.0f / canvasSize.x * 2.0f, 1.0f / canvasSize.y * 2.0f, 0.0f) * model;

		}
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
}

Component* VideoComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void VideoComponent::Save(JsonObject& obj) const
{
}

void VideoComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
}

void VideoComponent::Reset()
{
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

	if (mQuadVBO == 0)
	{
		glGenBuffers(1, &mQuadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);
	}
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

void VideoComponent::OpenVideo()
{
	//if (argc < 2) {
	//    printf("You need to specify a media file.\n");
	//    return;
	//}
	//const char* videoFilePath = "C:\\Users\\carlo\\Documents\\GitHub\\Assigment2\\Engine\\Game\\Assets\\Video\\una_rosa.mp4";
	const char* videoFilePath = "C:\\Users\\carlo\\Documents\\GitHub\\Assigment2\\Engine\\Game\\Assets\\Video\\bunny.mp4";

	LOG("initializing all the containers, codecs and protocols.");

	// AVFormatContext holds the header information from the format (Container)
	// Allocating memory for this component
	// http://ffmpeg.org/doxygen/trunk/structAVFormatContext.html
	pFormatContext = avformat_alloc_context();
	if (!pFormatContext)
	{
		assert(false && "ERROR could not allocate memory for Format Context");
		return;
	}

	LOG("opening the input file (%s) and loading format (container) header", videoFilePath);
	// Open the file and read its header. The codecs are not opened.
	// The function arguments are:
	// AVFormatContext (the component we allocated memory for),
	// url (filename),
	// AVInputFormat (if you pass NULL it'll do the auto detect)
	// and AVDictionary (which are options to the demuxer)
	// http://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga31d601155e9035d5b0e7efedc894ee49
	if (avformat_open_input(&pFormatContext, videoFilePath, NULL, NULL) != 0)
	{

		assert(false && "ERROR could not open the file");
		return;
	}

	// now we have access to some information about our file
	// since we read its header we can say what format (container) it's
	// and some other information related to the format itself.
	LOG("format %s, duration %lld us, bit_rate %lld", pFormatContext->iformat->name, pFormatContext->duration, pFormatContext->bit_rate);
	LOG("finding stream info from format");
	// read Packets from the Format to get stream information
	// this function populates pFormatContext->streams
	// (of size equals to pFormatContext->nb_streams)
	// the arguments are:
	// the AVFormatContext
	// and options contains options for codec corresponding to i-th stream.
	// On return each dictionary will be filled with options that were not found.
	// https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#gad42172e27cddafb81096939783b157bb
	if (avformat_find_stream_info(pFormatContext, NULL) < 0)
	{
		LOG("ERROR could not get the stream info");
		return;
	}

	// the component that knows how to enCOde and DECode the stream
	// it's the codec (audio or video)
	// http://ffmpeg.org/doxygen/trunk/structAVCodec.html
	const AVCodec* pCodec = NULL;
	// this component describes the properties of a codec used by the stream i
	// https://ffmpeg.org/doxygen/trunk/structAVCodecParameters.html
	//AVCodecParameters* pCodecParameters = NULL;

	// loop though all the streams and print its main information
	for (int i = 0; i < pFormatContext->nb_streams; i++)
	{
		AVCodecParameters* pLocalCodecParameters = NULL;
		pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
		LOG("AVStream->time_base before open coded %d/%d", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
		LOG("AVStream->r_frame_rate before open coded %d/%d", pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
		LOG("AVStream->start_time %" PRId64, pFormatContext->streams[i]->start_time);
		LOG("AVStream->duration %" PRId64, pFormatContext->streams[i]->duration);

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
				pCodecParameters = pLocalCodecParameters;
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
		LOG("File %s does not contain a video stream!", videoFilePath);
		return;
	}

	// https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
	pCodecContext = avcodec_alloc_context3(pCodec);
	if (!pCodecContext)
	{
		LOG("failed to allocated memory for AVCodecContext");
		return;
	}

	// Fill the codec context based on the values from the supplied codec parameters
	// https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
	if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
	{
		LOG("failed to copy codec params to codec context");
		return;
	}

	// Initialize the AVCodecContext to use the given AVCodec.
	// https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
	if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
	{
		LOG("failed to open codec through avcodec_open2");
		return;
	}

	// https://ffmpeg.org/doxygen/trunk/structAVFrame.html
	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	if (!pFrame)
	{
		LOG("failed to allocate memory for AVFrame");
		return;
	}
	// https://ffmpeg.org/doxygen/trunk/structAVPacket.html
	pPacket = av_packet_alloc();
	if (!pPacket)
	{
		LOG("failed to allocate memory for AVPacket");
		return;
	}

	scalerCtx = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt, pCodecContext->width, pCodecContext->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

}

void VideoComponent::CloseVideo()
{
	LOG("releasing all video resources");

	avformat_close_input(&pFormatContext);
	av_packet_free(&pPacket);
	av_frame_free(&pFrame);
	av_frame_free(&pFrameRGB);
	avcodec_free_context(&pCodecContext);
	sws_freeContext(scalerCtx);

	pFormatContext = nullptr;
	pPacket = nullptr;
	pFrame = nullptr;
	pCodecContext = nullptr;

	mVideoStreamIndex = -1;

}

void VideoComponent::ReadNextFrame()
{
	int response = 0;

	while (av_read_frame(pFormatContext, pPacket) >= 0)
	{
		if (pPacket->stream_index == mVideoStreamIndex)
		{
			LOG("AVPacket->pts %" PRId64, pPacket->pts);
			response = DecodePacket(pPacket, pCodecContext, pFrame);
			if (response < 0) break;
		}
		av_packet_unref(pPacket);
	}	
	
	//frameData = new uint8_t[pCodecContext->width * pCodecContext->height * 4];
	//uint8_t* dest[8] = { frameData, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	//int linSize[8] = { pCodecContext->width * 4, 0, 0, 0, 0, 0, 0, 0 };
	
	
	//int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height, 1);
	//uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	//
	//av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height, 1);
	av_image_alloc(pFrameRGB->data, pFrameRGB->linesize, pCodecContext->width, pCodecContext->height, AV_PIX_FMT_RGB24, 1);

	sws_scale(scalerCtx, pFrame->data, pFrame->linesize, 0, pCodecContext->height, pFrameRGB->data, pFrameRGB->linesize);
	//sws_scale(scalerCtx, pFrame->data, pFrame->linesize, 0, pCodecContext->height, pFrameRGB->data, pFrameRGB->linesize);
	//sws_scale(scalerCtx, pFrame->data, pFrame->linesize, 0, pCodecContext->height, dest, linSize);

	//av_free(buffer);

}

int VideoComponent::DecodePacket(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame)
{ 

	int response = avcodec_send_packet(pCodecContext, pPacket);
	if (response < 0)
	{
		LOG("Error while sending a packet to the decoder: %s", "error"); // av_err2str(response)
		return response;
	}

	while (response >= 0)
	{
		response = avcodec_receive_frame(pCodecContext, pFrame);

		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
		{
			break;
		}
		else if (response < 0)
		{
			LOG("Error while receiving a frame from the decoder: %s", "Error"); // av_err2str(response)
			return response;
		}

		if (response >= 0)
		{
			LOG(
				"Salchichon: Frame %d (type=%c, size=%d bytes, format=%d) pts %d key_frame %d [DTS %d]",
				pCodecContext->frame_num,
				av_get_picture_type_char(pFrame->pict_type),
				pFrame->pkt_size,
				pFrame->format,
				pFrame->pts,
				pFrame->key_frame
			);

			return -1;
		}
	}

	return 0;
}

void VideoComponent::Enable()
{
}

void VideoComponent::Disable()
{
}
