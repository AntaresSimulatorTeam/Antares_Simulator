#pragma once

#include "info.h"

namespace TimeElapsed
{
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual void flush(const InfoMapType& imt) = 0;
};
} // namespace TimeElapsed
