#pragma once

#include <string>
#include <yuni/core/string.h>

#include "i_writer.h"

namespace Antares
{
namespace Solver
{
class ImmediateFileResultWriter : public IResultWriter
{
public:
    ImmediateFileResultWriter(const char* folderOutput);
    // Write to file immediately, creating directories if needed
    void addJob(const std::string& entryPath, Yuni::Clob& entryContent) override;
    bool needsTheJobQueue() const override;

private:
    Yuni::String pOutputFolder;
};
} // namespace Solver
} // namespace Antares
