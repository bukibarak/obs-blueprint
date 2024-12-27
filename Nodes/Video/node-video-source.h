#pragma once
#include "Core/obs-blueprint-node.h"
#include "opencv-conf.h"

class NodeVideoSource : public OBSBlueprintNode {
public:
	NodeVideoSource();
	NodeVideoSource(const std::string& defaultFile);

	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeVideoSource(); }
	static std::vector<PinType> InputPins() { return {STRING_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:

	OBSBlueprintInputPin* filePin;
	std::string currentFile;
	float videoTime = 0.0f;
	bool valid = false;
	OBSBlueprintOutputPin* videoPin;

	cv::UMat frame;
	cv::VideoCapture reader;
};
