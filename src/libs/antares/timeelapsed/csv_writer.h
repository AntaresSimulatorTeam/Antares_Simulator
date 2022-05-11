#pragma once

#include <yuni/io/file.h>

#include "i_writer.h"
#include "info.h"

namespace TimeElapsed
{
class CSVWriter : public IWriter
{
public:
    CSVWriter() = default;
    bool setOutputFile(const Yuni::String& filename);
    void flush(const TimeInfoMapType& imt) override;

private:
    Yuni::IO::File::Stream mOutputFile;
};
} // namespace TimeElapsed
