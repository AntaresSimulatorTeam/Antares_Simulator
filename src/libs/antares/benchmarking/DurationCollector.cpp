/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/benchmarking/DurationCollector.h"

#include <numeric>
#include <string>

namespace Benchmarking
{

void DurationCollector::addDuration(const std::string& name, int64_t duration)
{
    const std::lock_guard lock(mutex_);
    duration_items_[name].push_back(duration);
}

void DurationCollector::toFileContent(FileContent& file_content)
{
    for (const auto& [name, durations]: duration_items_)
    {
        const int64_t duration_sum = accumulate(durations.begin(),
                                                durations.end(),
                                                static_cast<int64_t>(0));

        file_content.addDurationItem(name, (unsigned int)duration_sum, (int)durations.size());
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
