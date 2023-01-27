#pragma once

#include <yuni/core/string.h>
#include <antares/inifile.h>

#include <memory>
#include <string>

namespace Antares
{
namespace Solver
{
class IResultWriter
{
public:
    using Ptr = std::shared_ptr<IResultWriter>;
    virtual void addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent) = 0;
    virtual void addEntryFromBuffer(const std::string& entryPath, std::string& entryContent) = 0;
    virtual void addEntryFromFile(const std::string& entryPath, const std::string& filePath) = 0;
    virtual bool needsTheJobQueue() const = 0;
    virtual void finalize(bool verbose) = 0;
};
} // namespace Solver
} // namespace Antares
