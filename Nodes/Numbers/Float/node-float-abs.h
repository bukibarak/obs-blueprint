#pragma once
#include "Core/obs-blueprint-node.h"

class NodeFloatAbs : public OBSBlueprintNode, public NodeFactory<NodeFloatAbs> {
public:
	NodeFloatAbs() : OBSBlueprintNode(obs_module_text("NodeFloatAbs"))
	{
		inputPin = createInputPin(FLOAT_PIN, 0.0f, "value");
		outputPin = createOutputPin(FLOAT_PIN, 0.0f, "result");

		graphicsOptions.isCompactNode = true;
	}

	void execute(float deltaSeconds) override
	{
		outputPin->setValue(std::abs(inputPin->getValue<float>()));
		haveExecutedThisCycle = true;
	}

// protected: TODO WHY PROTECTED NOT WORKING ??
	static OBSBlueprintNode* CreateDefault() { return new NodeFloatAbs(); }
	static std::vector<PinType> InputPins() { return {FLOAT_PIN}; }
	static std::vector<PinType> OutputPins() { return {FLOAT_PIN}; }

private:
	OBSBlueprintInputPin* inputPin;
	OBSBlueprintOutputPin* outputPin;
};
