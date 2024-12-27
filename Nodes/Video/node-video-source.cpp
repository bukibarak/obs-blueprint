#include "node-video-source.h"

#include "Structs/obs-frame.h"

NodeVideoSource::NodeVideoSource() : OBSBlueprintNode(obs_module_text("NodeVideoSource"))
{
	filePin = createInputPin(STRING_PIN, std::string(), "filename");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "video");
}

NodeVideoSource::NodeVideoSource(const std::string &defaultFile) : NodeVideoSource()
{
	filePin->setValue(defaultFile);
}

void NodeVideoSource::execute(float deltaSeconds)
{
	// 1. Check if file have changed and rebuild reader if needed
	const std::string& filePinValue = filePin->getValue<std::string>();
	if (!filePinValue.empty() && filePinValue != currentFile) {
		videoTime = 0.0f;
		currentFile = filePinValue;
		valid = reader.open(filePinValue);
		if (!valid) {
			try {
				int val = std::stoi(filePinValue);
				valid = reader.open(val, cv::CAP_DSHOW);
			}
			catch (std::exception const &e) {
				GDebug("std::exception occured: %s", e.what());
			}
		}
	}

	// 2. Check if video is valid and frame can be captured
	if (valid) {
		double fps = reader.get(cv::CAP_PROP_FPS);
		if (fps > 0.0) {
			float desiredTime = videoTime + deltaSeconds;
			float t = videoTime;
			while (t < desiredTime) {
				if (!reader.read(frame))
					break;
				t += static_cast<float>(1.0 / fps);
			}
			if (t != videoTime) {
				cv::cvtColor(frame, frame, cv::COLOR_BGR2BGRA);
				videoPin->setValue(OBSFrame(frame));
				videoTime = static_cast<float>(reader.get(cv::CAP_PROP_POS_MSEC) / 1000.0);
				haveExecutedThisCycle = true;
			}
			else {
				videoPin->getValuePtr<OBSFrame>()->updated = false;
			}
		}
		else {
			if (reader.read(frame)) {
				cv::cvtColor(frame, frame, cv::COLOR_BGR2BGRA);
				videoPin->setValue(OBSFrame(frame));
				haveExecutedThisCycle = true;
			}
			else {
				videoPin->getValuePtr<OBSFrame>()->updated = false;
			}
		}
	}
	else {
		videoPin->setValue(OBSFrame::EmptyFrame);
	}
}
