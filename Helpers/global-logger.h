#pragma once
#include <util/base.h>

#define GInfo(format, ...)  blog(LOG_INFO,    "[OBS Blueprint] [INFO]  " format, ##__VA_ARGS__)
#define GDebug(format, ...) blog(LOG_DEBUG,   "[OBS Blueprint] [DEBUG] " format, ##__VA_ARGS__)
#define GWarn(format, ...)  blog(LOG_WARNING, "[OBS Blueprint] [WARN]  " format, ##__VA_ARGS__)
#define GError(format, ...) blog(LOG_ERROR,   "[OBS Blueprint] [ERROR] " format, ##__VA_ARGS__)
