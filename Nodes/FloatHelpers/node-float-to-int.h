#pragma once
#include <cmath>
#include "Core/obs-blueprint-node.h"

class NodeFloatToInt : public OBSBlueprintNode {
public:
	NodeFloatToInt()
	{
		floatPin = createInputPin(FLOAT_PIN, 0.0f);
		intPin = createOutputPin(INT_PIN, 0);
	}

	void execute(float deltaSeconds) override
	{
		float rawValue = *floatPin->getValuePtr<float>();
		int32_t value = static_cast<int32_t>(std::round(rawValue));
		intPin->setValue(value);
		haveExecutedThisCycle = true;
	}

	OBSBlueprintInputPin* getFloatInputPin() const { return floatPin; }
	OBSBlueprintOutputPin* getIntOutputPin() const { return intPin; }

private:
	OBSBlueprintInputPin* floatPin;
	OBSBlueprintOutputPin* intPin;
};
