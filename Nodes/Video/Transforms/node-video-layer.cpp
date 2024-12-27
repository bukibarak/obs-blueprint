#include "node-video-layer.h"

#include "Structs/obs-frame.h"

NodeVideoLayer::NodeVideoLayer() : OBSBlueprintNode(obs_module_text("NodeVideoLayer"))
{
	foreground = createInputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "fg");
	background = createInputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "bg");

	result = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "result");
}

void NodeVideoLayer::execute(float deltaSeconds)
{
	const OBSFrame& fgFrame = foreground->getValue<OBSFrame>();
	const OBSFrame& bgFrame = background->getValue<OBSFrame>();

	if(fgFrame.empty() && bgFrame.empty()) {
		// No frames given from input, result is empty frame as well
		if (!result->getValue<OBSFrame>().empty()) {
			result->setValue(OBSFrame::EmptyFrame);
			haveExecutedThisCycle = true;
		}
		previousState = Empty;
	}
	else if(bgFrame.empty()) {
		// Only foreground have frame, will juste propagate as is
		result->setValue(fgFrame);
		if (previousState != FgOnly) {
			result->getValuePtr<OBSFrame>()->updated = true;
			haveExecutedThisCycle = true;
		}
		previousState = FgOnly;
	}
	else if(fgFrame.empty()) {
		// Only background have frame, will just propagate as is
		result->setValue(bgFrame);
		if (previousState != BgOnly) {
			result->getValuePtr<OBSFrame>()->updated = true;
			haveExecutedThisCycle = true;
		}
		previousState = BgOnly;
	}
	else {
		// Both pin have frame, perform matrix transform
		// See: https://en.wikipedia.org/wiki/Alpha_compositing
		previousState = Blend;

		if (fgFrame.updated || bgFrame.updated) {
			int width = std::max(fgFrame.width(), bgFrame.width());
			int height = std::max(fgFrame.height(), bgFrame.height());

			std::vector<cv::UMat> fg; // BGRA Channels of the foreground image
			std::vector<cv::UMat> bg; // BGRA Channels of the backgournd image
			std::vector<cv::UMat> res; // BGRA Channels of the result image
			fg.resize(4); bg.resize(4); res.resize(4);

			if (fgFrame.width() == width && fgFrame.height() == height) {
				cv::split(fgFrame.UMat(), fg);
			}
			else {
				cv::UMat expanded(height, width, CV_8UC4, cv::Scalar::all(0));
				fgFrame.UMat().copyTo(expanded(cv::Rect(0,0,fgFrame.width(), fgFrame.height())));
				cv::split(expanded, fg);
			}
			if (bgFrame.width() == width && bgFrame.height() == height) {
				cv::split(bgFrame.UMat(), bg);
			}
			else {
				cv::UMat expanded(height, width, CV_8UC4, cv::Scalar::all(0));
				bgFrame.UMat().copyTo(expanded(cv::Rect(0,0,bgFrame.width(), bgFrame.height())));
				cv::split(expanded, bg);
			}

			cv::UMat alphaFgNorm;
			cv::UMat alphaBgNorm;
			cv::UMat alphaResNorm;
			cv::UMat oneMinusAlpha;

			cv::divide(fg[3], 255.0, alphaFgNorm);
			cv::divide(bg[3], 255.0, alphaBgNorm);
			cv::subtract(1.0, alphaFgNorm, oneMinusAlpha);
			cv::multiply(oneMinusAlpha, alphaBgNorm, alphaBgNorm);
			cv::add(alphaFgNorm, alphaBgNorm, alphaResNorm);

			for (int channel = 0; channel < 3; channel++) {
				cv::UMat left;
				cv::UMat right;
				cv::multiply(alphaFgNorm, fg[channel], left);
				cv::multiply(alphaBgNorm, bg[channel], right);
				cv::add(left, right, right);
				cv::divide(right, alphaResNorm, res[channel]);
			}

			cv::multiply(alphaResNorm, 255, res[3]);
			cv::UMat resultMat;
			cv::merge(res, resultMat);
			result->setValue(OBSFrame(resultMat));
			result->getValuePtr<OBSFrame>()->updated = true;
			haveExecutedThisCycle = true;
		}
		else {
			result->getValuePtr<OBSFrame>()->updated = false;
		}
	}
}

void NodeVideoLayer::onGraphBeginTick(float deltaSeconds)
{
	if (!foreground->isConnected()) {
		foreground->setValue(OBSFrame::EmptyFrame);
	}
	if (!background->isConnected()) {
		background->setValue(OBSFrame::EmptyFrame);
	}
}
