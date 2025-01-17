#include "node-camera-source.h"

#include "Helpers/wstring-converter.h"
#include "Structs/obs-frame.h"

NodeCameraSource::NodeCameraSource() : OBSBlueprintNode(obs_module_text("NodeCameraSource"))
{
	indexPin = createInputPin(INT_PIN, int32_t(), "index");

	DShow::Device::EnumVideoDevices(availableDevices);
	for (int i = 0; i < availableDevices.size(); i++) {
		indexPin->graphicsOptions.comboBoxOptions[converter::to_string(availableDevices[i].name)] = std::to_string(i);
	}

	indexPin->graphicsOptions.showAsComboBox = true;
}

NodeCameraSource::~NodeCameraSource()
{
	//TODO: Needed ? because if look on dshowcapture.cpp and device.cpp destructor it seems to be automatically done
	device.Stop();
	device.ShutdownGraph();
}

void NodeCameraSource::onGraphBeginTick(float deltaSeconds)
{
	//1. Check if id have changed
	if (const int32_t& newId = indexPin->getValue<int32_t>(); newId != currDeviceIndex) {
		currDeviceIndex = newId;

		//2. Try to update device with new id
		if (!setup())
			setFrame(OBSFrame::EmptyFrame);
	}

	//3. Stop device (if running) when pin is not connected
	// TODO: Better, stop device when pin is not used for main video output (directly or child node), even if connected
	// TODO: i.e.: this node is connected to a layer not (so isConnected() is true), but layer is connected to nothing --> should also stop device recording
	if (running && !videoPin->isConnected()) {
		device.Stop();
		running = false;
		setFrame(OBSFrame::EmptyFrame);
	}
}

void NodeCameraSource::execute(float deltaSeconds)
{
	// Try to (re)start device (if not running) when pin is connected
	if (!running && videoPin->isConnected()) {
		if (device.Start() ==DShow::Result::Error) {
			DShow::DeviceId id;
			device.GetVideoDeviceId(id);
			GError("[NodeCameraSource] Failed to start capture device '%ls'", id.name.c_str());
		}
		running = true; // Set running to true even if device couldn't be started, TODO: move to if-statement ?
	}

	// Update frame video pin if needed
	if (needFrameUpdate) {
		videoPin->setValue(currentFrame);
		needFrameUpdate = false;
		haveExecutedThisCycle = true;
	}
	else {
		videoPin->getValuePtr<OBSFrame>()->updated = false;
	}
}


// The following functions are mainly from win-dshow plugin
inline bool NodeCameraSource::setup()
{
	running = false;
	if (!device.ResetGraph()) {
		GError("[NodeCameraSource] Failed to reset capture device graph");
		return false;
	}
	if (!updateConfig()) {
		GError("[NodeCameraSource] Failed to update capture video configuration");
		return false;
	}
	if (!device.ConnectFilters()) { // TODO: What is this ? Necessary ? --> yes
		GError("[NodeCameraSource] Failed to connect filters");
		//device.ShutdownGraph();
		return false;
	}
	return true;
}

inline bool NodeCameraSource::updateConfig()
{
	if (currDeviceIndex < 0 || currDeviceIndex >= availableDevices.size()) {
		GWarn("[NodeCameraSource] Invalid device index: %d.", currDeviceIndex);
		return false;
	}
	/* TODO: WHY IS DESIRED FORMAT NOT WORKING (at least with NV12 internal format) ?? */
	// videoConfig.useDefaultConfig = false;
	// videoConfig.internalFormat = DShow::VideoFormat::Any;
	// videoConfig.format = DShow::VideoFormat::ARGB;
	/* -------------------------- */
	videoConfig.name = availableDevices[currDeviceIndex].name;
	videoConfig.path = availableDevices[currDeviceIndex].path;
	videoConfig.reactivateCallback = [this] { onReactivate(); };
	videoConfig.callback = std::bind(&NodeCameraSource::onVideoData, this, std::placeholders::_1, std::placeholders::_2,
					 std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
	if (!device.SetVideoConfig(&videoConfig)) {
		GWarn("[NodeCameraSource] Failed to set video config for device '%ls'", availableDevices[currDeviceIndex].name.c_str());
		return false;
	}
	if (FrameFormat::FromDShowFormat.find(videoConfig.format) == FrameFormat::FromDShowFormat.end()) {
		GWarn("[NodeCameraSource] Unsupported Video format: %s", EnumStr::VideoFormat[videoConfig.format]);
		return false;
	}

	return true;
}

void NodeCameraSource::setFrame(const OBSFrame &f)
{
	currentFrame = f;
	needFrameUpdate = true;
}

void NodeCameraSource::onVideoData(const DShow::VideoConfig &config, unsigned char *data, size_t size,
	long long startTime, long long endTime, long rotation)
{
	if (auto it = FrameFormat::FromDShowFormat.find(config.format); it != FrameFormat::FromDShowFormat.end()) {
		OBSFrame newFrame{config.cx, config.cy_abs, data, it->second};
		setFrame(newFrame);
	}
}

void NodeCameraSource::onReactivate()
{
	/* Nothing to do on reactivate ?? TODO: check */
}


