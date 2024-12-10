#pragma once



#define CUDA_AVAILABLE 1



#include "Nodes/Video/node-color-source.h"
#include "opencv2/core/types_c.h"

#if CUDA_AVAILABLE
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#else
#include <opencv2/core.hpp>
#include <opencv2/cudaimgproc.hpp>
#endif

/**
 * Video frame structure, containing the screen pixels matrix.\n
 * Used with pin of type \c VIDEO_PIN
 * @see PinType.
 */
struct OBSFrame {

	OBSFrame() = default;

	static inline const cv::Mat EmptyFrame{0,0,CV_8UC4, nullptr};
	cv::Mat toMat() const { if (mat.empty()) return EmptyFrame; return cv::Mat(mat); }

#if CUDA_AVAILABLE
	cv::cuda::GpuMat mat;
	static inline const cv::cuda::GpuMat EmptyFrameGpu{0,0,CV_8UC4, nullptr, CV_AUTO_STEP};
	OBSFrame(const cv::cuda::GpuMat& mat) : mat(mat) {}
	OBSFrame(int width, int height, uint8_t* cpuData, gs_color_format format) : mat(EmptyFrameGpu)
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
#else
	cv::Mat mat;
	OBSFrame(cv::Mat mat) : mat(std::move(mat)) {}
	OBSFrame(int width, int height, uint8_t* data, gs_color_format format) : mat(EmptyFrame)
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
#endif

	const int& width() const { return mat.cols; }
	const int& height() const { return mat.rows; }
	// uchar* data() const {return mat.data;}
};
