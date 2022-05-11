#pragma once

#include <yuni/yuni.h>
#include <yuni/string.h>

#include <map>

namespace TimeElapsed
{
struct TimeInfo
{
    TimeInfo(unsigned int duration_ms, unsigned int nCalls) : duration_ms_(duration_ms), numberOfCalls_(nCalls)
    {
    }
    Yuni::sint64 duration_ms_ = 0;
    unsigned int numberOfCalls_ = 0;
};

using TimeInfoMapType = std::map<Yuni::ShortString256, TimeInfo>;
} // namespace TimeElapsed
