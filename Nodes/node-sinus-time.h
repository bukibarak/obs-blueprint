#pragma once
#include "Core/obs-blueprint-node.h"

class NodeSinusTime : public OBSBlueprintNode {
public:
	NodeSinusTime(const float& defaultSpeed = 1.0f, const float& defaultAmplitude = 1.0f);

	void execute(float deltaSeconds) override;

	void onGraphBeginTick(float deltaSeconds) override;

	OBSBlueprintInputPin* getSpeedInputPin() const { return speedPin; }
	OBSBlueprintInputPin* getAmplitudeInputPin() const { return amplitudePin; }
	OBSBlueprintOutputPin* getResultPin() const { return resultPin; }

private:
	OBSBlueprintInputPin* speedPin;
	OBSBlueprintInputPin* amplitudePin;
	OBSBlueprintOutputPin* resultPin;

	float totalTime = 0.0f;
};
