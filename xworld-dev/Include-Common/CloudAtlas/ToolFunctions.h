#pragma once

inline bool MakeDir(const char *dir)
{
    int nRetCode = 0;
#ifdef _MSC_VER
    nRetCode = _mkdir(dir);
#else
    nRetCode = mkdir(dir, 0777);
#endif
    return nRetCode != -1 || (errno == EEXIST);
}

#ifdef __cplusplus
template<typename T>
inline const T& Max(const T& a, const T& b)
{
    return (a < b) ? b : a;
}
template<typename T>
inline const T& Min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}
#else
#define Max(a,b)    (((a) > (b)) ? (a) : (b))
#define Min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

inline bool SafeCopyString(char* pszBuffer, size_t uBufferSize, const char* pszString)
{
    size_t  uLen  = strlen(pszString);

    if (uLen + 1 <= uBufferSize)
    {
        memcpy(pszBuffer, pszString, uLen + 1);
        return true;
    }

    return false;
}

template<size_t uBufferSize>
inline bool SafeCopyString(char (&szBuffer)[uBufferSize], const char* pszString)
{
    return SafeCopyString(szBuffer, uBufferSize, pszString);
}

inline bool SafeCopyString(wchar_t* pszBuffer, size_t uBufferSize, const wchar_t* pszString)
{
    size_t  uLen = wcslen(pszString);

    if (sizeof(wchar_t) * (uLen + 1) <= uBufferSize)
    {
        memcpy(pszBuffer, pszString, sizeof(wchar_t) * (uLen + 1));
        return true;
    }

    return false;
}

template<size_t uBufferSize>
inline bool SafeCopyString(wchar_t (&szBuffer)[uBufferSize], const wchar_t* pszString)
{
    return SafeCopyString(szBuffer, uBufferSize, pszString);
}

inline void* MemDup(const void* pvData, size_t uDataLen)
{
    void* pvBuffer = malloc(uDataLen);
    if (pvBuffer)
    {
        memcpy(pvBuffer, pvData, uDataLen);
    }
    return pvBuffer;
}

#ifdef _MSC_VER
inline struct tm* localtime_r(const time_t* timep, struct tm* result)
{
    errno_t nErr = localtime_s(result, timep);

    return (nErr == 0) ? result : NULL;
}
#endif

// 加密数据，再次调用解密数据
inline void EncryptData(char* pbyData, size_t uDataLen, uint64_t uBaseOffset)
{
    for (size_t i = 0; i < uDataLen; i++)
    {
        pbyData[i] ^= (char)(uBaseOffset + i);
    }
}
