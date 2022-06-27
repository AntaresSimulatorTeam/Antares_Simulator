#pragma once

#include "info.h"

namespace TimeElapsed
{
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual void flush() = 0;
};
} // namespace TimeElapsed
