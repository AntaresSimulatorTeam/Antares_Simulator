#pragma once

#include <yuni/core/string.h>

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

class NullResultWriter: public Solver::IResultWriter {
    void addEntryFromBuffer(const std::string &, Yuni::Clob &) override;
    void addEntryFromBuffer(const std::string &, std::string &) override;
    void addEntryFromFile(const std::string &, const std::string &) override;
    bool needsTheJobQueue() const override;
    void finalize(bool ) override;
};

} // namespace Solver
} // namespace Antares
