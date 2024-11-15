#pragma once

#include "Core/obs-blueprint-node.h"

class NodeIntToFloat : public OBSBlueprintNode {
public:
	NodeIntToFloat() : OBSBlueprintNode(obs_module_text("NodeIntToFloat"))
	{
		intPin = createInputPin(INT_PIN, 0, "int");
		floatPin = createOutputPin(FLOAT_PIN, 0.0f, "float");

		graphicsOptions.showName = false;
	}

	void execute(float deltaSeconds) override
	{
		floatPin->setValue(static_cast<float>(intPin->getValue<int32_t>()));
		haveExecutedThisCycle = true;
	}

	static OBSBlueprintNode* CreateDefault() { return new NodeIntToFloat(); }
	static std::vector<PinType> InputPins() { return {INT_PIN}; }
	static std::vector<PinType> OutputPins() { return {FLOAT_PIN}; }

private:
	OBSBlueprintInputPin* intPin;
	OBSBlueprintOutputPin* floatPin;
};
