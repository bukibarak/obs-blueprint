#pragma once
#include <cmath>
#include "Core/obs-blueprint-node.h"

class NodeFloatToInt : public OBSBlueprintNode {
public:
	NodeFloatToInt() : OBSBlueprintNode(obs_module_text("NodeFloatToInt"))
	{
		floatPin = createInputPin(FLOAT_PIN, 0.0f, "float");
		intPin = createOutputPin(INT_PIN, 0, "int");

		graphicsOptions.showName = false;
	}

	void execute(float deltaSeconds) override
	{
		intPin->setValue(static_cast<int32_t>(std::round(floatPin->getValue<float>())));
		haveExecutedThisCycle = true;
	}

	static OBSBlueprintNode* CreateDefault() { return new NodeFloatToInt(); }
	static std::vector<PinType> InputPins() { return {FLOAT_PIN}; }
	static std::vector<PinType> OutputPins() { return {INT_PIN}; }

private:
	OBSBlueprintInputPin* floatPin;
	OBSBlueprintOutputPin* intPin;
};
