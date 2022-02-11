#pragma once

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <map>

namespace TimeElapsed
{
class Aggregator final
{
public:
    void append(const Yuni::ShortString256& text, Yuni::sint64 duration_ms);

private:
    struct Info
    {
        Info(Yuni::sint64 duration_ms, unsigned int nCalls) :
         duration_ms(duration_ms), nCalls(nCalls)
        {
        }
        Yuni::sint64 duration_ms = 0;
        unsigned int nCalls = 0;
    };

    std::map<Yuni::ShortString256, Info> mInfoMap;
    /* friend class TimeElapsedWriter; */
};
} // namespace TimeElapsed
