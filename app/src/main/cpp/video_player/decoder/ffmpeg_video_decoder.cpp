#include "ffmpeg_video_decoder.h"

#define LOG_TAG "FFMPEGVideoDecoder"

FFMPEGVideoDecoder::FFMPEGVideoDecoder() {
}

FFMPEGVideoDecoder::FFMPEGVideoDecoder(JavaVM *g_jvm, jobject obj)
					: VideoDecoder(g_jvm, obj) {
}

FFMPEGVideoDecoder::~FFMPEGVideoDecoder() {
}

TextureFrameUploader* FFMPEGVideoDecoder::createTextureFrameUploader() {
	TextureFrameUploader* textureFrameUploader = new YUVTextureFrameUploader();
	return textureFrameUploader;
}

float FFMPEGVideoDecoder::updateTexImage(TextureFrame* textureFrame) {
	float position = -1;
	//将AVFrame 封装成 VideoFrame，即将数据分开成YUV分量
	VideoFrame *yuvFrame = handleVideoFrame();
	if (yuvFrame) {
		((YUVTextureFrame*) textureFrame)->setVideoFrame(yuvFrame);
		//创建3个纹理对象，并且将 VideoFrame 的 YUV数据分别绑定到3个纹理对象中，方便后续离线渲染
		textureFrame->updateTexImage();
		position = yuvFrame->position;
		delete yuvFrame;
	}
	return position;
}

bool FFMPEGVideoDecoder::decodeVideoFrame(AVPacket packet, int* decodeVideoErrorState) {
	int pktSize = packet.size;

	int gotframe = 0;

	while (pktSize > 0) {
		int len = avcodec_decode_video2(videoCodecCtx, videoFrame, &gotframe, &packet);
		if (len < 0) {
			LOGI("decode video error, skip packet");
			*decodeVideoErrorState = 1;
			break;
		}
		if (gotframe) {
			if (videoFrame->interlaced_frame) {
				avpicture_deinterlace((AVPicture*) videoFrame, (AVPicture*) videoFrame, videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height);
			}
			this->uploadTexture();
		}
		if (0 == len) {
			break;
		}
		pktSize -= len;
	}

	return (bool)gotframe;
}

void FFMPEGVideoDecoder::flushVideoFrames(AVPacket packet, int* decodeVideoErrorState) {
	if (videoCodecCtx->codec->capabilities & CODEC_CAP_DELAY) {
		packet.data = 0;
		packet.size = 0;
		av_init_packet(&packet);
		int gotframe = 0;
		int len = avcodec_decode_video2(videoCodecCtx, videoFrame, &gotframe, &packet);

		LOGI("flush video %d", gotframe);

		if (len < 0) {
			LOGI("decode video error, skip packet");
			*decodeVideoErrorState = 1;
		}
		if (gotframe) {
			if (videoFrame->interlaced_frame) {
				avpicture_deinterlace((AVPicture*) videoFrame, (AVPicture*) videoFrame, videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height);
			}
			this->uploadTexture();
		} else {
			LOGI("output EOF");
			isVideoOutputEOF = true;
		}
	}
}
