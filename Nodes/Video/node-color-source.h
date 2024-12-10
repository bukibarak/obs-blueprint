#pragma once

#include "Core/obs-blueprint-node.h"
#include "Structs/pixel.h"

class NodeColorSource : public OBSBlueprintNode {
public:

	NodeColorSource();
	NodeColorSource(const int32_t& defaultWidth, const int32_t& defaultHeight, const uint32_t& defaultColor);

	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeColorSource(200, 100, 0xFFFF0000); }
	static std::vector<PinType> InputPins() { return {INT_PIN, INT_PIN, COLOR_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:
	OBSBlueprintInputPin* pinWidth;
	OBSBlueprintInputPin* pinHeight;
	OBSBlueprintInputPin* pinColor;
	OBSBlueprintOutputPin* pinVideo;
	uint32_t color;
};
