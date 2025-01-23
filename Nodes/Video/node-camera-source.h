#pragma once
#include "Core/obs-blueprint-node.h"
#include <dshowcapture.hpp>

#include "Structs/obs-frame.h"

class NodeCameraSource : public OBSBlueprintNode {
public:
	NodeCameraSource();
	~NodeCameraSource() override;

	void onGraphBeginTick(float deltaSeconds) override;
	void onGraphEndTick() override;
	void execute(float deltaSeconds) override;


	static OBSBlueprintNode* CreateDefault() { return new NodeCameraSource(); }
	static std::vector<PinType> InputPins() { return {INT_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:

	// The following functions are mainly from win-dshow plugin
	inline bool setup();
	inline bool updateConfig();
	inline void setFrame(const OBSFrame& f);
	void onVideoData(const DShow::VideoConfig &config, unsigned char *data, size_t size, long long startTime,
			     long long endTime, long rotation);
	void onReactivate();

	OBSFrame currentFrame = OBSFrame::EmptyFrame;
	std::atomic_bool needFrameUpdate = false;

	int32_t currDeviceIndex = -1;
	std::vector<DShow::VideoDevice> availableDevices{};

	DShow::VideoConfig videoConfig;
	DShow::Device device{};
	bool running = false;

	OBSBlueprintInputPin* indexPin;
	OBSBlueprintInputPin* resolutionPin;
	OBSBlueprintOutputPin* videoPin;

};
