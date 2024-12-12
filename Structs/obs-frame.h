#pragma once

#include "Nodes/Video/node-color-source.h"
#include "opencv2/core/types_c.h"
#include "opencv-conf.h"

/**
 * Video frame structure, containing the screen pixels matrix.\n
 * Used with pin of type \c VIDEO_PIN
 * @see PinType.
 */
struct OBSFrame {

	OBSFrame() = default;

	bool updated = true;
	static const cv::Mat EmptyMat;
	static const OBSFrame EmptyFrame;

	bool empty() const {return mat.empty(); }

#if CUDA_AVAILABLE
	static const cv::cuda::GpuMat EmptyGpuMat;

	cv::cuda::GpuMat mat;
	OBSFrame(const cv::cuda::GpuMat& mat) : mat(mat) {}
	OBSFrame(int width, int height, uint8_t* cpuData, gs_color_format format) : mat(EmptyGpuMat)
	{
		cv::Mat cpuMat(height, width, CV_8UC4, cpuData);
		mat = cv::cuda::GpuMat(cpuMat);
		if (format == GS_BGRA) {} // Nothing to do in this case
		else if (format == GS_RGBA) {
			cv::cuda::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
		}
		else {
			GError("Unsupported frame format (%i). Formats supported are RGBA (%i) and BGRA (%i)", format, GS_RGBA, GS_BGRA);
		}
	}

	cv::Mat cpuMat() const { return mat.empty() ? EmptyMat : cv::Mat(mat); }

#else
	cv::Mat mat;
	OBSFrame(cv::Mat mat) : mat(std::move(mat)) {}
	OBSFrame(int width, int height, uint8_t* data, gs_color_format format) : mat(EmptyMat)
	{
		if (format == GS_BGRA || format == GS_RGBA) {
			mat = cv::Mat(height, width, CV_8UC4, data).clone();
			if (format == GS_RGBA) {
				cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
			}
		}
		else {
			GError("Unsupported frame format (%i). Formats supported are RGBA (%i) and BGRA (%i)", format, GS_RGBA, GS_BGRA);
		}
	}

	cv::Mat cpuMat() const { return mat.empty() ? EmptyMat : mat;}
#endif

	const int& width() const { return mat.cols; }
	const int& height() const { return mat.rows; }
	// uchar* data() const {return mat.data;}
};
