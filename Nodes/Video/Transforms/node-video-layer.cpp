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
	if(!foreground->isConnected() && !background->isConnected()) {
		// Nothing is connected to the node
		if (fgConnected || bgConnected) {
			foreground->setValue(OBSFrame::EmptyFrame);
			background->setValue(OBSFrame::EmptyFrame);
			result->setValue(OBSFrame::EmptyFrame);
			haveExecutedThisCycle = true;
		}
	}
	else if(!background->isConnected()) {
		// Only foreground connected, will just propagate as is
		const OBSFrame& fgFrame = foreground->getValue<OBSFrame>();
		if (bgConnected || !fgConnected || fgFrame.updated) {
			background->setValue(OBSFrame::EmptyFrame);
			result->setValue(fgFrame);
			haveExecutedThisCycle = true;
		}
	}
	else if(!foreground->isConnected()) {
		// Only background connected, will just propagate as is
		const OBSFrame& bgFrame = background->getValue<OBSFrame>();
		if (fgConnected || !bgConnected || bgFrame.updated) {
			foreground->setValue(OBSFrame::EmptyFrame);
			result->setValue(bgFrame);
			haveExecutedThisCycle = true;
		}
	}
	else {
		// Both pin connected, perform matrix transform
		// See: https://en.wikipedia.org/wiki/Alpha_compositing
		OBSFrame* fgFrame = foreground->getValuePtr<OBSFrame>();
		OBSFrame* bgFrame = background->getValuePtr<OBSFrame>();


			if (!fgConnected || fgFrame->updated || !bgConnected || bgFrame->updated) {
				int width = std::max(fgFrame->width(), bgFrame->width());
				int height = std::max(fgFrame->height(), bgFrame->height());

#if CUDA_AVAILABLE
				cv::cuda::GpuMat fg[4];
				cv::cuda::GpuMat bg[4];
				cv::cuda::GpuMat res[4];

				if (fgFrame->width() == width && fgFrame->height() == height) {
					cv::cuda::split(fgFrame->mat, fg);
				}
				else {
					cv::cuda::GpuMat expanded(height, width, CV_8UC4, cv::Scalar::all(0));
					if (!fgFrame->empty()) // TODO BETTER pass bg frame as is if fg frame is empty and do not count as executed this cycle
						fgFrame->mat.copyTo(expanded(cv::Rect(0,0,fgFrame->width(), fgFrame->height())));
					cv::cuda::split(expanded, fg);
				}
				if (bgFrame->width() == width && bgFrame->height() == height) {
					cv::cuda::split(bgFrame->mat, bg);
				}
				else {
					cv::cuda::GpuMat expanded(height, width, CV_8UC4, cv::Scalar::all(0));
					if (!bgFrame->empty()) // TODO BETTER pass bg frame as is if fg frame is empty and do not count as executed this cycle
						bgFrame->mat.copyTo(expanded(cv::Rect(0,0,bgFrame->width(), bgFrame->height())));
					cv::cuda::split(expanded, bg);
				}

				cv::cuda::GpuMat alphaFgNorm;
				cv::cuda::GpuMat  alphaBgNorm;
				cv::cuda::GpuMat alphaResNorm;
				cv::cuda::GpuMat oneMinusAlpha;

				cv::cuda::divide(fg[3], 255.0, alphaFgNorm);
				cv::cuda::divide(bg[3], 255.0, alphaBgNorm);
				cv::cuda::subtract(1.0, alphaFgNorm, oneMinusAlpha);
				cv::cuda::multiply(oneMinusAlpha, alphaBgNorm, alphaBgNorm);
				cv::cuda::add(alphaFgNorm, alphaBgNorm, alphaResNorm);

				for (int channel = 0; channel < 3; channel++) {
					cv::cuda::GpuMat left;
					cv::cuda::GpuMat right;
					cv::cuda::multiply(alphaFgNorm, fg[channel], left);
					cv::cuda::multiply(alphaBgNorm, bg[channel], right);
					cv::cuda::add(left, right, right);
					cv::cuda::divide(right, alphaResNorm, res[channel]);
				}

				cv::cuda::multiply(alphaResNorm, 255, res[3]);
				cv::cuda::GpuMat resultMat;
				cv::cuda::merge(res, 4, resultMat);
				result->setValue(OBSFrame(resultMat));

#else

				cv::Mat fg[4]; // BGRA Channels of the foreground image
				cv::Mat bg[4]; // BGRA Channels of the backgournd image
				cv::Mat res[4]; // BGRA Channels of the result image

				if (fgFrame->width() == width && fgFrame->height() == height) {
					cv::split(fgFrame->mat, fg);
				}
				else {
					const OBSFrame& test = OBSFrame::EmptyFrame;
					bool updated = test.updated;
					cv::Mat expanded(height, width, CV_8UC4, cv::Scalar::all(0));
					if (!fgFrame->empty()) // TODO BETTER pass bg frame as is if fg frame is empty and do not count as executed this cycle
						fgFrame->mat.copyTo(expanded(cv::Rect(0,0,fgFrame->width(), fgFrame->height())));
					cv::split(expanded, fg);
				}
				if (bgFrame->width() == width && bgFrame->height() == height) {
					cv::split(bgFrame->mat, bg);
				}
				else {
					cv::Mat expanded(height, width, CV_8UC4, cv::Scalar::all(0));
					if (!bgFrame->empty()) // TODO BETTER pass bg frame as is if fg frame is empty and do not count as executed this cycle
						bgFrame->mat.copyTo(expanded(cv::Rect(0,0,bgFrame->width(), bgFrame->height())));
					cv::split(expanded, bg);
				}

				auto alphaFgNorm = fg[3] / 255.0;
				auto alphaBgNorm = bg[3] / 255.0;
				auto mulPart =  alphaBgNorm.mul(1 - alphaFgNorm);
				auto alphaResNorm = alphaFgNorm + mulPart;

				for (int channel = 0; channel < 3; channel++) {
					cv::divide(fg[channel].mul(alphaFgNorm) + bg[channel].mul(mulPart), alphaResNorm, res[channel]);
				}

				res[3] = alphaResNorm * 255.0;
				cv::Mat resultMat;
				cv::merge(res, 4, resultMat);
				result->setValue(OBSFrame(resultMat));
#endif

				haveExecutedThisCycle = true;
			}
		}

	fgConnected = foreground->isConnected();
	bgConnected = background->isConnected();
	result->getValuePtr<OBSFrame>()->updated = haveExecutedThisCycle;
}
