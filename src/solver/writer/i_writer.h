#pragma once

#include <memory>

namespace Antares
{
namespace Solver
{
class IResultWriter
{
public:
    using Ptr = std::shared_ptr<IResultWriter>;
    virtual void addJob(const std::string& entryPath, const char* entryContent, size_t entrySize)
      = 0;
    virtual bool needsTheJobQueue() const = 0;
};
} // namespace Solver
} // namespace Antares
