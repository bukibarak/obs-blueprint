#include "node-camera-source.h"

#include "Helpers/wstring-converter.h"
#include "Structs/obs-frame.h"

// Taken from win-dshow.cpp
namespace WinDShow {
struct Resolution {
	int cx, cy;

	inline Resolution(int cx, int cy) : cx(cx), cy(cy) {}
};

static void InsertResolution(std::vector<Resolution> &resolutions, int cx, int cy)
{
	int bestCY = 0;
	size_t idx = 0;

	for (; idx < resolutions.size(); idx++) {
		const Resolution &res = resolutions[idx];
		if (res.cx > cx)
			break;

		if (res.cx == cx) {
			if (res.cy == cy)
				return;

			if (!bestCY)
				bestCY = res.cy;
			else if (res.cy > bestCY)
				break;
		}
	}

	resolutions.insert(resolutions.begin() + idx, Resolution(cx, cy));
}

static inline void AddCap(std::vector<Resolution> &resolutions, const DShow::VideoInfo &cap)
{
	InsertResolution(resolutions, cap.minCX, cap.minCY);
	InsertResolution(resolutions, cap.maxCX, cap.maxCY);
}
}


NodeCameraSource::NodeCameraSource() : OBSBlueprintNode(obs_module_text("NodeCameraSource"))
{
	indexPin = createInputPin(INT_PIN, int32_t(), "index");
	resolutionPin = createInputPin(STRING_PIN, std::string(), "resolution");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "video");

	// TODO: allow to select resolution (combo box) --> see how it's done in win-dshow.cpp
	resolutionPin->graphicsOptions.showAsComboBox = true;

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
	// Check if id have changed
	if (const int32_t& newId = indexPin->getValue<int32_t>(); newId != currDeviceIndex) {
		currDeviceIndex = newId;

		// Try to update device with new id
		if (!setup())
			setFrame(OBSFrame::EmptyFrame);
	}
}

void NodeCameraSource::onGraphEndTick()
{
	// Stop device (if running) when node is no more ticked
	if (!haveTickedThisCycle && running) {
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
	const DShow::VideoDevice& videoDevice = availableDevices[currDeviceIndex];
	std::vector<WinDShow::Resolution> resolutions;

	resolutionPin->graphicsOptions.comboBoxOptions.clear();
	for (const auto& cap : videoDevice.caps) {
		WinDShow::AddCap(resolutions, cap);
	}
	for (const auto& resolution : resolutions) {
		std::stringstream ss;
		ss << resolution.cx << "x" << resolution.cy;
		GDebug("%s", ss.str().c_str());
		resolutionPin->graphicsOptions.comboBoxOptions[ss.str()] = ss.str();
	}
	resolutionPin->graphicsOptions.onOptionsChanged.execute();

	videoConfig.name = videoDevice.name;
	videoConfig.path = videoDevice.path;
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


