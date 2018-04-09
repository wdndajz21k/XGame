#pragma once

class XTimeFrame
{
    DECLARE_LUA_CLASS(XTimeFrame);
public:
	bool Init();
	bool GetState(int domainId, const char* timeFrame);
    bool GetStateByTime(int domainId, const char* timeFrame, int time);
	bool OpenTimeFrame(int domainId, const char* timeFrame);
	bool CloseTimeFrame(int domainId, const char* timeFrame);
	bool RestoreTimeFrame(int domainId, const char* timeFrame);
	int CalcOpenTime(int domainId, const char* timeFrame, bool ignoreTime);
	
	M_LUA int GetOpenTime(int domainId, const char* timeFrame);
	M_LUA int GetOpenDay(int domainId, const char* timeFrame);
	M_LUA int GetDomainDay(int domainId);
public:
    std::unordered_map<std::string, xtab_time_frame> TimeFrameMap;
    std::unordered_map<int, std::unordered_map<std::string, int>> TempStateMap;
};

extern XTimeFrame* TimeFrame;

unsigned int GetDomainCreateTime(int domain);
