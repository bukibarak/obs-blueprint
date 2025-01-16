#pragma once
#include <unordered_map>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "graphics/graphics.h"
#include "dshowcapture.hpp"
#include "media-io/video-io.h"

namespace FrameFormat {
	enum PixelFormat : uint8_t {
		BGRA,	// OK (default, raw)
		BGR,	// NOT TESTED
		RGB,	// NOT TESTED
		RGBA,	// OK
		GRAY,	// NOT TESTED
		BGR565, // NOT TESTED
		BGR555, // NOT TESTED
		NV12,	// OK
		YV12,	// NOT TESTED
		NV21,	// NOT TESTED
		IYUV,	// OK
		UYVY,	// NOT TESTED
		YUY2,	// OK
		YVYU,	// NOT TESTED

		// List of synonyms/aliases
		YUV420sp = NV21,
		I420 = IYUV,
		YUV420p = YV12,
		Y422 = UYVY,
		UYNV = UYVY,
		YUYV = YUY2,
		YUNV = YUY2,

	};

	static const std::unordered_map<PixelFormat, int> Type {
		{BGRA, CV_8UC4},
		{BGR, CV_8UC3},
		{RGB, CV_8UC3},
		{RGBA, CV_8UC4},
		{GRAY, CV_8UC1},
		{BGR565, CV_16UC3},
		{NV12, CV_8UC1},
		{YV12, CV_8UC1},
		{NV21, CV_8UC1},
		{IYUV, CV_8UC1},
		{UYVY, CV_8UC2},
		{YUY2, CV_8UC1},
		{YVYU, CV_8UC1},
	};

	static const std::unordered_map<PixelFormat, cv::ColorConversionCodes> Converter {
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

	static const std::unordered_map<gs_color_format, PixelFormat> FromGsColorFormat {
		{GS_RGBA, RGBA},
		{GS_BGRA, BGRA},
	} ;

	static const std::unordered_map<DShow::VideoFormat, PixelFormat> FromDShowFormat {
		{DShow::VideoFormat::ARGB, RGBA},
		{DShow::VideoFormat::I420, IYUV},
		{DShow::VideoFormat::NV12, NV12},
		{DShow::VideoFormat::YV12, YV12},
		{DShow::VideoFormat::YVYU, YVYU},
		{DShow::VideoFormat::YUY2, YUY2},
		{DShow::VideoFormat::UYVY, UYVY},
	};

	static const std::unordered_map<video_format, PixelFormat> FromVideoFormat {
		{VIDEO_FORMAT_I420, IYUV},
		//{VIDEO_FORMAT_I422, UYVY},
		{VIDEO_FORMAT_NV12, NV12},
		{VIDEO_FORMAT_YVYU, YVYU},
		{VIDEO_FORMAT_YUY2, YUY2},
		{VIDEO_FORMAT_UYVY, UYVY},
		{VIDEO_FORMAT_RGBA, RGBA},
		{VIDEO_FORMAT_BGRA, BGRA},
		{VIDEO_FORMAT_Y800, GRAY},
		{VIDEO_FORMAT_BGR3, BGR},
	};
}
