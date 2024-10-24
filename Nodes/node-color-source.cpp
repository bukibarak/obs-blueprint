#include "node-color-source.h"
#include "Structs/video-frame.h"

NodeColorSource::NodeColorSource()
{
	displayName = obs_module_text("NodeColorSource");

	pinWidth = createInputPin(INT_PIN, int32_t());
	pinHeight = createInputPin(INT_PIN, int32_t());
	pinColor = createInputPin(COLOR_PIN, uint32_t());

	pinVideo = createOutputPin(VIDEO_PIN, video_frame());
}

NodeColorSource::NodeColorSource(const int32_t& defaultWidth, const int32_t& defaultHeight,
	const uint32_t& defaultColor): NodeColorSource()
{
	pinWidth->setValue(defaultWidth);
	pinHeight->setValue(defaultHeight);
	pinColor->setValue(defaultColor);
}

NodeColorSource::~NodeColorSource()
{
	delete[] pixels;
}

void NodeColorSource::execute(float deltaSeconds)
{

	if(*pinWidth->getValuePtr<int32_t>() != width || *pinHeight->getValuePtr<int32_t>() != height || *pinColor->getValuePtr<uint32_t>() != color) {

		// TODO OPTIMIZE WITH PREVIOUS VALUE (IF ANY)


		width = *pinWidth->getValuePtr<int32_t>();
		height = *pinHeight->getValuePtr<int32_t>();
		color = *pinColor->getValuePtr<uint32_t>();

		delete[] pixels;
		pixels = new pixel[width * height];

		pixel defaultPixel;
		defaultPixel.r = color >> 0 & 0xFF;
		defaultPixel.g = color >> 8 & 0xFF;
		defaultPixel.b = color >> 16 & 0xFF;
		defaultPixel.a = color >> 24 & 0xFF;

		for(int32_t i = 0; i < width * height; i++) {
			pixels[i] = defaultPixel;
		}

		video_frame* outputVideoPin = pinVideo->getValuePtr<video_frame>();
		outputVideoPin->width = width;
		outputVideoPin->height = height;
		outputVideoPin->pixels = pixels;

		haveExecutedThisCycle = true;
	}
}
