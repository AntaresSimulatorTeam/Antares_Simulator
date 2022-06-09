#pragma once

#include "info.h"

namespace Benchmarking
{
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual void flush() = 0;
};
} // namespace Benchmarking
