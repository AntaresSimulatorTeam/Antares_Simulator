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

class DurationCollector
{
public:
    void toFileContent(FileContent& file_content);
    void addDuration(const std::string& name, int64_t duration);

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
