#include "content_handler.h"

namespace TimeElapsed
{
void ContentHandler::append(const Yuni::ShortString256& text, Yuni::sint64 duration_ms)
{
    auto res = mInfoMap.find(text);
    if (res != mInfoMap.end())
    {
        // Found !
        res->second.duration_ms_ += duration_ms;
        res->second.numberOfCalls_++;
    }
    else
    {
        mInfoMap.emplace(text, TimeInfo(duration_ms, 1));
    }
}

void ContentHandler::flush(IWriter& writer) const
{
    writer.flush(mInfoMap);
}
} // namespace TimeElapsed
