#pragma once
#include "Core/obs-blueprint-node.h"
#include "Structs/video-frame.h"

class NodeVideoBreak : public OBSBlueprintNode {
public:
	NodeVideoBreak() : OBSBlueprintNode(obs_module_text("NodeVideoBreak"))
	{
		videoPin = createInputPin(VIDEO_PIN, video_frame(), "video");

		widthPin = createOutputPin(INT_PIN, 0, "width");
		heightPin = createOutputPin(INT_PIN, 0, "height");

		graphicsOptions.showName = false;
	}

	void execute(float deltaSeconds) override
	{
		if(videoPin->isConnected()) {
			video_frame* frame = videoPin->getValuePtr<video_frame>();
			widthPin->setValue(static_cast<int32_t>(frame->width));
			heightPin->setValue(static_cast<int32_t>(frame->height));
			haveExecutedThisCycle = true;
		}
	}

	static OBSBlueprintNode* CreateDefault() { return new NodeVideoBreak(); }
	static std::vector<PinType> InputPins() { return {VIDEO_PIN, VIDEO_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:

	OBSBlueprintInputPin* videoPin;
	OBSBlueprintOutputPin* widthPin;
	OBSBlueprintOutputPin* heightPin;
};
