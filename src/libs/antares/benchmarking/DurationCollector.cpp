// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/benchmarking/DurationCollector.h"

#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>

namespace Benchmarking
{

static std::string formatDuration(int64_t duration_ms)
{
    int hours = duration_ms / (1000 * 60 * 60);
    duration_ms %= (1000 * 60 * 60);
    int minutes = duration_ms / (1000 * 60);
    duration_ms %= (1000 * 60);
    int seconds = duration_ms / 1000;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << "h" << std::setfill('0') << std::setw(2)
        << minutes << "m" << std::setfill('0') << std::setw(2) << seconds << "s";
    return oss.str();
}

void DurationCollector::toFileContent(FileContent& file_content)
{
    for (const auto& [name, durations]: duration_items_)
    {
        int64_t duration_sum = accumulate(durations.begin(),
                                          durations.end(),
                                          static_cast<int64_t>(0));

        file_content.addDurationItem(name,
                                     (unsigned int)duration_sum,
                                     formatDuration(duration_sum),
                                     (int)durations.size());
    }
}

DurationCollector::OperationTimer DurationCollector::operator()(const std::string& key)
{
    return OperationTimer(*this, key);
}

void DurationCollector::OperationTimer::addDuration(int64_t duration_ms) const
{
    const std::scoped_lock lock(collector.mutex_);
    collector.duration_items_[key].push_back(duration_ms);
}

void operator<<(const DurationCollector::OperationTimer& op, const std::function<void(void)>& f)
{
    using clock = std::chrono::steady_clock;
    auto start_ = clock::now();
    f();
    auto end_ = clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
    op.addDuration(duration_ms);
}

int64_t DurationCollector::getTime(const std::string& name) const
{
    const auto& v = duration_items_.at(name);

    return accumulate(v.begin(), v.end(), static_cast<int64_t>(0));
}

} // namespace Benchmarking
