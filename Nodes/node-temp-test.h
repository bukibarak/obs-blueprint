#pragma once
#include "Core/obs-blueprint-node.h"
#include "Structs/video-frame.h"

class NodeTempTest : public OBSBlueprintNode {
public:
	NodeTempTest() : OBSBlueprintNode("TEMP TEST")
	{
		inputPin = createInputPin(VIDEO_PIN, video_frame(), "IN");
		outputPin = createOutputPin(VIDEO_PIN, video_frame(), "OUT");

		graphicsOptions.isCompactNode = true;
		graphicsOptions.showName = false;
	}

	void execute(float deltaSeconds) override
	{
		if(inputPin->isConnected()) {
			outputPin->setValue(inputPin->getValue<video_frame>());
			haveExecutedThisCycle = true;
		}
		else {
			outputPin->setValue(emptyFrame);
		}
	}

private:

	video_frame emptyFrame;
	OBSBlueprintInputPin* inputPin;
	OBSBlueprintOutputPin* outputPin;
};
