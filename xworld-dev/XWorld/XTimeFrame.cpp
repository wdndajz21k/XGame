#include "stdafx.h"
#include "XTimeFrame.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

enum XETimeFrameTempState
{
	emTimeFrameDefault = 0,
	emTimeFrameTempOpen,
	emTimeFrameTempClose,
};

XTimeFrame* TimeFrame = nullptr;

BEGIN_DECLARE_SCRIPT(XTimeFrame)
{
	LUA_FUNC(GetState);
    LUA_FUNC(GetStateByTime);
	LUA_FUNC(OpenTimeFrame);
	LUA_FUNC(CloseTimeFrame);
	LUA_FUNC(RestoreTimeFrame);
	LUA_FUNC(GetOpenTime);
	LUA_FUNC(GetOpenDay);
	LUA_FUNC(GetDomainDay);
}
END_DECLARE_SCRIPT()

static unsigned int CalcTime(unsigned int dwOpenTime, int nDay, int nTime)
{
	int		nHour = nTime / 100;
	int		nMinute = nTime % 100;
	unsigned int	dwRet = 0;
	unsigned int	dwTimeDiff = XTimeUtils::GetTimeZoneOffset();

	// 计算开服当天的0点
	dwOpenTime -= (dwOpenTime + dwTimeDiff) % (3600 * 24);

	// 开服的第一天实际上是第0天
	nDay--;

	dwRet = dwOpenTime +
		nDay * 24 * 3600 +
		nHour * 3600 +
		nMinute * 60;

	return dwRet;
}

bool XTimeFrame::Init()
{
	bool result = ReadTableFile<xtab_time_frame>("setting/timeframe.tab", TimeFrameMap, "time_frame");
	XLOG_FAILED_JUMP(result);

	result = true;

Exit0:
	return result;
}

bool XTimeFrame::GetState(int domain, const char* timeFrame)
{
	bool isOpen = false;
	if (TempStateMap.find(domain) != TempStateMap.end() && TempStateMap[domain].find(timeFrame) != TempStateMap[domain].end())
	{
		int nTempState = TempStateMap[domain][timeFrame];
		if (nTempState == emTimeFrameTempOpen)
			isOpen = true;
		else if (nTempState == emTimeFrameTempClose)
			isOpen = false;
	}
	else
	{
		auto it = TimeFrameMap.find(timeFrame);
		unsigned int serverOpenTime = GetDomainCreateTime(domain);
		XFAILED_JUMP(serverOpenTime);
		XFAILED_JUMP(it != TimeFrameMap.end());

		unsigned int openTime = CalcTime(serverOpenTime, it->second.open_day(), it->second.open_time());
		unsigned int closeTime = 0xFFFFFFFF;
		if (it->second.close_day() + it->second.close_time() > 0)
			closeTime = CalcTime(serverOpenTime, it->second.close_day(), it->second.close_time());

		isOpen = openTime < (unsigned int)AppHandle->GetTime() && closeTime >(unsigned int)AppHandle->GetTime();
	}
Exit0:
	return isOpen;
}

bool XTimeFrame::GetStateByTime(int domain, const char* timeFrame, int time)
{
    bool isOpen = false;
    if (TempStateMap.find(domain) != TempStateMap.end() && TempStateMap[domain].find(timeFrame) != TempStateMap[domain].end())
    {
        int nTempState = TempStateMap[domain][timeFrame];
        if (nTempState == emTimeFrameTempOpen)
            isOpen = true;
        else if (nTempState == emTimeFrameTempClose)
            isOpen = false;
    }
    else
    {
        auto it = TimeFrameMap.find(timeFrame);
        unsigned int serverOpenTime = GetDomainCreateTime(domain);
        XFAILED_JUMP(serverOpenTime);
        XFAILED_JUMP(it != TimeFrameMap.end());

        unsigned int openTime = CalcTime(serverOpenTime, it->second.open_day(), it->second.open_time());
        unsigned int closeTime = 0xFFFFFFFF;
        if (it->second.close_day() + it->second.close_time() > 0)
            closeTime = CalcTime(serverOpenTime, it->second.close_day(), it->second.close_time());

        isOpen = openTime < (unsigned int)time && closeTime >(unsigned int)time;
    }
Exit0:
    return isOpen;
}

bool XTimeFrame::OpenTimeFrame(int domain, const char* timeFrame)
{
	bool result = false;
	XFAILED_JUMP(TimeFrameMap.find(timeFrame) != TimeFrameMap.end());
	TempStateMap[domain][timeFrame] = emTimeFrameTempOpen;
Exit0:
	return result;
}

bool XTimeFrame::CloseTimeFrame(int domain, const char* timeFrame)
{
	bool result = false;
	XFAILED_JUMP(TimeFrameMap.find(timeFrame) != TimeFrameMap.end());
	TempStateMap[domain][timeFrame] = emTimeFrameTempClose;
Exit0:
	return result;
}

bool XTimeFrame::RestoreTimeFrame(int domain, const char* timeFrame)
{
	bool result = false;
	XFAILED_JUMP(TimeFrameMap.find(timeFrame) != TimeFrameMap.end());
	XFAILED_JUMP(TempStateMap[domain].find(timeFrame) != TempStateMap[domain].end());
	TempStateMap[domain].erase(timeFrame);
Exit0:
	return result;
}

M_LUA int XTimeFrame::GetOpenTime(int domainId, const char* timeFrame)
{
	return CalcOpenTime(domainId, timeFrame, false);
}

M_LUA int XTimeFrame::GetOpenDay(int domainId, const char* timeFrame)
{
	return CalcOpenTime(domainId, timeFrame, true);
}

M_LUA int XTimeFrame::GetDomainDay(int domainId)
{
	unsigned serverOpenTime = GetDomainCreateTime(domainId);
	if (!serverOpenTime)
		return 0;

	int open = XTimeUtils::GetLocalDay(serverOpenTime);
	int now = XTimeUtils::GetLocalDay(time(nullptr));
	return now - open + 1;
}

int XTimeFrame::CalcOpenTime(int domainId, const char* timeFrame, bool ignoreTime)
{
	if (timeFrame == nullptr)
		return 0;

	auto it = TimeFrameMap.find(timeFrame);
	if (it == TimeFrameMap.end())
		return 0;

	unsigned serverOpenTime = GetDomainCreateTime(domainId);
	if (!serverOpenTime)
		return 0;

	auto openTime = ignoreTime ? 0 : it->second.open_time();
	return CalcTime(serverOpenTime, it->second.open_day(), openTime);
}
