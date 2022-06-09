#pragma once

#include <yuni/yuni.h>
#include <yuni/string.h>

#include "info.h"
#include "i_writer.h"

namespace Benchmarking
{
class ContentHandler final
{
public:
    void append(const Yuni::ShortString256& text, Yuni::sint64 duration_ms);

    using iterator = TimeInfoMapType::iterator;
    iterator begin();
    iterator end();

    using const_iterator = TimeInfoMapType::const_iterator;
    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    TimeInfoMapType mInfoMap;
};
} // namespace Benchmarking
