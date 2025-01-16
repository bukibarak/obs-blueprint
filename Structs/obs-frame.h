#pragma once

#include "Core/frame-format.h"
#include "Helpers/enum-to-string.h"
#include "Helpers/global-logger.h"


/**
 * Video frame structure, containing the screen pixels matrix.\n
 * Used with pin of type \c VIDEO_PIN
 * @see PinType.
 */
struct OBSFrame {

	OBSFrame() : mat(0, 0, CV_8UC4) {}
	//OBSFrame(const cv::UMat& mat) : mat(mat) { GDebug("OBS Frame COPY constructor");}
	OBSFrame(cv::UMat mat) : mat(std::move(mat)) {}

	OBSFrame(int width, int height, uint8_t* data, FrameFormat::PixelFormat format) : mat(height, width, CV_8UC4)
	{
		if (format == FrameFormat::BGRA) {
			cv::Mat(height, width, FrameFormat::Type.at(format), data).copyTo(mat);
		}
		else if (auto it = FrameFormat::Converter.find(format); it != FrameFormat::Converter.end()) {
			cv::UMat raw;
			cv::Mat temp{(is_yuv(format) ? (3*height) / 2 : height), width, FrameFormat::Type.at(format), data};
			//cv::Mat TESTONLY(height, width, CV_8UC4);
			//cv::cvtColor(temp, TESTONLY, it->second); // Used to view variable as image with Rider debugger
			temp.copyTo(raw);
			cv::cvtColor(raw, mat, it->second);
		}
		else {
			GError("Unsupported frame format: %s. Frame will be empty", EnumStr::PixelFormat[format]);
		}
	}

	bool empty() const { return mat.empty(); }
	const int& width() const { return mat.cols; }
	const int& height() const { return mat.rows; }

	const cv::UMat& UMat() const {return mat;}
	cv::Mat getMat() const { return mat.empty() ? cv::Mat(0, 0, CV_8UC4) : mat.getMat(cv::ACCESS_READ); }

	bool updated = true;

	static const OBSFrame EmptyFrame;

private:
	cv::UMat mat;

	static bool is_yuv(FrameFormat::PixelFormat format)
	{
		switch (format) {
			case FrameFormat::NV12:
			case FrameFormat::YV12:
			case FrameFormat::NV21:
			case FrameFormat::IYUV:
			case FrameFormat::UYVY:
			case FrameFormat::YUY2:
			case FrameFormat::YVYU:
				return true;
			default:
				return false;
		}
	}
};
