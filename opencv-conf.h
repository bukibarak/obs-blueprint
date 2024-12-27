#pragma once
#include "opencv2/opencv_modules.hpp"

#if defined(HAVE_OPENCV_CUDAARITHM) && defined(HAVE_OPENCV_CUDAIMGPROC) && defined(HAVE_OPENCV_CUDACODEC)
#define CUDA_AVAILABLE 0
#else
#define CUDA_AVAILABLE 0
#endif

#if CUDA_AVAILABLE
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudacodec.hpp>
#else
#include <opencv2/core.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/videoio.hpp>
#endif
