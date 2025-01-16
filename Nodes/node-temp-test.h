#pragma once
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include "ffmpeg-err2str.h"

#include "Core/obs-blueprint-node.h"

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
		int status;
		if (status = avformat_open_input(&ctx, pathFile, nullptr, nullptr); status < 0) {
			GError("Failed to open '%s': %s", pathFile, cpp_err2str(status));
			return;
		}
		if (status = avformat_find_stream_info(ctx, nullptr); status < 0) {
			GError("Failed to find stream info for '%s' : %s", pathFile, cpp_err2str(status));
			avformat_close_input(&ctx);
			return;
		}
		const AVCodec* decoder = nullptr;
		if (status = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0); status < 0) {
			GError("Failed to find stream/decoder for '%s' : %s", pathFile, cpp_err2str(status));
			avformat_close_input(&ctx);
			return;
		}

		const char* test = cpp_err2str(status);

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
};
