#pragma once
#include "Core/obs-blueprint-node.h"

class OBSBlueprintVariable;

class NodeVariableGet : public OBSBlueprintNode {
public:
	NodeVariableGet(OBSBlueprintVariable* linkedVariable);
	~NodeVariableGet();

	void execute(float deltaSeconds) override;

private:
	OBSBlueprintVariable* variable;
	OBSBlueprintOutputPin* pin;

	std::function<void(std::string, std::string)> variableRenamed = [this](std::string oldValue, std::string newValue) {
			displayName = newValue;
	};
};
