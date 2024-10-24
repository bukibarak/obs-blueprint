#include "node-color-source.h"
#include "Structs/video-frame.h"

NodeColorSource::NodeColorSource() : OBSBlueprintNode(obs_module_text("NodeColorSource"))
{
	pinWidth = createInputPin(INT_PIN, int32_t(), "width");
	pinHeight = createInputPin(INT_PIN, int32_t(), "height");
	pinColor = createInputPin(COLOR_PIN, uint32_t(), "color");

	pinVideo = createOutputPin(VIDEO_PIN, video_frame(), "video");
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
	int32_t pinWValue = *pinWidth->getValuePtr<int32_t>();
	int32_t pinHValue = *pinHeight->getValuePtr<int32_t>();

	if(pinWValue != width || pinHValue != height || *pinColor->getValuePtr<uint32_t>() != color) {
		delete[] pixels;

		if(pinWValue < 0 || pinHValue < 0) {
			GError("Color source cannot have a negative width or height! Will not output anything...");
			width = 0;
			height = 0;
			pixels = nullptr;
		}
		else {
			// TODO OPTIMIZE TO NOT RECREATE FULL PIXEL ARRAY ON EACH TICK!
			width = pinWValue;
			height = pinHValue;
			color = *pinColor->getValuePtr<uint32_t>();

			pixels = new pixel[width * height];

			pixel defaultPixel;
			defaultPixel.r = color >> 0 & 0xFF;
			defaultPixel.g = color >> 8 & 0xFF;
			defaultPixel.b = color >> 16 & 0xFF;
			defaultPixel.a = color >> 24 & 0xFF;

			for(int32_t i = 0; i < width * height; i++) {
				pixels[i] = defaultPixel;
			}
		}

		video_frame* outputVideoPin = pinVideo->getValuePtr<video_frame>();
		outputVideoPin->width = width;
		outputVideoPin->height = height;
		outputVideoPin->pixels = pixels;

		haveExecutedThisCycle = true;
	}
}
