#pragma once

#pragma warning(disable:4127)

#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif

#ifdef _MSC_VER
    #define __THIS_FUNCTION__   __FUNCTION__
#endif

#ifdef __linux
    #define __THIS_FUNCTION__   __PRETTY_FUNCTION__
#endif

#define XFAILED_JUMP(Condition) \
    do  \
    {   \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)

#define XASSERT_JUMP(Condition) \
    do  \
    {   \
        if (!(Condition))   \
		{					\
			assert(false);	\
            goto Exit0;     \
		}					\
    } while (false)

#define XFAILED_RET_CODE(Condition, nCode) \
    do  \
    {   \
        if (!(Condition))   \
        {   \
            nResult = nCode;    \
            goto Exit0;         \
        }   \
    } while (false)

#define XSUCCESS_JUMP(Condition) \
    do  \
    {   \
        if (Condition)      \
            goto Exit1;     \
    } while (false)
	
#define XCOM_FAILED_JUMP(Condition) \
    do  \
    {   \
        if (FAILED(Condition))   \
            goto Exit0;     \
    } while (false)
	
#define XLOG_FAILED_JUMP(Condition) \
    do  \
    {   \
        if (!(Condition))   \
        {                   \
            ::LogError("XLOG_FAILED_JUMP($) at $:$ in $", #Condition, __FILE__, __LINE__, __THIS_FUNCTION__   \
            );              \
            goto Exit0;     \
        }                   \
    } while (false)

#define XLOG_FAILED(Condition) \
    do  \
    {   \
        if (!(Condition))   \
        {                   \
            ::LogError("XLOG_FAILED($) at $:$ in $", #Condition, __FILE__, __LINE__, __THIS_FUNCTION__); \
        }                   \
    } while (false)

#define XDELETE_ARRAY(pArray)     \
    do  \
    {   \
        if (pArray)                 \
        {                           \
            delete[] (pArray);      \
            (pArray) = NULL;        \
        }                           \
    } while (false)


#define XDELETE(p)    \
    do  \
    {   \
        if (p)              \
        {                   \
            delete (p);     \
            (p) = NULL;     \
        }                   \
    } while (false)

#define XFREE(pvBuffer) \
    do  \
    {   \
        if (pvBuffer)               \
        {                           \
            free(pvBuffer);         \
            (pvBuffer) = NULL;      \
        }                           \
    } while (false)


#define XCOM_RELEASE(pInterface) \
    do  \
    {   \
        if (pInterface)                 \
        {                               \
            (pInterface)->Release();    \
            (pInterface) = NULL;        \
        }                               \
    } while (false)

#define XFILE_CLOSE(p)    \
    do  \
    {   \
        if  (p) \
        {       \
            fclose(p);  \
            (p) = NULL; \
        }   \
    } while (false)

#define XFD_CLOSE(p)    \
    do  \
    {   \
        if  ((p) >= 0) \
        {       \
            close(p);  \
            (p) = -1; \
        }   \
    } while (false)


#define BIND0(FUNC) std::bind(&FUNC, this)
#define BIND1(FUNC) std::bind(&FUNC, this, std::placeholders::_1)
#define BIND2(FUNC) std::bind(&FUNC, this, std::placeholders::_1, std::placeholders::_2)
#define BIND3(FUNC) std::bind(&FUNC, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND4(FUNC) std::bind(&FUNC, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define BIND5(FUNC) std::bind(&FUNC, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)

struct XStringLess
{
    bool operator()(const char* pszX, const char* pszY) const
    {
        return strcmp(pszX, pszY) < 0;
    }
};

template <typename T>
using XStrMap = std::map<const char*, T, XStringLess>;

template <typename T>
void ClearStrMap(XStrMap<T>& strMap)
{
    std::vector<const char*> keys;
    for (auto& it : strMap)
    {
        keys.push_back(it.first);
    }
    strMap.clear();

    for (auto& p : keys)
    {
        delete p;
    }
}
