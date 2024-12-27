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
	const int32_t& w = pinWidth->getValue<int32_t>();
	const int32_t& h = pinHeight->getValue<int32_t>();
	const uint32_t& c = pinColor->getValue<uint32_t>();

	const OBSFrame& frame = pinVideo->getValue<OBSFrame>();

	// Check if pixel matrix needs to be updated
	if (w != frame.width() || h != frame.height() || c != color) {
		if (w <= 0 || h <= 0) {
			// width and height must be > 0, otherwise it's just empty frame
			if (!pinVideo->getValue<OBSFrame>().empty()) {
				pinVideo->setValue(OBSFrame::EmptyFrame);
				haveExecutedThisCycle = true;
			}
		}
		else {
			color = c;
			pixel p = pixel::ColorToPixel(color);
			cv::UMat m{h, w, CV_8UC4, cv::Scalar(p.b, p.g, p.r, p.a)};
			pinVideo->setValue(OBSFrame(m));
			haveExecutedThisCycle = true;
		}
	}
	else if (!frame.empty()) {
		pinVideo->getValuePtr<OBSFrame>()->updated = false;
	}
}
