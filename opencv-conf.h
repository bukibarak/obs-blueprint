#define CUDA_AVAILABLE 1

#if CUDA_AVAILABLE
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#else
#include <opencv2/core.hpp>
#include <opencv2/cudaimgproc.hpp>
#endif
