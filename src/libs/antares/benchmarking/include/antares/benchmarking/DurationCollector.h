#pragma once

#include <vector>
#include <map>
#include <string>
#include <mutex>

#include "file_content.h"

namespace Benchmarking
{
class IDurationCollector
{
public:
    virtual void addDuration(const std::string& name, int64_t duration) = 0;
};

class NullDurationCollector : public IDurationCollector
{
public:
    NullDurationCollector() = default;
    void addDuration(const std::string& /* name */, int64_t /* duration */) override
    { /* Do nothing */
    }
};

class DurationCollector : public IDurationCollector
{
public:
    DurationCollector() = default;

    void toFileContent(FileContent& file_content);
    void addDuration(const std::string& name, int64_t duration) override;

private:
    std::map<std::string, std::vector<int64_t>> duration_items_;

    // Durations can be added in a context of multi-threading, so we need to protect
    // these additions from thread concurrency
    std::mutex mutex_;
};

} // namespace Benchmarking
