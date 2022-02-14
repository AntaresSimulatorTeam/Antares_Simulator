#pragma once

#include <yuni/io/file.h>

#include "i_writer.h"
#include "info.h"

namespace TimeElapsed
{
class FileWriter : public IWriter
{
public:
    explicit FileWriter(const Yuni::String& filename);
    bool checkAndPrepareOutput() override;
    void flush(const InfoMapType& imt) override;

private:
    const Yuni::String mFilename;
    Yuni::IO::File::Stream mOutputFile;
};
} // namespace TimeElapsed
