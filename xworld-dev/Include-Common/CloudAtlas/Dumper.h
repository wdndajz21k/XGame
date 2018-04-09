#pragma once

#ifdef _WIN32

long __stdcall XExceptionHander(_EXCEPTION_POINTERS* excp);			//全局异常处理捕获

void InitDumper(const char *identity);

#endif