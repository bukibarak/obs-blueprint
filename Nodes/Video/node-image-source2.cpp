#include "node-image-source2.h"

#include <fstream>

#include "graphics/image-file.h"
#include "Structs/obs-frame.h"
#include "util/platform.h"

NodeImageSource2::NodeImageSource2() : OBSBlueprintNode(obs_module_text("NodeImageSource"))
{
	pathPin = createInputPin(STRING_PIN, std::string(), "Path");
	loadPin = createInputPin(BOOLEAN_PIN, true, "Direct load");
	videoPin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "Video");
}

NodeImageSource2::NodeImageSource2(const std::string &file) : NodeImageSource2()
{
	pathPin->setValue(file);
}

NodeImageSource2::~NodeImageSource2()
{
	destroyMtx.lock(); // This will ensure that the loading thread has terminated before deleting the node
}

void NodeImageSource2::onGraphBeginTick(float deltaSeconds)
{
	// If direct load is set to true, check if path have changed and load image even if node is not running
	// (GraphBeginTick/GraphEndTick is executed even if the node is not connected)
	if (loadPin->getValue<bool>() && !loading) {
		if (auto& newPath = pathPin->getValue<std::string>(); newPath != currFile) {
			currFile = pathPin->getValue<std::string>();
			load();
		}
	}
}

void NodeImageSource2::execute(float deltaSeconds)
{
	// 1. Check if path have changed and reload image if so
	if (!loading) {
		if (auto& newPath = pathPin->getValue<std::string>(); newPath != currFile) {
			currFile = pathPin->getValue<std::string>();
			load();
		}
	}

	if (valid) {
		// 2. Check if image modified time have changed and reload image if so
		modifiedTimeCounter += deltaSeconds;
		if (modifiedTimeCounter >= modifiedTimeDelay) {
			modifiedTimeCounter = 0.0f;
			time_t t = getFileModifiedTime();
			if(fileModifiedTime != t)
				load();
		}

		// 3. Check if 1 frame is available --> static image
		if (availableFrames == 1) {
			if (currFrameIndex != 0) {
				videoPin->setValue(frames.front());
				currFrameIndex = 0;
				haveExecutedThisCycle = true;
			} else {
				videoPin->getValuePtr<OBSFrame>()->updated = false;
			}
		}
		// 4. Check if more frame are available --> animated image (gif)
		else if (availableFrames > 1) {
			gifFrameSec += deltaSeconds;
			float timeIndex = 0.0f;
			int index = -1;
			while (timeIndex < gifFrameSec) {
				if (index >= availableFrames - 1)
					break;
				++index;
				timeIndex += framesDelay[index] / 1000.0f;
			}


			if (index != currFrameIndex) {
				videoPin->setValue(frames[index]);
				currFrameIndex = index;
				//videoPin->getValuePtr<OBSFrame>()->updated = true;
				haveExecutedThisCycle = true;
			}
			else {
				videoPin->getValuePtr<OBSFrame>()->updated = false;
				if (!loading && index == availableFrames - 1 && gifFrameSec >= timeIndex)
					gifFrameSec = 0.0f; // Reset gif when last frame delay have been reached
			}
		}
	}
}

void NodeImageSource2::unload()
{
	availableFrames = 0;
	videoPin->setValue(OBSFrame::EmptyFrame);
	// currFile.clear();
	valid = false;
	fileModifiedTime = 0;
	gifFrameSec = 0.0f;
	modifiedTimeCounter = 0.0f;
	frames.clear();
	framesDelay.clear();
	currFrameIndex = -1;
}

void NodeImageSource2::load()
{
	unload();
	if (th != nullptr) {
		GError("[NodeImageSource] Unable to load image while previous one is still loading!");
		return;
	}
	if(currFile.empty()) {
		GWarn("[NodeImageSource] no file path provided");
		return;
	}
	if(!std::ifstream(currFile).good()) {
		GWarn("[NodeImageSource] Couldn't open file at %s", currFile.c_str());
		return;
	}
	loading = true;
	valid = true;
	fileModifiedTime = getFileModifiedTime();
	th = new std::thread(&NodeImageSource2::threadLoad, this);
	th->detach(); // TODO: Necessary ?
}

void NodeImageSource2::threadLoad()
{
	destroyMtx.lock(); // Prevent to destroy the class while thread is still active
	gs_image_file_t image;
	try {
		gs_image_file_init(&image, currFile.c_str());
		if (const auto it = FrameFormat::FromGsColorFormat.find(image.format); it == FrameFormat::FromGsColorFormat.end()) {
			GWarn("[NodeImageSource] Unsupported image format for '%s': %s", currFile.c_str(), EnumStr::gs_color_format[image.format]);
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
						frames.push_back({
							static_cast<int>(image.cx),
							static_cast<int>(image.cy),
							image.animation_frame_cache[image.cur_frame],
							it->second
						});
						frame_delay = image.gif.frames[image.cur_frame].frame_delay;
						if (frame_delay == 0)
							frame_delay = 10;
						framesDelay.push_back(frame_delay * 10);
						++availableFrames;
					}
					else {
						GWarn("[NodeImageSource] Couldn't decode gif frame #%d at time %.3fs", i, static_cast<float>(timeNs / 1000000ULL) / 1000.0f);
					}
				}
			}
			else {
				frames.push_back({static_cast<int>(image.cx), static_cast<int>(image.cy), image.texture_data, it->second});
				availableFrames = 1;
			}
			GDebug("[NodeImageSource] Loaded %d frames for image '%s'", availableFrames.load(), currFile.c_str());
		}
	}
	catch (const std::exception& e) {
		GError("[NodeImageSource] %s for file '%s'", e.what(), currFile.c_str());
	}
	catch (...) {
		GError("[NodeImageSource] Unknown exception thrown while loading file '%s'", currFile.c_str());
	}

	obs_enter_graphics();
	gs_image_file_free(&image);
	obs_leave_graphics();
	loading = false;
	th = nullptr; // Set himself to nullptr just before end of execution
	destroyMtx.unlock();
}

time_t NodeImageSource2::getFileModifiedTime() const
{
	struct stat stat;
	if(os_stat(currFile.c_str(), &stat) != 0)
		return -1;
	return stat.st_mtime;
}
