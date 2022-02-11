#pragma once

#include <yuni/yuni.h>
#include <yuni/string.h>

#include <map>

namespace TimeElapsed
{
struct Info
{
    Info(Yuni::sint64 duration_ms, unsigned int nCalls) : duration_ms(duration_ms), nCalls(nCalls)
    {
    }
    Yuni::sint64 duration_ms = 0;
    unsigned int nCalls = 0;
};

using InfoMapType = std::map<Yuni::ShortString256, Info>;
} // namespace TimeElapsed
