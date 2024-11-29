#include "node-color-source.h"

#include "Helpers/pin-helper.h"
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
	const int32_t& pinWValue = pinWidth->getValue<int32_t>();
	const int32_t& pinHValue = pinHeight->getValue<int32_t>();
	const uint32_t& pinCValue = pinColor->getValue<uint32_t>();

	if(pinWValue != width || pinHValue != height || pinCValue != color) {
		delete[] pixels;
		pixels = nullptr;
		color = pinCValue;

		if(pinWValue < 0 || pinHValue < 0) {
			GError("Color source cannot have a negative width or height! Will not output anything...");
			width = 0;
			height = 0;
		}
		else {
			// TODO OPTIMIZE TO NOT RECREATE FULL PIXEL ARRAY ON EACH TICK!
			width = pinWValue;
			height = pinHValue;

			pixels = new pixel[width * height];

			pixel defaultPixel = pixel::ColorToPixel(color);

			for(int32_t i = 0; i < width * height; i++) {
				pixels[i] = defaultPixel;
			}
		}

		pinVideo->setValue(video_frame(width, height, pixels));

		haveExecutedThisCycle = true;
	}
}
