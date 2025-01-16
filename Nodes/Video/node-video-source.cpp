#include "node-video-source.h"

NodeVideoSource::NodeVideoSource() : OBSBlueprintNode(obs_module_text("NodeVideoSource"))
{
	filePin = createInputPin(STRING_PIN, std::string(), "filename");
	loadPin = createInputPin(BOOLEAN_PIN, true, "Smart load");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "video");
}

NodeVideoSource::NodeVideoSource(const std::string &defaultFile) : NodeVideoSource()
{
	filePin->setValue(defaultFile);
}

void NodeVideoSource::execute(float deltaSeconds)
{
	const std::string& path = filePin->getValue<std::string>();
	if (!path.empty() && (currFile.empty() || path != currFile)) {
		currFile = path;
		load();
	}

	if (frameUpdated) {
		videoPin->setValue(frame);
		frameUpdated = false;
		haveExecutedThisCycle = true;
	}
	else if (!videoPin->getValue<OBSFrame>().empty()) {
		videoPin->getValuePtr<OBSFrame>()->updated = false;
	}
}

void NodeVideoSource::load()
{
	unload();

	// Only with C++20
	// info = {
	// 	.opaque = this,
	// 	.v_cb = OnVideoFrame,
	// 	.v_preload_cb = OnPreloadFrame,
	// 	.v_seek_cb = OnSeekFrame,
	// 	...
	// };

	mp_media_info updated;
	updated.opaque = this;
	updated.a_cb = nullptr;
	updated.v_cb = OnVideoFrame;
	updated.v_preload_cb = OnPreloadFrame;
	updated.v_seek_cb = OnSeekFrame;
	updated.stop_cb = OnVideoStopped;
	updated.path = currFile.c_str();
	updated.format = nullptr;
	// updated.format = "BGRA"; // Not a FFMPEG input format... What is an FFMPEG input format ? https://ffmpeg.org/ffmpeg-formats.html#Description
	updated.ffmpeg_options = nullptr;
	updated.buffering = 2 * 1024 * 1024; // 2MiB
	updated.speed = 100; // 1.0x
	updated.force_range = VIDEO_RANGE_DEFAULT;
	updated.is_linear_alpha = false;
	updated.hardware_decoding = false;
	updated.is_local_file = true;
	updated.reconnecting = false;
	updated.request_preload = false;
	updated.full_decode = false;
	info = updated;
	player = media_playback_create(&info);
	media_playback_play(player, true, false);
}

void NodeVideoSource::unload()
{
	videoPin->setValue(OBSFrame::EmptyFrame);
	media_playback_destroy(player);
	player = nullptr;
	info = {};
	frameUpdated = false;
}

void NodeVideoSource::convert( obs_source_frame * obsFrame)
{
	if (obsFrame == nullptr) {
		frame = OBSFrame::EmptyFrame;
		return;
	}

	size_t total = 0;
	for (int i = 0; i < MAX_AV_PLANES; i++) {
		if (obsFrame->data[i] == nullptr)
			break;
		uint32_t divisor = obsFrame->width / obsFrame->linesize[i];
		total += (obsFrame->width / divisor) * (obsFrame->height / divisor);
	}
	auto* data = new unsigned char[total];

	size_t pos = 0;
	for (int i = 0; i < MAX_AV_PLANES; i++) {
		if (obsFrame->data[i] == nullptr)
			break;

		uint32_t divisor = obsFrame->width / obsFrame->linesize[i];
		size_t size = (obsFrame->width / divisor) * (obsFrame->height / divisor);
		memcpy(data + pos, obsFrame->data[i], size);
		pos += size;
	}

	if (auto it = FrameFormat::FromVideoFormat.find(obsFrame->format); it != FrameFormat::FromVideoFormat.end()) {
		OBSFrame newFrame{static_cast<int>(obsFrame->width), static_cast<int>(obsFrame->height), data, it->second};
		frame = newFrame;
		frameUpdated = true;
	}
	delete[] data;
}

void NodeVideoSource::OnVideoFrame(void *nodePtr, obs_source_frame *frame)
{
	NodeVideoSource* node = static_cast<NodeVideoSource *>(nodePtr);
	node->convert(frame);
}

void NodeVideoSource::OnPreloadFrame(void *nodePtr, obs_source_frame *frame)
{
	NodeVideoSource* node = static_cast<NodeVideoSource *>(nodePtr);
	node->convert(frame);
}

void NodeVideoSource::OnSeekFrame(void *nodePtr, obs_source_frame *frame)
{
	GDebug(". Seek");
}

void NodeVideoSource::OnVideoStopped(void *nodePtr)
{
	GDebug("... Stop");
	NodeVideoSource* node = static_cast<NodeVideoSource *>(nodePtr);
	node->videoPin->setValue(OBSFrame::EmptyFrame);
	node->frameUpdated = false;
}
