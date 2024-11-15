#pragma once
#include <cstdint>

#include "pixel.h"

/**
 * Video frame structure, containing the screen size and pixels.\n
 * Used with pin of type \c VIDEO_PIN
 * @see PinType.
 */
struct video_frame {

	/**
	 * Create a new video frame with no data.
	 */
	video_frame() : width(0), height(0), pixels(nullptr) {}

	/**
	 * Create a new video frame with given data.
	 * @param px The frame width.
	 * @param py The frame height.
	 * @param data The frame pixel data array ptr. The array size must be \c width*height
	 */
	video_frame(const uint32_t& px, const uint32_t& py, pixel* data) : width(px), height(py), pixels(data) {}


	/** The frame width. */
	uint32_t width;
	/** The frame height. */
	uint32_t height;
	/** The frame pixels array ptr. Size is width*height. */
	pixel* pixels;
};
