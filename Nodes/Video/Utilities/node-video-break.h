#pragma once
#include "Core/obs-blueprint-node.h"

class NodeVideoBreak : public OBSBlueprintNode {
public:
	NodeVideoBreak() : OBSBlueprintNode(obs_module_text("NodeVideoBreak"))
	{
		videoPin = createInputPin(VIDEO_PIN, OBSFrame(), "video");

		widthPin = createOutputPin(INT_PIN, 0, "width");
		heightPin = createOutputPin(INT_PIN, 0, "height");

		graphicsOptions.showName = false;
	}

	void execute(float deltaSeconds) override
	{
		if(videoPin->isConnected()) {
			OBSFrame* frame = videoPin->getValuePtr<OBSFrame>();
			widthPin->setValue(frame->width());
			heightPin->setValue(frame->height());
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
