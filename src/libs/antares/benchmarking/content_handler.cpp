#include "content_handler.h"

namespace Benchmarking
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

ContentHandler::iterator ContentHandler::begin()
{
    return mInfoMap.begin();
}

ContentHandler::iterator ContentHandler::end()
{
    return mInfoMap.end();
}

ContentHandler::const_iterator ContentHandler::cbegin() const
{
    return mInfoMap.cbegin();
}

ContentHandler::const_iterator ContentHandler::cend() const
{
    return mInfoMap.cend();
}

} // namespace Benchmarking
