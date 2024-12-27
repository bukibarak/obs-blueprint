#include "wstring-converter.h"

std::string converter::to_string(const std::wstring &wstr)
{
	// Calculating the length of the multibyte string
	size_t len = wcstombs(nullptr, wstr.c_str(), 0) + 1;

	// Creating a buffer to hold the multibyte string
	char* buffer = new char[len];

	// Converting wstring to string
	wcstombs(buffer, wstr.c_str(), len);

	// Creating std::string from char buffer
	std::string str(buffer);

	// Cleaning up the buffer
	delete[] buffer;

	return str;
}
