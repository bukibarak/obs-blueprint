#pragma once
#include "Core/obs-blueprint-node.h"

class NodeFloatAbs : public OBSBlueprintNode {
public:
	NodeFloatAbs() : OBSBlueprintNode(obs_module_text("NodeFloatAbs"))
	{
		inputPin = createInputPin(FLOAT_PIN, 0.0f, "value");
		outputPin = createOutputPin(FLOAT_PIN, 0.0f, "result");
	}

	void execute(float deltaSeconds) override
	{
		outputPin->setValue(std::abs(*inputPin->getValuePtr<float>()));
		haveExecutedThisCycle = true;
	}

	OBSBlueprintInputPin* getInputPin() const { return inputPin; }
	OBSBlueprintOutputPin* getOutputPin() const { return outputPin; }

private:
	OBSBlueprintInputPin* inputPin;
	OBSBlueprintOutputPin* outputPin;
};
