#include "node-image-souce.h"

#include <fstream>

#include "util/platform.h"
#include "util/threading-windows.h"

NodeImageSource::NodeImageSource() : OBSBlueprintNode(obs_module_text("NodeImageSource"))
{
	pathPin = createInputPin(STRING_PIN, std::string(), "Path");
	loadPin = createInputPin(BOOLEAN_PIN, true, "Direct load");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "Video");

	pathPin->onValueChanged += onPathChanged;

	// TODO: ADD EXECUTION PIN TO RESTART GIF
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
	pathPin->onValueChanged -= onPathChanged; // Unnecessary, pin callbacks will be deleted by parent destructor no matter what
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

	if (imageFrames.empty()) { // 3. Check if image have frames and return empty frame if not
		if (!videoPin->getValue<OBSFrame>().empty()) {
			videoPin->setValue(OBSFrame::EmptyFrame);
			haveExecutedThisCycle = true;
		}
	}
	else if (imageFrameCurrIndex == SIZE_MAX) { // 4. Check if image index is undefined and set it to 0 if true
		//videoPin->setValue(imageFrames.front()); // NOT NEEDED because video pin will be set at the end of load() function
		imageFrameCurrIndex = 0;
	}
	else if (imageFrames.size() > 1) { // 5. If image is a gif, update frame if necessary
		gifFrameSec += deltaSeconds;
		while (gifFrameSec > gifFrameLimit)
			gifFrameSec -= gifFrameLimit;

		float timeIndex = 0.0f;
		size_t index = -1;
		while (timeIndex < gifFrameSec) {
			++index;
			timeIndex += imageDelayMs[index] / 1000.0f;
		}

		if (index != imageFrameCurrIndex) {
			imageFrameCurrIndex = index;
			videoPin->setValue(imageFrames[index]);
			videoPin->getValuePtr<OBSFrame>()->updated = true;
			haveExecutedThisCycle = true;
		}
		else {
			videoPin->getValuePtr<OBSFrame>()->updated = false;
		}
	}
	else { // 6. If image is static, do nothing
		videoPin->getValuePtr<OBSFrame>()->updated = false;
	}
}

void NodeImageSource::unload()
{
	videoPin->setValue(OBSFrame::EmptyFrame);
	fileModifiedTime = -1;
	imageFrameCurrIndex = SIZE_MAX; // Set as 'undefined' for execution pin
	imageFrames.clear();
	imageDelayMs.clear();
}


void NodeImageSource::load()
{
	if (mutex.try_lock()) {
		unload();
		if(filePath.empty()) {
			GWarn("[NodeImageSource] no file path provided");
			mutex.unlock();
			return;
		}
		if(!std::ifstream(filePath).good()) {
			GWarn("[NodeImageSource] Couldn't open file at %s", filePath.c_str());
			mutex.unlock();
			return;
		}

		fileModifiedTime = getFileModifiedTime();
		gs_image_file_t image;
		gs_image_file_init(&image, filePath.c_str());
		if (image.format != GS_RGBA && image.format != GS_BGRA) {
			GWarn("[NodeImageSource] Unsupported image format: %d", image.format);
			obs_enter_graphics();
			gs_image_file_free(&image);
			obs_leave_graphics();
			mutex.unlock();
			return;
		}

		if (image.is_animated_gif) {
			uint64_t timeNs = 0;
			for (uint32_t i = 0; i < image.gif.frame_count; i++) {
				gif_frame& gifFrame = image.gif.frames[i];
				uint32_t frame_delay = gifFrame.frame_delay == 0 ? 10 : gifFrame.frame_delay;
				uint64_t frameNs = frame_delay * 10000000ULL;
				timeNs += frameNs;
				bool updated = gs_image_file_tick(&image, frameNs + 1);
				if (updated) {
					imageFrames.push_back({
						static_cast<int>(image.cx),
						static_cast<int>(image.cy),
						image.animation_frame_cache[image.cur_frame],
						FrameFormat::FromGsColorFormat.at(image.format)
					});
					frame_delay = image.gif.frames[image.cur_frame].frame_delay == 0 ? 10 : image.gif.frames[image.cur_frame].frame_delay;
					imageDelayMs.push_back(frame_delay * 10);
				}
				else {
					GError("[NodeImageSource] Couldn't decode gif frame #%d at time %.3fs", i, static_cast<float>(timeNs / 1000000ULL) / 1000.0f);
				}
			}
			gifFrameSec = 0.0f;
			gifFrameLimit = static_cast<float>(timeNs / 1000000ULL) / 1000.0f;
		}
		else {
			imageFrames.push_back({
				static_cast<int>(image.cx),
				static_cast<int>(image.cy),
				image.texture_data,
				FrameFormat::FromGsColorFormat.at(image.format)});
		}

		obs_enter_graphics();
		gs_image_file_free(&image);
		obs_leave_graphics();

		videoPin->setValue(imageFrames.empty() ? OBSFrame::EmptyFrame : imageFrames.front()); // TODO: move to execute ?
		GInfo("[NodeImageSource] image loaded (%s)", filePath.c_str());
		mutex.unlock();
	}
}

time_t NodeImageSource::getFileModifiedTime() const
{
	struct stat stat;
	if(os_stat(filePath.c_str(), &stat) != 0)
		return -1;
	return stat.st_mtime;
}
