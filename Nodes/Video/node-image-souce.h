#pragma once
#include "Core/obs-blueprint-node.h"
#include "graphics/image-file.h"
#include "Structs/obs-frame.h"


class NodeImageSource : public OBSBlueprintNode {
public:
	NodeImageSource();
	NodeImageSource(const std::string& defaultFile);
	~NodeImageSource() override;

	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeImageSource(); }
	static std::vector<PinType> InputPins() { return {STRING_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:
	OBSBlueprintInputPin* pathPin;
	OBSBlueprintOutputPin* videoPin;

	float checkModifiedCounter = 0.0f;
	float gifFrameSec = 0.0f;
	float gifFrameLimit = 0.0f;
	size_t gifCurrIndex = 0;
	std::string filePath{};
	time_t fileModifiedTime = -1;
	std::vector<OBSFrame> imageFrames;
	std::vector<uint32_t> imageDelayMs;

	volatile bool loaded;

	void unload();
	void load();

	time_t getFileModifiedTime() const;

};
