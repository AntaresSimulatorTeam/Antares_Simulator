#pragma once

#include <yuni/io/file.h>

#include "i_writer.h"
#include "info.h"

namespace TimeElapsed
{
class FileWriter : public IWriter
{
public:
    FileWriter() = default;
    bool setOutputFile(const Yuni::String& filename);
    void flush(const InfoMapType& imt) override;

private:
    Yuni::IO::File::Stream mOutputFile;
};
} // namespace TimeElapsed
