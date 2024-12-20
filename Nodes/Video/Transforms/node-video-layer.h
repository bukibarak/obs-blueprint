﻿#pragma once
#include "Core/obs-blueprint-node.h"
#include "Structs/pixel.h"

class NodeVideoLayer : public OBSBlueprintNode {
public:
	NodeVideoLayer();
	~NodeVideoLayer() override = default;

	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeVideoLayer(); }
	static std::vector<PinType> InputPins() { return {VIDEO_PIN, VIDEO_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:
	OBSBlueprintInputPin* foreground;
	bool fgConnected = false;
	OBSBlueprintInputPin* background;
	bool bgConnected = false;
	OBSBlueprintOutputPin* result;
};
