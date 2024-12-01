#include "node-video-layer.h"
#include "Structs/video-frame.h"

NodeVideoLayer::NodeVideoLayer() : OBSBlueprintNode(obs_module_text("NodeVideoLayer"))
{
	foreground = createInputPin(VIDEO_PIN, video_frame(), "fg");
	background = createInputPin(VIDEO_PIN, video_frame(), "bg");

	result = createOutputPin(VIDEO_PIN, video_frame(), "result");
}

void NodeVideoLayer::execute(float deltaSeconds)
{
	if(!foreground->isConnected() && !background->isConnected()) {
		// Nothing connected to the node
		foreground->setValue(video_frame());
		background->setValue(video_frame());
		result->setValue(video_frame());
	}
	else if(!background->isConnected()) {
		// Only foreground connected, will just propagate as is
		background->setValue(video_frame());
		result->setValue(foreground->getValue<video_frame>());
		haveExecutedThisCycle = true;
	}
	else if(!foreground->isConnected()) {
		// Only background connected, will just propagate as is
		foreground->setValue(video_frame());
		result->setValue(background->getValue<video_frame>());
		haveExecutedThisCycle = true;
	}
	else {
		// Both pin connected, perform layer transform
		video_frame* foregroundFrame = foreground->getValuePtr<video_frame>();
		video_frame* backgroundFrame = background->getValuePtr<video_frame>();

		uint32_t maxWidth = std::max(foregroundFrame->width, backgroundFrame->width);
		uint32_t maxHeight = std::max(foregroundFrame->height, backgroundFrame->height);

		delete[] resultData;
		resultData = new pixel[maxWidth * maxHeight];

		for(uint32_t h = 0; h < maxHeight; h++) {
			for(uint32_t w = 0; w < maxWidth; w++) {
				uint32_t foregroundIndex = h * foregroundFrame->width + w;
				uint32_t backgroundIndex = h * backgroundFrame->width + w;
				uint32_t resultIndex = h * maxWidth + w;
				pixel& resultPixel = resultData[resultIndex];
				if(w < foregroundFrame->width && h < foregroundFrame->height) {
					if(w < backgroundFrame->width && h < backgroundFrame->height) {
						pixel* foregroundPixel = &foregroundFrame->pixels[foregroundIndex];
						pixel* backgroundPixel = &backgroundFrame->pixels[backgroundIndex];
						if(foregroundPixel->a == UINT8_MAX || backgroundPixel->a == 0) {
							// Only foreground pixel
							resultPixel = *foregroundPixel;
						}
						else if(foregroundPixel->a == 0) {
							// Only background pixel
							resultPixel = *backgroundPixel;
						}
						else {
							// Combine pixel colors

							// METHOD 1 NOT GOOD
							// uint8_t a0 = foregroundPixel->a;
							// uint8_t a1 = backgroundPixel->a;
							// uint8_t left_part = (1 - a0) * a1; // (1 - a0)a1
							// uint8_t alpha = left_part + a1; // (1 - a0)a1 + a0
							// resultPixel.r = (left_part*backgroundPixel->r + a0*foregroundPixel->r) / alpha;
							// resultPixel.g = (left_part*backgroundPixel->g + a0*foregroundPixel->g) / alpha;
							// resultPixel.b = (left_part*backgroundPixel->b + a0*foregroundPixel->b) / alpha;
							// resultPixel.a = alpha;

							// METHOD 2 https://stackoverflow.com/questions/1944095/how-to-mix-two-argb-pixels
							uint8_t aA = foregroundPixel->a;
							uint8_t aB = backgroundPixel->a;
							uint8_t alpha = aA + (aB * (UINT8_MAX - aA) / UINT8_MAX);
							resultPixel.r = (foregroundPixel->r * aA + backgroundPixel->r * aB * (UINT8_MAX - aA) / UINT8_MAX) / alpha;
							resultPixel.g = (foregroundPixel->g * aA + backgroundPixel->g * aB * (UINT8_MAX - aA) / UINT8_MAX) / alpha;
							resultPixel.b = (foregroundPixel->b * aA + backgroundPixel->b * aB * (UINT8_MAX - aA) / UINT8_MAX) / alpha;
							resultPixel.a = alpha;
						}
					}
					else {
						// Only foreground pixel
						resultPixel = foregroundFrame->pixels[foregroundIndex];
					}
				}
				else if(w < backgroundFrame->width && h < backgroundFrame->height) {
					// Only background pixel
					resultPixel = backgroundFrame->pixels[backgroundIndex];
				}
				else {
					resultPixel = {};
				}
			}
		}

		result->setValue(video_frame(maxWidth, maxHeight, resultData));
		haveExecutedThisCycle = true;
	}
}
