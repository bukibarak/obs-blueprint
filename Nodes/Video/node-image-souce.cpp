#include "node-image-souce.h"

#include <fstream>

#include "util/platform.h"
#include "util/threading-windows.h"

NodeImageSource::NodeImageSource() : OBSBlueprintNode(obs_module_text("NodeImageSource"))
{
	pathPin = createInputPin(STRING_PIN, std::string(), "Path");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "Video");

	// TODO ADD EXECUTION PIN TO RESTART GIF
}

NodeImageSource::NodeImageSource(const std::string &defaultFile) : NodeImageSource()
{
	pathPin->setValue(defaultFile);
	filePath = defaultFile;
	load();
}

NodeImageSource::~NodeImageSource()
{
	unload();
}

void NodeImageSource::execute(float deltaSeconds)
{
	// 1. Check if path have changed and reload image if so
	if(pathPin->getValue<std::string>() != filePath) {
		filePath = pathPin->getValue<std::string>();
		load();
		haveExecutedThisCycle = true;
	}

	// 2. Check if image modified time have changed and reload image if so
	checkModifiedCounter += deltaSeconds;
	if(checkModifiedCounter >= 1.0f) { // check every 1 sec.
		time_t t = getFileModifiedTime();

		checkModifiedCounter = 0.0f;
		if(fileModifiedTime != t) {
			load();
			haveExecutedThisCycle = true;
		}
	}

	// 3. If image is a gif, update frame if necessary
	if (imageFrames.size() > 1) {
		gifFrameSec += deltaSeconds;
		while (gifFrameSec > gifFrameLimit)
			gifFrameSec -= gifFrameLimit;

		float timeIndex = 0.0f;
		size_t index = -1;
		while (timeIndex < gifFrameSec) {
			++index;
			timeIndex += imageDelayMs[index] / 1000.0f;
		}

		if (index != gifCurrIndex) {
			gifCurrIndex = index;
			videoPin->setValue(imageFrames[index]);
			haveExecutedThisCycle = true;
		}
	}

	videoPin->getValuePtr<OBSFrame>()->updated = haveExecutedThisCycle;
}

void NodeImageSource::unload()
{
	os_atomic_set_bool(&loaded, false);
	videoPin->setValue(OBSFrame::EmptyFrame);
	imageFrames.clear();
	imageDelayMs.clear();
}


void NodeImageSource::load()
{
	unload();
	if(filePath.empty()) {
		GWarn("[NodeImageSource] no file path provided");
		return;
	}
	if(!std::ifstream(filePath).good()) {
		GWarn("[NodeImageSource] Couldn't open file at %s", filePath.c_str());
		return;
	}

	if(os_atomic_load_bool(&loaded))
		return;

	fileModifiedTime = getFileModifiedTime();
	gs_image_file_t image;
	gs_image_file_init(&image, filePath.c_str());

	if (image.is_animated_gif) {
		uint64_t timeNs = 0;
		for (uint32_t i = 0; i < image.gif.frame_count; i++) {
			gif_frame& gifFrame = image.gif.frames[i];
			uint32_t frame_delay = gifFrame.frame_delay == 0 ? 10 : gifFrame.frame_delay;
			uint64_t frameNs = frame_delay * 10000000ULL;
			timeNs += frameNs;
			bool updated = gs_image_file_tick(&image, frameNs + 1);
			if (updated) {
				imageFrames.push_back({static_cast<int>(image.cx), static_cast<int>(image.cy), image.animation_frame_cache[image.cur_frame], image.format});
				frame_delay = image.gif.frames[image.cur_frame].frame_delay == 0 ? 10 : image.gif.frames[image.cur_frame].frame_delay;
				imageDelayMs.push_back(frame_delay * 10);
			}
			else {
				GError("[NodeImageSource] Couldn't decode gif frame #%i at time %.3fs", i, static_cast<float>(timeNs / 1000000ULL) / 1000.0f);
			}
		}
		gifFrameSec = 0.0f;
		gifCurrIndex = 0;
		gifFrameLimit = static_cast<float>(timeNs / 1000000ULL) / 1000.0f;
	}
	else {
		imageFrames.push_back({static_cast<int>(image.cx), static_cast<int>(image.cy), image.texture_data, image.format});
	}

	obs_enter_graphics();
	gs_image_file_free(&image);
	obs_leave_graphics();
	videoPin->setValue(imageFrames.empty() ? OBSFrame::EmptyFrame : imageFrames.front());
	GInfo("[NodeImageSource] image loaded (%s)", filePath.c_str());

	os_atomic_set_bool(&loaded, true);
}

time_t NodeImageSource::getFileModifiedTime() const
{
	struct stat stat;
	if(os_stat(filePath.c_str(), &stat) != 0)
		return -1;
	return stat.st_mtime;
}
