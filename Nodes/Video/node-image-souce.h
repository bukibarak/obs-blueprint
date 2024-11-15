#pragma once
#include "Core/obs-blueprint-node.h"
#include "graphics/image-file.h"
#include "Structs/pixel.h"

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
	video_frame* video;

	float checkModifiedCounter = 0.0f;
	std::string filePath{};
	time_t fileModifiedTime = -1;
	gs_image_file_t image{};
	pixel* imageData = nullptr;
	uint64_t lastTime = 0;

	volatile bool loaded;

	void unload();
	void load();
	void update(bool updateTick = false);
	bool convert(bool updateTick);

	time_t getFileModifiedTime() const;

};
