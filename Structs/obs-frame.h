#pragma once

#include "dshowcapture.hpp"
#include <obs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "Helpers/global-logger.h"

namespace FrameFormat {
	enum ColorFormat {
		BGRA, // default, raw
		BGR, // NOT TESTED
		RGB, // NOT TESTED
		RGBA, // OK
		GRAY, // NOT TESTED
		BGR565, // NOT TESTED
		BGR555, // NOT TESTED
		NV12, // OK
		YV12, // NOT TESTED
		NV21, // NOT TESTED
		IYUV, // NOT TESTED
		UYVY, // NOT TESTED
		YUY2, // OK
		YVYU, // NOT TESTED
	};

	static const std::unordered_map<ColorFormat, int> Type {
		{BGRA, CV_8UC4},
		{BGR, CV_8UC3},
		{RGB, CV_8UC3},
		{RGBA, CV_8UC4},
		{GRAY, CV_8UC1},
		{BGR565, CV_16UC3},
		{NV12, CV_8UC1},
		{YV12, CV_8UC1},
		{NV21, CV_8UC2},
		{IYUV, CV_8UC2},
		{UYVY, CV_8UC2},
		{YUY2, CV_8UC2},
		{YVYU, CV_8UC2},
	};

	static const std::unordered_map<ColorFormat, cv::ColorConversionCodes> Converter {
		{BGR, cv::COLOR_BGR2RGBA},
		{RGB, cv::COLOR_RGB2RGBA},
		{RGBA, cv::COLOR_RGBA2BGRA},
		{GRAY, cv::COLOR_GRAY2BGRA},
		{BGR565, cv::COLOR_BGR5652BGRA},
		{BGR555, cv::COLOR_BGR5552BGRA},
		{NV12, cv::COLOR_YUV2BGRA_NV12},
		{YV12, cv::COLOR_YUV2BGRA_YV12},
		{NV21, cv::COLOR_YUV2BGRA_NV21},
		{IYUV, cv::COLOR_YUV2BGRA_IYUV},
		{UYVY, cv::COLOR_YUV2BGRA_UYVY},
		{YUY2, cv::COLOR_YUV2BGRA_YUY2},
		{YVYU, cv::COLOR_YUV2BGRA_YVYU},
	};

	static const std::unordered_map<gs_color_format, ColorFormat> FromGsColorFormat {
		{GS_RGBA, RGBA},
		{GS_BGRA, BGRA},
	} ;

	static const std::unordered_map<DShow::VideoFormat, ColorFormat> FromVideoFormat {
		{DShow::VideoFormat::ARGB, RGBA},
		{DShow::VideoFormat::NV12, NV12},
		{DShow::VideoFormat::YV12, YV12},
		{DShow::VideoFormat::YVYU, YVYU},
		{DShow::VideoFormat::YUY2, YUY2},
		{DShow::VideoFormat::UYVY, UYVY},
	};
}




/**
 * Video frame structure, containing the screen pixels matrix.\n
 * Used with pin of type \c VIDEO_PIN
 * @see PinType.
 */
struct OBSFrame {

	OBSFrame() : mat(0, 0, CV_8UC4) {}
	//OBSFrame(const cv::UMat& mat) : mat(mat) { GDebug("OBS Frame COPY constructor");}
	OBSFrame(cv::UMat mat) : mat(std::move(mat)) {}

	OBSFrame(int width, int height, uint8_t* data, FrameFormat::ColorFormat format) : mat(height, width, CV_8UC4)
	{
		if (format == FrameFormat::BGRA) {
			cv::Mat(height, width, FrameFormat::Type.at(format), data).copyTo(mat);
		}
		else if (auto it = FrameFormat::Converter.find(format); it != FrameFormat::Converter.end()) {
			cv::UMat raw;
			cv::Mat temp{format == FrameFormat::NV12 || format == FrameFormat::YV12 ? static_cast<int>(height * 3.0/2.0) : height, width, FrameFormat::Type.at(format), data};
			temp.copyTo(raw);
			cv::cvtColor(raw, mat, it->second);
		}
		else {
			GError("Unsupported frame format (%d). Frame will be empty", format);
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
};
