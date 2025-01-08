#pragma once
#include "Core/obs-blueprint-node.h"
#include "opencv-conf.h"
#include "Structs/obs-frame.h"

extern "C" {
	#include <media-playback/media-playback.h> // media-playback is compiled with C compiler, not C++
}

class NodeVideoSource : public OBSBlueprintNode {
public:
	NodeVideoSource();
	NodeVideoSource(const std::string& defaultFile);

	void execute(float deltaSeconds) override;

	static OBSBlueprintNode* CreateDefault() { return new NodeVideoSource(); }
	static std::vector<PinType> InputPins() { return {STRING_PIN, BOOLEAN_PIN}; }
	static std::vector<PinType> OutputPins() { return {VIDEO_PIN}; }

private:

	OBSBlueprintInputPin* filePin;
	OBSBlueprintInputPin* loadPin;
	OBSBlueprintOutputPin* videoPin;

	std::string currFile;
	OBSFrame frame = OBSFrame::EmptyFrame;
	bool frameUpdated = false;

	std::mutex mutex; // Prevent loading the same video multiple times if called by multiple threads
	void load();
	void unload();
	void convert(obs_source_frame* obsFrame);

	mp_media_info info{};
	media_playback_t* player = nullptr;

	static void OnVideoFrame(void* nodePtr, obs_source_frame* frame);
	static void OnPreloadFrame(void* nodePtr, obs_source_frame* frame);
	static void OnSeekFrame(void* nodePtr, obs_source_frame* frame);
	static void OnVideoStopped(void* nodePtr);
};
