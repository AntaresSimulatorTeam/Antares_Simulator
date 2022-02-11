#pragma once

#include <yuni/yuni.h>
#include <yuni/string.h>

#include "info.h"
#include "i_writer.h"

namespace TimeElapsed
{
class Aggregator final
{
public:
    void append(const Yuni::ShortString256& text, Yuni::sint64 duration_ms);
    void flush(IWriter& writer);

private:
    InfoMapType mInfoMap;
};
} // namespace TimeElapsed
