#pragma once

#include <yuni/yuni.h>
#include <yuni/string.h>

#include <map>

namespace TimeElapsed
{
struct Info
{
    Info(unsigned int duration_ms, unsigned int nCalls) : duration_ms_(duration_ms), numberOfCalls_(nCalls)
    {
    }
    unsigned int duration_ms_ = 0;
    unsigned int numberOfCalls_ = 0;
};

using InfoMapType = std::map<Yuni::ShortString256, Info>;
} // namespace TimeElapsed
