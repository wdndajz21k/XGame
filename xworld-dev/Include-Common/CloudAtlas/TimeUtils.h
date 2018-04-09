#pragma once

#include <ctime>

namespace XTimeUtils
{
    int GetTimeZoneOffset();
    int GetLocalDay(std::time_t timestamp);
} // XTimeUtils
