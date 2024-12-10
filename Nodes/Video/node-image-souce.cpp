#include "node-image-souce.h"

#include <fstream>

#include "util/platform.h"
#include "util/threading-windows.h"

NodeImageSource::NodeImageSource() : OBSBlueprintNode(obs_module_text("NodeImageSource"))
{
	pathPin = createInputPin(STRING_PIN, std::string(), "Path");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame(), "Video");

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

	// 3. If image is a gif, tick and update image
	if(image.is_animated_gif) {
		uint64_t frame_time = obs_get_video_frame_time();

		if(lastTime) {
			uint64_t elapsed = frame_time - lastTime;
			bool updated = gs_image_file_tick(&image, elapsed);

			if(updated) {
				videoPin->setValue(OBSFrame(image.cx, image.cy, image.animation_frame_cache[image.cur_frame], image.format));
			}
		}
		lastTime = frame_time;
	}
}

void NodeImageSource::unload()
{
	os_atomic_set_bool(&loaded, false);
	gs_image_file_free(&image);
	videoPin->setValue(OBSFrame());
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
	gs_image_file_init(&image, filePath.c_str());

	if (image.is_animated_gif) {
		videoPin->setValue(OBSFrame(image.cx, image.cy, static_cast<uint8_t*>(image.gif.frame_image), image.format));
	}
	else {
		videoPin->setValue(OBSFrame(image.cx, image.cy, image.texture_data, image.format));
	}

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
