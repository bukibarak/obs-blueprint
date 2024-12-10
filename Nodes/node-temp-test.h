#pragma once
#include "Core/obs-blueprint-node.h"

class NodeTempTest : public OBSBlueprintNode {
public:
	NodeTempTest() : OBSBlueprintNode("TEMP TEST")
	{
		inputPin = createInputPin(VIDEO_PIN, OBSFrame(), "IN");
		outputPin = createOutputPin(VIDEO_PIN, OBSFrame(), "OUT");

		graphicsOptions.isCompactNode = true;
		graphicsOptions.showName = false;
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
