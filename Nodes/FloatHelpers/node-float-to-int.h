#pragma once
#include <cmath>
#include "Core/obs-blueprint-node.h"

class NodeFloatToInt : public OBSBlueprintNode {
public:
	NodeFloatToInt() : OBSBlueprintNode(obs_module_text("NodeFloatToInt"))
	{
		floatPin = createInputPin(FLOAT_PIN, 0.0f, "value (float)");
		intPin = createOutputPin(INT_PIN, 0, "result (int)");
	}

	void execute(float deltaSeconds) override
	{
		intPin->setValue(static_cast<int32_t>(std::round(*floatPin->getValuePtr<float>())));
		haveExecutedThisCycle = true;
	}

	OBSBlueprintInputPin* getInputPin() const { return floatPin; }
	OBSBlueprintOutputPin* getOutputPin() const { return intPin; }

private:
	OBSBlueprintInputPin* floatPin;
	OBSBlueprintOutputPin* intPin;
};
