#pragma once
#include "Core/obs-blueprint-node.h"
#include "graphics/image-file.h"
#include "Structs/obs-frame.h"

class NodeImageSource : public OBSBlueprintNode {
public:
	NodeImageSource();
	NodeImageSource(const std::string& defaultFile);
	~NodeImageSource() override;

	void onGraphBeginTick(float deltaSeconds) override;
	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeImageSource(); }
	static std::vector<PinType> InputPins() { return {STRING_PIN, BOOLEAN_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:
	OBSBlueprintInputPin* pathPin;
	OBSBlueprintInputPin* loadPin;
	OBSBlueprintOutputPin* videoPin;

	float checkModifiedCounter = 0.0f;
	float gifFrameSec = 0.0f;
	float gifFrameLimit = 0.0f;

	std::string filePath{};
	time_t fileModifiedTime = -1;
	size_t imageFrameCurrIndex = SIZE_MAX;
	std::vector<OBSFrame> imageFrames;
	std::vector<uint32_t> imageDelayMs;

	std::thread* currentLoadThread = nullptr;
	std::mutex mutex;

	time_t getFileModifiedTime() const;
	void unload();
	void asyncLoad();
	void load();

	void _internal_threadLoad();
};
