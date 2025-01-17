#pragma once
#include "Structs/obs-frame.h"
#include "ffmpeg-err2str.h"
#include "Core/obs-blueprint-node.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}


class NodeTempTest : public OBSBlueprintNode {
public:
	NodeTempTest() : OBSBlueprintNode("TEMP TEST")
	{
		inputPin = createInputPin(VIDEO_PIN, OBSFrame(), "IN");
		outputPin = createOutputPin(VIDEO_PIN, OBSFrame(), "OUT");

		graphicsOptions.isCompactNode = true;
		graphicsOptions.showName = false;

		const char* const pathFile = "C:/temp/test/bgra.png";
		AVFormatContext* ctx = nullptr;
		AVCodecContext* codec = nullptr;
		AVPixelFormat format;
		AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
		int status;

		GDebug("Available device types:");
		while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
			GDebug("  - %s", av_hwdevice_get_type_name(type));
		}

		if (status = avformat_open_input(&ctx, pathFile, nullptr, nullptr); status < 0) {
			GError("Failed to open '%s': %s", pathFile, cpp_err2str(status));
			ffmpeg_free(ctx, codec);
			return;
		}
		if (status = avformat_find_stream_info(ctx, nullptr); status < 0) {
			GError("Failed to find stream info for '%s' : %s", pathFile, cpp_err2str(status));
			ffmpeg_free(ctx, codec);
			return;
		}
		const AVCodec* decoder = nullptr;
		if (status = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0); status < 0) {
			GError("Failed to find stream/decoder for '%s' : %s", pathFile, cpp_err2str(status));
			ffmpeg_free(ctx, codec);
			return;
		}

		type = AV_HWDEVICE_TYPE_D3D11VA;
		for (int i = 0;;i++) {
			const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i); // Use GPU to decode if available (I think)
			if (!config) {
				GError("Decoder '%s' does not support device %s for '%s'",
						decoder->name, av_hwdevice_get_type_name(type), pathFile);
				ffmpeg_free(ctx, codec);
				return;
			}
			if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && config->device_type == type) {
				format = config->pix_fmt;
				break;
			}
		}
		codec = avcodec_alloc_context3(decoder);

		// Well this is too complicated for now --> TODO: see if we can use directly FFMPEG to ALWAYS convert all inputs
		// like images, gif, videos, dshow (camera), win/screen capture, etc. to BGRA
		// BUT using the GPU if available, and the CPU if not (this is the real tricky part)
		// --> best would be to leave data on GPU and create UMat from GPU data (dunno if it's possible) in order
		// to reduce to maximum CPU <-> GPU data transfert!
		ffmpeg_free(ctx, codec);
	}

	void execute(float deltaSeconds) override
	{
		if(inputPin->isConnected()) {
			outputPin->setValue(inputPin->getValue<OBSFrame>());
			haveExecutedThisCycle = true;
		}
		else {
			outputPin->setValue(OBSFrame());
		}
	}

private:

	OBSBlueprintInputPin* inputPin;
	OBSBlueprintOutputPin* outputPin;

	static void ffmpeg_free(AVFormatContext* ctx, AVCodecContext* codec)
	{
		avcodec_free_context(&codec);
		avformat_close_input(&ctx);
	}
};
