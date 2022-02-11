#include "aggregator.h"

namespace TimeElapsed
{
void Aggregator::append(const Yuni::ShortString256& text, Yuni::sint64 duration_ms)
{
    auto res = mInfoMap.find(text);
    if (res != mInfoMap.end())
    {
        // Found !
        res->second.duration_ms += duration_ms;
        res->second.nCalls++;
    }
    else
    {
        mInfoMap.emplace(text, Info(duration_ms, 1));
    }
}
} // namespace TimeElapsed
