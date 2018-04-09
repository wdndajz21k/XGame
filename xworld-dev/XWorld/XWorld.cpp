#include "stdafx.h"
#include "XWorld/XWorld.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

struct XStatLog
{
    tm Time;
    FILE* FD;
};

std::unordered_map<std::string, XStatLog> StatLogMap;

double XPoint::DistanceTo(int x, int y)
{
    return GetDistance(X, Y, x, y);
}

double XPoint::DistanceTo(const XPoint& point)
{
    return GetDistance(X, Y, point.X, point.Y);
}

double GetDistance(int x1, int y1, int x2, int y2)
{
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

double GetDistanceSquare(int x1, int y1, int x2, int y2)
{
	double a = x1 - x2;
	double b = y1 - y2;
	return a * a + b * b;
}

double GetDistance(const XPoint& point1, const XPoint& point2)
{
    return GetDistance(point1.X, point1.Y, point2.X, point2.Y);
}

void Normalize(float& x, float& y)
{
    if (std::abs(x) > std::numeric_limits<float>::epsilon() || y > std::numeric_limits<float>::epsilon())
    {
        float dist = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
        x = x / dist;
        y = y / dist;
    }
}

static FILE* OpenStatLog(const std::string& logType)
{
    time_t timeNow = time(NULL);
    tm* tmNow = localtime(&timeNow);
    auto it = StatLogMap.find(logType);
    if (it != StatLogMap.end())
    {
        tm lastTime;
        lastTime = it->second.Time;
        if (tmNow->tm_year == lastTime.tm_year && tmNow->tm_mon == lastTime.tm_mon && tmNow->tm_mday == lastTime.tm_mday)
            return it->second.FD;
        else
            XFILE_CLOSE(it->second.FD);
    }

    char file[1024], path[1024];
    sprintf(path, "stat/%s/%2.2d%2.2d%2.2d", AppHandle->AppName.c_str(), tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday);
    sprintf(file, "%s/%s.log", path, logType.c_str());

    MakeDir("stat");
    MakeDir(XTool::Format("stat/$", AppHandle->AppName).c_str());
    MakeDir(path);

    FILE *fd = fopen(file, "a+");
    StatLogMap[logType] = { *tmNow, fd };
    return fd;
}

void CloseStatLog()
{
    for (auto& it : StatLogMap)
    {
        XFILE_CLOSE(it.second.FD);
    }
    StatLogMap.clear();
}

void InternalStatLog(const std::string& logType, const std::string& log)
{
    time_t timeNow = time(nullptr);
    tm* localTime = localtime(&timeNow);
    FILE* fd = OpenStatLog(logType);
    char buffer[8192];
    int length = sprintf(buffer, "[%d-%2.2d-%2.2d-%2.2d-%2.2d-%2.2d],%s",
        localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec, logType.c_str());

    fwrite(buffer, length, 1, fd);
    fwrite(log.c_str(), log.size(), 1, fd);
    fwrite("\n", 1, 1, fd);
    fflush(fd);

#if WIN32
    fprintf(stdout,
        "%2.2d-%2.2d %2.2d:%2.2d:%2.2d<%s>: ",
        localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec, logType.c_str());
    fwrite(log.substr(1).c_str(), log.size() - 1, 1, stdout);
    fprintf(stdout, "\n");
    fflush(stdout);
#endif
}
