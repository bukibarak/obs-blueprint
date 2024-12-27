#pragma once
#include "Core/obs-blueprint-node.h"
#include "opencv-conf.h"
#include <dshowcapture.hpp>

#include "Structs/obs-frame.h"

class NodeCameraSource : public OBSBlueprintNode {
public:
	NodeCameraSource();
	~NodeCameraSource() override;

	void onGraphBeginTick(float deltaSeconds) override;
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

	std::mutex mutex{};

	OBSFrame frame = OBSFrame::EmptyFrame;
	bool frameUpdated = false;
	bool validFormat = false;

	int32_t index = -1;
	std::vector<DShow::VideoDevice> devices{};

	DShow::VideoConfig videoConfig;
	DShow::Device device{};

	OBSBlueprintInputPin* indexPin;
	OBSBlueprintOutputPin* videoPin;

	std::function<void()> videoConnectionChanged = [this] {
		if (device.Valid()) {
			if (videoPin->isConnected()) {
				if (device.Start() == DShow::Result::Error) {
					DShow::DeviceId id;
					device.GetVideoDeviceId(id);
					GError("[NodeCameraSource] Failed to start capture device '%ls'", id.name.c_str());
				}
			}
			else {
				device.Stop();
			}
		}
	};

};
