#pragma once
#include "Core/obs-blueprint-node.h"
#include "Structs/pixel.h"


class NodeVideoLayer : public OBSBlueprintNode {
	enum State : uint8_t {Empty, FgOnly, BgOnly, Blend};
public:
	NodeVideoLayer();
	// ~NodeVideoLayer() override = default;

	void execute(float deltaSeconds) override;
	void onGraphBeginTick(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeVideoLayer(); }
	static std::vector<PinType> InputPins() { return {VIDEO_PIN, VIDEO_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:

	State previousState = Empty;

	OBSBlueprintInputPin* foreground;
	OBSBlueprintInputPin* background;

	OBSBlueprintOutputPin* result;
};
