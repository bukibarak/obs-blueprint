#pragma once

#include "Core/frame-format.h"

/**
 * Video frame structure, containing the screen pixels matrix.\n
 * <b>For compatibility between nodes, all frames are converted to BGRA (ARGB) pixel format!</b> \n\n
 * Used with pin of type \c VIDEO_PIN
 * @see PinType.
 */
struct OBSFrame {

	/**
	 * Create a new empty frame. \n\n
	 * \b NOTE: It is recommended to use \c OBSFrame::EmptyFrame instead.
	 */
	OBSFrame();

	//OBSFrame(const cv::UMat& mat) : mat(mat) { GDebug("OBS Frame COPY constructor");}

	/**
	 * Create a new frame from an existing \c UMat \n
	 * Used with node that perform pixel matrix operations, i.e.: \c NodeVideoLayer
	 * @see cv::UMat
	 * @param mat The pixel matrix. <b>MUST be in BGRA (ARGB) pixel format!</b>
	 */
	OBSFrame(cv::UMat mat);

	/**
	 * Create a new frame from existing data.\n
	 * Frame data will be automatically converted to BGRA (ARGB) pixel format.
	 * @param width Frame width (in pixels)
	 * @param height Frame height (in pixels)
	 * @param data Frame data, continuous/packed
	 * @param format Input data format. If not BGRA (ARGB), will be converted using \c cv::cvtColor
	 */
	OBSFrame(int width, int height, uint8_t* data, FrameFormat::PixelFormat format);

	bool empty() const;
	const int& width() const;
	const int& height() const;

	/**
	 * Get the \c UMat related to this frame. \n
	 * \b Warning: data may be stored on GPU RAM.
	 * @return The frame \c cv::UMat related
	 */
	const cv::UMat& UMat() const;

	/**
	 * Convert the \c UMat into a \c Mat with data stored on CPU RAM.
	 * @return The converted \c cv::UMat related
	 */
	cv::Mat getMat() const;

	/** Indicate if the frame was updated since last tick. Useful for performance improvement. */
	bool updated = true;

	/** A frame containing no data. */
	static const OBSFrame EmptyFrame;

private:
	cv::UMat umat;

	static bool isYUV(const FrameFormat::PixelFormat& format);
};
