#include "obs-frame.h"

const cv::Mat OBSFrame::EmptyMat{0, 0, CV_8UC4, nullptr};

#if CUDA_AVAILABLE
const OBSFrame OBSFrame::EmptyFrame{cv::cuda::GpuMat(0, 0, CV_8UC4)};
const cv::cuda::GpuMat OBSFrame::EmptyGpuMat{0,0,CV_8UC4, nullptr, CV_AUTO_STEP};
#else
const OBSFrame OBSFrame::EmptyFrame{0,0,nullptr, GS_BGRA};
#endif
