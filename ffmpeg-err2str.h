// #pragma once

// #ifdef __cplusplus
// extern "C" {
// #endif
//
// 	const char* cpp_av_err2str(int errnum);
//
// #ifdef __cplusplus
// }
// #endif

#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllexport) const char* cpp_err2str(int errnum);
#ifdef __cplusplus
}
#endif
