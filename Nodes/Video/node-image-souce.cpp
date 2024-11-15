#include "node-image-souce.h"

#include <fstream>

#include "Structs/video-frame.h"
#include "util/platform.h"
#include "util/threading-windows.h"

NodeImageSource::NodeImageSource() : OBSBlueprintNode(obs_module_text("NodeImageSource"))
{
	pathPin = createInputPin(STRING_PIN, std::string(), "Path");
	videoPin = createOutputPin(VIDEO_PIN, video_frame(), "Video");

	//TODO ADD EXECUTION PIN TO RESTART GIF

	video = videoPin->getValuePtr<video_frame>();
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
				update(true);
			}
		}
		lastTime = frame_time;
	}
}

void NodeImageSource::unload()
{
	os_atomic_set_bool(&loaded, false);

	// Only delete image data ptr if it has been converted from image format
	bool deleteData = image.format == GS_BGRA || image.format == GS_BGRX;

	obs_enter_graphics();
	gs_image_file_free(&image);
	obs_leave_graphics();
	video->width = 0;
	video->height = 0;
	video->pixels = nullptr;
	if(deleteData) {
		delete[] imageData;
	}
	imageData = nullptr;
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

	update();
	GInfo("[NodeImageSource] image loaded (%s)", filePath.c_str());

	os_atomic_set_bool(&loaded, true);
}

void NodeImageSource::update(bool updateTick)
{
	if(image.format == GS_RGBA) {
		// In this case, the pixel format is already good, no need to convert
		if(image.is_animated_gif) {
			if(updateTick) {
				imageData = reinterpret_cast<pixel*>(image.animation_frame_cache[image.cur_frame]);
			} else {
				imageData = static_cast<pixel*>(image.gif.frame_image);
			}
		}
		else {
			imageData = reinterpret_cast<pixel*>(image.texture_data);
		}
	}
	else {
		bool ok = convert(updateTick);
		if(!ok) return;
	}

	video->width = image.cx;
	video->height = image.cy;
	video->pixels = imageData;
}

bool NodeImageSource::convert(bool updateTick)
{
	delete[] imageData;

	if(image.format != GS_BGRA && image.format != GS_BGRX) {
		GError("[NodeImageSource] Unsupported image color format. Supported are RGBA, BGRA and BGRX");
		return false;
	}

	imageData = new pixel[image.cx * image.cy];
	uint32_t* convert;
	if(image.is_animated_gif) {
		if(updateTick) {
			convert = reinterpret_cast<uint32_t*>(image.animation_frame_cache[image.cur_frame]);
		}
		else {
			convert = static_cast<uint32_t*>(image.gif.frame_image);
		}
	}
	else {
		convert = reinterpret_cast<uint32_t*>(image.texture_data);
	}

	if(image.format == GS_BGRA) {
		// Perform conversion
		for(uint32_t i = 0; i < image.cx * image.cy; i++) {
			imageData[i].b = (convert[i] & 0x000000FF) >> 0;
			imageData[i].g = (convert[i] & 0x0000FF00) >> 8;
			imageData[i].r = (convert[i] & 0x00FF0000) >> 16;
			imageData[i].a = (convert[i] & 0xFF000000) >> 24;
		}
	}
	else {
		// Perform conversion
		for(uint32_t i = 0; i < image.cx * image.cy; i++) {
			imageData[i].b = (convert[i] & 0x000000FF) >> 0;
			imageData[i].g = (convert[i] & 0x0000FF00) >> 8;
			imageData[i].r = (convert[i] & 0x00FF0000) >> 16;
			imageData[i].a = 0xFF; // Alpha is always equal to 1.0 with BGRX
		}
	}
	return true;
}

time_t NodeImageSource::getFileModifiedTime() const
{
	struct stat stat;
	if(os_stat(filePath.c_str(), &stat) != 0)
		return -1;
	return stat.st_mtime;
}
