#pragma once

// This C/C++ interface is just to compile macro av_err2str with C99 compiler, because with C++ compiler we got error:
// C4576: "a parenthesized type followed by an initializer list is a non-standard explicit type conversion syntax"

#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllexport) const char* cpp_err2str(int errnum);
#ifdef __cplusplus
}
#endif
