#pragma once

#ifdef _WIN32

long __stdcall XExceptionHander(_EXCEPTION_POINTERS* excp);			//ȫ���쳣������

void InitDumper(const char *identity);

#endif