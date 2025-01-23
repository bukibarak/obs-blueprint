#include "obs-frame.h"
#include "Helpers/enum-to-string.h"
#include "Helpers/global-logger.h"

const OBSFrame OBSFrame::EmptyFrame{};

OBSFrame::OBSFrame()  : umat(0, 0, CV_8UC4) {}

OBSFrame::OBSFrame(cv::UMat mat) : umat(std::move(mat)) {}

OBSFrame::OBSFrame(int width, int height, uint8_t *data, FrameFormat::PixelFormat format) : umat(height, width, CV_8UC4)
{
	if (format == FrameFormat::BGRA) {
		cv::Mat(height, width, FrameFormat::Type.at(format), data).copyTo(umat);
	}
	else if (auto it = FrameFormat::Converter.find(format); it != FrameFormat::Converter.end()) {
		cv::UMat raw;
		cv::Mat temp{(isYUV(format) ? heightYUV(height, format) : height), width, FrameFormat::Type.at(format), data};
		//cv::Mat TESTONLY(height, width, CV_8UC4);
		//cv::cvtColor(temp, TESTONLY, it->second); // Used to view variable as image with Rider debugger
		temp.copyTo(raw);
		cv::cvtColor(raw, umat, it->second);
	}
	else {
		GError("Unsupported frame format: %s. Frame will be empty", EnumStr::PixelFormat[format]);
	}
}

bool OBSFrame::empty() const
{
	return umat.empty();
}

const int & OBSFrame::width() const
{
	return umat.cols;
}

const int & OBSFrame::height() const
{
	return umat.rows;
}

const cv::UMat & OBSFrame::UMat() const
{
	return umat;
}

cv::Mat OBSFrame::getMat() const
{
	return umat.empty() ? cv::Mat(0, 0, CV_8UC4) : umat.getMat(cv::ACCESS_READ);
}

bool OBSFrame::isYUV(const FrameFormat::PixelFormat& format)
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
	case FrameFormat::BGRA:
	case FrameFormat::BGR:
	case FrameFormat::RGB:
	case FrameFormat::RGBA:
	case FrameFormat::GRAY:
	case FrameFormat::BGR565:
	case FrameFormat::BGR555:
	default:
		return false;
	}
}

int OBSFrame::heightYUV(const int &height, const FrameFormat::PixelFormat &format)
{
	switch (format) {
	case FrameFormat::NV12:
	case FrameFormat::YV12:
	case FrameFormat::NV21:
	case FrameFormat::IYUV:
		return (3*height) / 2; // 12 bits per pixels
	case FrameFormat::UYVY:
	case FrameFormat::YUY2:
	case FrameFormat::YVYU:
		return height; // 16 bits per pixels
	case FrameFormat::BGRA:
	case FrameFormat::BGR:
	case FrameFormat::RGB:
	case FrameFormat::RGBA:
	case FrameFormat::GRAY:
	case FrameFormat::BGR565:
	case FrameFormat::BGR555:
	default:
			return height; // 8 bits per pixels
	}
}
