#pragma once

class XPerformanceStat
{
public:
    struct XStatInfo
    {
        int64_t nCost;
        int64_t nCount;
        int64_t nPeakCost;
        time_t  nPeakTime;
    };

public:
    ~XPerformanceStat();

    void Reset();
    void SetName(const char cszName[], const char* pszIdentity = NULL);
    void Stat(const char* pszName, int64_t nTime);
    XStatInfo* GetStatInfo(const char* pszName);
    void Output(bool bUseTimestamp = false);

private:
    XStrMap<XStatInfo> m_CostTable;

    std::string m_strName;
    std::string m_strIdentity;
};

#define CALL_AND_STAT(line) \
    do  \
    {   \
        int64_t nTscTime = (int64_t)RDTSC();   \
        static auto s_pStat = PerformanceStat->GetStatInfo(#line);    \
        line;   \
        nTscTime = (int64_t)RDTSC() - nTscTime;    \
        s_pStat->nCost += nTscTime;  \
        s_pStat->nCount++;  \
        if (nTscTime > s_pStat->nPeakCost)  \
        {  \
            s_pStat->nPeakCost = nTscTime;  \
            s_pStat->nPeakTime = time(NULL);  \
        }  \
    } while (false)


#define BEGIN_STAT(desc)    \
    {   \
        int64_t nTscTime = (int64_t)RDTSC();  \
        static auto s_pStat = PerformanceStat->GetStatInfo(desc);

#define END_STAT()  \
        nTscTime = (int64_t)RDTSC() - nTscTime;    \
        s_pStat->nCost += nTscTime;  \
        s_pStat->nCount++;  \
        if (nTscTime > s_pStat->nPeakCost)  \
        {  \
            s_pStat->nPeakCost = nTscTime;  \
            s_pStat->nPeakTime = time(NULL);  \
        }  \
    }

#define BEGIN_STAT_EX(h, desc)    \
    {   \
        int64_t nTscTime = (int64_t)RDTSC();  \
        auto s_pStat = h->GetStatInfo(desc);

extern XPerformanceStat *PerformanceStat;

