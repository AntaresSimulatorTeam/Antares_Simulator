#pragma once

#include <yuni/io/file.h>

#include "i_writer.h"
#include "content_handler.h"

namespace Benchmarking
{
class CSVWriter : public IWriter
{
public:
    explicit CSVWriter(const Yuni::String& filename, ContentHandler* handler);
    void flush() override;

private:
    Yuni::IO::File::Stream mOutputFile;
    ContentHandler* contentHandler_ = nullptr;
};
} // namespace Benchmarking
