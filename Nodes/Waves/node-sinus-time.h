#pragma once
#include "Core/obs-blueprint-node.h"

class NodeSinusTime : public OBSBlueprintNode {
public:
	NodeSinusTime(const float& defaultSpeed = 1.0f, const float& defaultAmplitude = 1.0f, const float& defaultShift = 0.0f);

	void execute(float deltaSeconds) override;
	void onGraphLinked() override;
	void onGraphBeginTick(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeSinusTime(); }
	static std::vector<PinType> InputPins() { return {FLOAT_PIN, FLOAT_PIN, FLOAT_PIN}; }
	static std::vector<PinType> OutputPins() { return {FLOAT_PIN}; }

private:
	OBSBlueprintInputPin* speedPin;
	OBSBlueprintInputPin* amplitudePin;
	OBSBlueprintInputPin* shiftPin;
	OBSBlueprintOutputPin* resultPin;

	float totalTime;
};
