#include "node-color-source.h"

#include "Structs/obs-frame.h"

NodeColorSource::NodeColorSource() : OBSBlueprintNode(obs_module_text("NodeColorSource"))
{
	pinWidth = createInputPin(INT_PIN, int32_t(), "width");
	pinHeight = createInputPin(INT_PIN, int32_t(), "height");
	pinColor = createInputPin(COLOR_PIN, uint32_t(), "color");

	pinVideo = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "video");
}

NodeColorSource::NodeColorSource(const int32_t& defaultWidth, const int32_t& defaultHeight,
	const uint32_t& defaultColor): NodeColorSource()
{
	pinWidth->setValue(defaultWidth);
	pinHeight->setValue(defaultHeight);
	pinColor->setValue(defaultColor);
}

void NodeColorSource::execute(float deltaSeconds)
{
	const int32_t& pinWValue = pinWidth->getValue<int32_t>();
	const int32_t& pinHValue = pinHeight->getValue<int32_t>();
	const uint32_t& pinCValue = pinColor->getValue<uint32_t>();

	const OBSFrame& frame = pinVideo->getValue<OBSFrame>();

	if (pinWValue != frame.width() || pinHValue != frame.height() || pinCValue != color) {
		// Size of the frame matrix image have changed, recreate
		int32_t newWidth, newHeight;
		color = pinCValue;

		if (pinWValue < 0 || pinHValue < 0) {
			GError("Color source cannot have a negative width or height! Will not output anything...");
			newWidth = 0;
			newHeight = 0;
		}
		else {
			newWidth = pinWValue;
			newHeight = pinHValue;
		}

		pixel p = pixel::ColorToPixel(color);
		pinVideo->setValue(OBSFrame({newHeight, newWidth, CV_8UC4, cv::Scalar(p.b, p.g, p.r, p.a)}));
		haveExecutedThisCycle = true;
	}
	else {
		pinVideo->getValuePtr<OBSFrame>()->updated = false;
	}
}
