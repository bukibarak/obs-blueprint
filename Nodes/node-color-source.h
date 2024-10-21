#pragma once

#include "Core/obs-blueprint-node.h"
#include "Structs/pixel.h"

class NodeColorSource : public OBSBlueprintNode {
public:

	NodeColorSource();
	NodeColorSource(const int32_t& defaultWidth, const int32_t& defaultHeight, const uint32_t& defaultColor);
	~NodeColorSource() override;

	void execute(float deltaSeconds) override;

	OBSBlueprintInputPin* getWidthPin() const {return pinWidth; }
	OBSBlueprintInputPin* getHeightPin() const {return pinHeight; }
	OBSBlueprintInputPin* getColorPin() const {return pinColor; }
	OBSBlueprintOutputPin* getVideoPin() const { return pinVideo; }

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
