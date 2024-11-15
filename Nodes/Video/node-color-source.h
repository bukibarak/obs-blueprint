#pragma once

#include "Core/obs-blueprint-node.h"
#include "Structs/pixel.h"

class NodeColorSource : public OBSBlueprintNode {
public:

	NodeColorSource();
	NodeColorSource(const int32_t& defaultWidth, const int32_t& defaultHeight, const uint32_t& defaultColor);
	~NodeColorSource() override;

	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeColorSource(200, 100, 0xFF0000FF); }
	static std::vector<PinType> InputPins() { return {INT_PIN, INT_PIN, COLOR_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:
	OBSBlueprintInputPin* pinWidth;
	OBSBlueprintInputPin* pinHeight;
	OBSBlueprintInputPin* pinColor;
	OBSBlueprintOutputPin* pinVideo;
	int32_t width;
	int32_t height;
	uint32_t color;
	pixel* pixels = nullptr;
};
