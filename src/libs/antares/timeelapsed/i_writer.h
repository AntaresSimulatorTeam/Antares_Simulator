#pragma once

#include "info.h"

#include <yuni/string.h>

namespace TimeElapsed
{
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual bool checkAndPrepareOutput() = 0;
    virtual void flush(const InfoMapType& imt) = 0;
};
} // namespace TimeElapsed
