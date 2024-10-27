#include "global-logger.h"

#include <string>

void MyLogger::Info(const char *format, ...)
{
	va_list args;

	std::string message = "[OBS Blueprint] [INFO]  ";
	message += format;

	va_start(args, format);
	blogva(LOG_INFO, message.c_str(), args);
	va_end(args);
}
