#pragma once
#include <unordered_map>

#include "Core/pin-type.h"
#include "Core/frame-format.h"

/**
 * Contains all strings (as <c>const char*</c>) for enumerations names.
 * TODO: Use Magic Enum instead ? https://github.com/Neargye/magic_enum --> require C++17
 */
namespace EnumStr {

inline std::unordered_map<FrameFormat::PixelFormat, const char*> PixelFormat = {
	{FrameFormat::BGRA,	"BGRA"},
	{FrameFormat::BGR, "BGR"},
	{FrameFormat::RGB, "RGB"},
	{FrameFormat::RGBA, "RGBA"},
	{FrameFormat::GRAY,	"GRAY"},
	{FrameFormat::BGR565, "BGR565"},
	{FrameFormat::BGR555, "BGR555"},
	{FrameFormat::NV12,	"NV12"},
	{FrameFormat::YV12,	"YV12 (420p)"},
	{FrameFormat::NV21,	"NV21 (420sp)"},
	{FrameFormat::IYUV,	"IYUV (I420)"},
	{FrameFormat::UYVY,	"UYVY (Y422, UYNV)"},
	{FrameFormat::YUY2,	"YUY2 (YUYV, YUNV)"},
	{FrameFormat::YVYU,	"YVYU"},
};

inline std::unordered_map<PinType, const char*> PinType {
	{UNKNOWN_PIN, "UNKNOWN_PIN"},
	{AUDIOVIDEO_PIN, "AUDIOVIDEO_PIN"},
	{AUDIO_PIN, "AUDIO_PIN"},
	{VIDEO_PIN,	"VIDEO_PIN"},
	{EXECUTION_PIN, "EXECUTION_PIN"},
	{BOOLEAN_PIN, "BOOLEAN_PIN"},
	{BYTE_PIN, "BYTE_PIN"},
	{INT_PIN, "INT_PIN"},
	{FLOAT_PIN, "FLOAT_PIN"},
	{CHAR_PIN, "CHAR_PIN"},
	{STRING_PIN, "STRING_PIN"},
	{COLOR_PIN,	"COLOR_PIN"},
	{ANY_PIN, "ANY_PIN"},
};

inline std::unordered_map<gs_color_format, const char *> gs_color_format = {
	{GS_UNKNOWN, "UNKNOWN"},
	{GS_A8, "A8"},
	{GS_R8, "R8"},
	{GS_RGBA, "RGBA"},
	{GS_BGRX, "BGRX"},
	{GS_BGRA, "BGRA"},
	{GS_R10G10B10A2, "R10G10B10A2"},
	{GS_RGBA16, "RGBA16"},
	{GS_R16, "R16"},
	{GS_RGBA16F, "RGBA16F"},
	{GS_RGBA32F, "RGBA32F"},
	{GS_RG16F, "RG16F"},
	{GS_RG32F, "RG32F"},
	{GS_R16F, "R16F"},
	{GS_R32F, "R32F"},
	{GS_DXT1, "DXT1"},
	{GS_DXT3, "DXT3"},
	{GS_DXT5, "DXT5"},
	{GS_R8G8, "R8G8"},
	{GS_RGBA_UNORM, "RGBA_UNORM"},
	{GS_BGRX_UNORM, "BGRX_UNORM"},
	{GS_BGRA_UNORM, "BGRA_UNORM"},
	{GS_RG16, "RG16"},
};

inline std::unordered_map<video_format, const char*> video_format = {
	{VIDEO_FORMAT_NONE, "NONE"},
	{VIDEO_FORMAT_I420, "I420"},
	{VIDEO_FORMAT_NV12, "NV12"},
	{VIDEO_FORMAT_YVYU, "YVYU"},
	{VIDEO_FORMAT_YUY2, "YUY2"},
	{VIDEO_FORMAT_UYVY, "UYVY"},
	{VIDEO_FORMAT_RGBA, "RGBA"},
	{VIDEO_FORMAT_BGRA, "BGRA"},
	{VIDEO_FORMAT_BGRX, "BGRX"},
	{VIDEO_FORMAT_Y800, "Y800"},
	{VIDEO_FORMAT_I444, "I444"},
	{VIDEO_FORMAT_BGR3, "BGR3"},
	{VIDEO_FORMAT_I422, "I422"},
	{VIDEO_FORMAT_I40A, "I40A"},
	{VIDEO_FORMAT_I42A, "I42A"},
	{VIDEO_FORMAT_YUVA, "YUVA"},
	{VIDEO_FORMAT_AYUV, "AYUV"},
	{VIDEO_FORMAT_I010, "I010"},
	{VIDEO_FORMAT_P010, "P010"},
	{VIDEO_FORMAT_I210, "I210"},
	{VIDEO_FORMAT_I412, "I412"},
	{VIDEO_FORMAT_YA2L, "YA2L"},
	{VIDEO_FORMAT_P216, "P216"},
	{VIDEO_FORMAT_P416, "P416"},
	{VIDEO_FORMAT_V210, "V210"},
	{VIDEO_FORMAT_R10L, "R10L"},
};

inline std::unordered_map<DShow::VideoFormat, const char*> VideoFormat = {
	{DShow::VideoFormat::Any, "Any"},
	{DShow::VideoFormat::Unknown, "Unknown"},
	{DShow::VideoFormat::ARGB, "ARGB (BGRA)"},
	{DShow::VideoFormat::XRGB, "XRGB (BGRX)"},
	{DShow::VideoFormat::RGB24, "RGB24"},
	{DShow::VideoFormat::I420, "I420"},
	{DShow::VideoFormat::NV12, "NV12"},
	{DShow::VideoFormat::YV12, "YV12"},
	{DShow::VideoFormat::Y800, "Y800"},
	{DShow::VideoFormat::P010, "P010"},
	{DShow::VideoFormat::YVYU, "YVYU"},
	{DShow::VideoFormat::YUY2, "YUY2"},
	{DShow::VideoFormat::UYVY, "UYVY"},
	{DShow::VideoFormat::HDYC, "HDYC"},
	{DShow::VideoFormat::MJPEG, "MJPEG"},
	{DShow::VideoFormat::H264, "H264"},
	{DShow::VideoFormat::HEVC, "HEVX"},
};

}
