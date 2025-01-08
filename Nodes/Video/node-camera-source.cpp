#include "node-camera-source.h"

#include "Helpers/wstring-converter.h"
#include "Structs/obs-frame.h"

NodeCameraSource::NodeCameraSource() : OBSBlueprintNode(obs_module_text("NodeCameraSource"))
{
	indexPin = createInputPin(INT_PIN, int32_t(), "index");

	DShow::Device::EnumVideoDevices(devices);
	for (int i = 0; i < devices.size(); i++) {
		indexPin->graphicsOptions.comboBoxOptions[converter::to_string(devices[i].name)] = std::to_string(i);
	}

	indexPin->graphicsOptions.showAsComboBox = true;

	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "video");
	videoPin->onConnectionChanged += videoConnectionChanged;
}

NodeCameraSource::~NodeCameraSource()
{
	if (device.Valid()) {
		device.Stop();
		device.ShutdownGraph();
	}
	videoPin->onConnectionChanged -= videoConnectionChanged; // Unnecessary, pin callbacks will be deleted by OBSBlueprintNode destructor no matter what
}

void NodeCameraSource::onGraphBeginTick(float deltaSeconds)
{
	//1. Check if id have changed
	const int32_t& newId = indexPin->getValue<int32_t>();
	if (newId != index) {
		index = newId;

		//2. Construct/update reader if needed
		if (!setup()) {
			setFrame(OBSFrame::EmptyFrame);
		}
		else if (videoPin->isConnected()) {
			videoConnectionChanged();
		}
	}
}

void NodeCameraSource::execute(float deltaSeconds)
{
	if (mutex.try_lock()) {
		if (frameUpdated) {
			videoPin->setValue(frame);
			haveExecutedThisCycle = true;
		}
		mutex.unlock();
	}
}


// The following functions are mainly from win-dshow plugin
inline bool NodeCameraSource::setup()
{
	validFormat = false;

	if (!device.ResetGraph()) {
		GError("[NodeCameraSource] Failed to reset capture device graph");
		return false;
	}
	if (!updateConfig()) {
		GError("[NodeCameraSource] Failed to update capture video configuration");
		return false;
	}
	if (!device.ConnectFilters()) { // TODO: What is this ? Necessary ?
		GError("[NodeCameraSource] Failed to connect filters");
		return false;
	}
	return true;
}

inline bool NodeCameraSource::updateConfig()
{
	if (index < 0 || index >= devices.size()) {
		GWarn("[NodeCameraSource] Invalid device index: %d.", index);
		return false;
	}
	videoConfig.name = devices[index].name;
	videoConfig.path = devices[index].path;
	videoConfig.reactivateCallback = [this] { onReactivate(); };
	videoConfig.callback = std::bind(&NodeCameraSource::onVideoData, this, std::placeholders::_1, std::placeholders::_2,
					 std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
	if (!device.SetVideoConfig(&videoConfig)) {
		GWarn("[NodeCameraSource] Failed to set video config for device '%ls'", devices[index].name.c_str());
		return false;
	}
	if (FrameFormat::FromDShowFormat.find(videoConfig.format) == FrameFormat::FromDShowFormat.end()) {
		GWarn("[NodeCameraSource] Unsupported Video format: %d", videoConfig.format);
		return false;
	}
	validFormat = true;
	return true;
}

void NodeCameraSource::setFrame(const OBSFrame &f)
{
	mutex.lock();
	frame = f;
	frameUpdated = true;
	mutex.unlock();
}

void NodeCameraSource::onReactivate()
{
	GDebug("NodeCameraSource::onReactivate, what to do?");
}

void NodeCameraSource::onVideoData(const DShow::VideoConfig &config, unsigned char *data, size_t size,
	long long startTime, long long endTime, long rotation)
{
	if (auto it = FrameFormat::FromDShowFormat.find(config.format); it != FrameFormat::FromDShowFormat.end()) {
		OBSFrame newFrame{config.cx, config.cy_abs, data, it->second};
		setFrame(newFrame);
	}
}


