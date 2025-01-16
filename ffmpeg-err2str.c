#include "ffmpeg-err2str.h"
#include <libavutil/error.h>


const char* cpp_err2str(int errnum)
{
	return av_err2str(errnum);
}
