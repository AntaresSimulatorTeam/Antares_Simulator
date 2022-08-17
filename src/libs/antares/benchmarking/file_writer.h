#pragma once

#include <yuni/yuni.h>
#include "file_content.h"

namespace Benchmarking
{
class FileWriter
{
public:
    FileWriter(FileContent& fileContent);
    virtual void saveToBuffer(std::string& buffer) const = 0;

protected:
    // Member data
    FileContent& fileContent_;
};

class iniFilewriter final : public FileWriter
{
public:
    explicit iniFilewriter(FileContent& fileContent);
    void saveToBuffer(std::string& buffer) const override;
};

} // namespace Benchmarking
