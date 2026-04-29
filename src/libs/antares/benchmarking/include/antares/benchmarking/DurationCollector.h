// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "file_content.h"

namespace Benchmarking
{

class DurationCollector final
{
public:
    void toFileContent(FileContent& file_content);

    struct OperationTimer
    {
        OperationTimer(DurationCollector& collector, const std::string& key):
            collector(collector),
            key(key)
        {
        }

        void addDuration(int64_t duration_ms) const;

        DurationCollector& collector;
        const std::string key;
    };

    OperationTimer operator()(const std::string& key);

    friend void operator<<(const OperationTimer& op, const std::function<void(void)>& f);

    int64_t getTime(const std::string& name) const;

private:
    std::map<std::string, std::vector<int64_t>> duration_items_;

    // Durations can be added in a context of multi-threading, so we need to protect
    // these additions from thread concurrency
    std::mutex mutex_;
};

} // namespace Benchmarking
