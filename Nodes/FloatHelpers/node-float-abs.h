#pragma once
#include "Core/obs-blueprint-node.h"

class NodeFloatAbs : public OBSBlueprintNode {
public:
	NodeFloatAbs()
	{
		displayName = obs_module_text("NodeFloatAbs");

		inputPin = createInputPin(FLOAT_PIN, 0.0f);
		outputPin = createOutputPin(FLOAT_PIN, 0.0f);
	}

	void execute(float deltaSeconds) override
	{
		outputPin->setValue(std::abs(*inputPin->getValuePtr<float>()));
	}

	OBSBlueprintInputPin* getInputPin() const { return inputPin; }
	OBSBlueprintOutputPin* getOutputPin() const { return outputPin; }

private:
	OBSBlueprintInputPin* inputPin;
	OBSBlueprintOutputPin* outputPin;
};
