#pragma once
#include "Core/obs-blueprint-node.h"

struct OBSFrame;

class NodeImageSource2 : public OBSBlueprintNode {
public:
	NodeImageSource2();
	NodeImageSource2(const std::string& file);
	~NodeImageSource2() override;

	void onGraphBeginTick(float deltaSeconds) override;
	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeImageSource2(); }
	static std::vector<PinType> InputPins() { return {STRING_PIN, BOOLEAN_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:
	OBSBlueprintInputPin* pathPin;
	OBSBlueprintInputPin* loadPin;
	OBSBlueprintOutputPin* videoPin;

	std::string currFile;
	bool valid = false;
	time_t fileModifiedTime;
	std::vector<OBSFrame> frames{};
	std::vector<uint32_t> framesDelay{};
	int currFrameIndex = -1;
	float gifFrameSec = 0.0f;

	float modifiedTimeCounter = 0.0f;
	float modifiedTimeDelay = 1.0f; // Check for file modified time every 1sec, only check when node is running

	std::thread* th = nullptr;
	std::atomic_bool loading = false;
	std::atomic_int availableFrames = 0;
	std::mutex destroyMtx{};

	void unload();
	void load();
	void threadLoad();

	time_t getFileModifiedTime() const;
};
