#pragma once

#include <yuni/yuni.h>
#include <yuni/io/file/stream.h>

#include "file_content.h"

namespace Benchmarking
{
class FileWriter
{
public:
    FileWriter(FileContent& fileContent);
    virtual void flush() = 0;

protected:
    // Member data
    FileContent& fileContent_;
};

class iniFilewriter final : public FileWriter
{
public:
    explicit iniFilewriter(Yuni::String& filePath, FileContent& fileContent);
    void flush() override;
private:
    Yuni::String& filePath_;
};

} // namespace Benchmarking

