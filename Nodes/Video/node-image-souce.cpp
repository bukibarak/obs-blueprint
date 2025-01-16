#include "node-image-souce.h"

#include <fstream>
#include "util/platform.h"

NodeImageSource::NodeImageSource() : OBSBlueprintNode(obs_module_text("NodeImageSource"))
{
	pathPin = createInputPin(STRING_PIN, std::string(), "Path");
	loadPin = createInputPin(BOOLEAN_PIN, true, "Direct load");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "Video");

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
	mutex.lock();
	delete currentLoadThread;
	unload();
	mutex.unlock();
}

void NodeImageSource::onGraphBeginTick(float deltaSeconds)
{
	if (loadPin->getValue<bool>() && pathPin->getValue<std::string>() != filePath) {
		filePath = pathPin->getValue<std::string>();
		asyncLoad();
	}
}

void NodeImageSource::execute(float deltaSeconds)
{
	// 1. Check if path have changed and reload image if so
	if(pathPin->getValue<std::string>() != filePath) {
		filePath = pathPin->getValue<std::string>();
		asyncLoad();
	}

	// 2. Check if image modified time have changed and reload image if so
	checkModifiedCounter += deltaSeconds;
	if(checkModifiedCounter >= 1.0f) { // check every 1 sec.
		time_t t = getFileModifiedTime();

		checkModifiedCounter = 0.0f;
		if(fileModifiedTime != t) {
			asyncLoad();
			haveExecutedThisCycle = true;
		}
	}

	// Use mutex to prevent executing code below while new image is beeing loaded, use try_lock to prevent freezing main tick thread
	if (mutex.try_lock()) {
		if (imageFrames.empty()) { // 3. Check if image have frames and return empty frame if not
			if (!videoPin->getValue<OBSFrame>().empty()) {
				videoPin->setValue(OBSFrame::EmptyFrame);
				haveExecutedThisCycle = true;
			}
		}
		else if (imageFrameCurrIndex == SIZE_MAX) { // 4. Check if image index is undefined and set it to 0 if true
			//videoPin->setValue(imageFrames.front()); // NOT NEEDED because video pin will be set at the end of load() function
			imageFrameCurrIndex = 0; // TODO: why is that already??
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

void NodeImageSource::unload()
{
	videoPin->setValue(OBSFrame::EmptyFrame);
	fileModifiedTime = -1;
	imageFrameCurrIndex = SIZE_MAX; // Set as 'undefined' for execution pin
	imageFrames.clear();
	imageDelayMs.clear();
}

void NodeImageSource::asyncLoad()
{
	mutex.lock();
	if (currentLoadThread != nullptr)
		currentLoadThread->join();
	mutex.unlock();

	currentLoadThread = new std::thread(&NodeImageSource::_internal_threadLoad, this);
	currentLoadThread->detach(); // TODO: necessary ?
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

	fileModifiedTime = getFileModifiedTime();
	gs_image_file_t image;
	gs_image_file_init(&image, filePath.c_str());
	if (auto it = FrameFormat::FromGsColorFormat.find(image.format); it == FrameFormat::FromGsColorFormat.end()) {
		GWarn("[NodeImageSource] Unsupported image format: %s", EnumStr::gs_color_format[image.format]);
		obs_enter_graphics();
		gs_image_file_free(&image);
		obs_leave_graphics();
	}
	else {
		if (image.is_animated_gif) {
			uint64_t timeNs = 0;
			for (uint32_t i = 0; i < image.gif.frame_count; i++) {
				gif_frame& gifFrame = image.gif.frames[i];
				uint32_t frame_delay = gifFrame.frame_delay == 0 ? 10 : gifFrame.frame_delay;
				uint64_t frameNs = frame_delay * 10000000ULL;
				timeNs += frameNs;
				if (gs_image_file_tick(&image, frameNs + 1)) {
					imageFrames.push_back({
						static_cast<int>(image.cx),
						static_cast<int>(image.cy),
						image.animation_frame_cache[image.cur_frame],
						it->second
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
				it->second});
		}

		obs_enter_graphics();
		gs_image_file_free(&image);
		obs_leave_graphics();

		videoPin->setValue(imageFrames.empty() ? OBSFrame::EmptyFrame : imageFrames.front()); // TODO: move to execute ?
		GInfo("[NodeImageSource] image loaded (%s)", filePath.c_str());
	}
}

void NodeImageSource::_internal_threadLoad()
{
	mutex.lock();
	load();
	currentLoadThread = nullptr;
	mutex.unlock();
}
