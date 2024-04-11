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

#include <functional>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>

namespace Antares
{

class DurationCollector
{
public:
    using clock = std::chrono::steady_clock;

    std::map<std::string, long> times;

    struct OperationTimer
    {
        OperationTimer(DurationCollector& collector, const std::string& key):
            collector(collector), key(key) {}

        DurationCollector& collector;
        const std::string key;
    };

    OperationTimer operator()(const std::string& key)
    {
        return OperationTimer(*this, key);
    }

    friend void operator<<(const OperationTimer& op, std::function<void(void)> f)
    {
        auto start_ = clock::now();
        f();
        auto end_ = clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
        op.collector.times[op.key] += duration_ms;
    }
    void summary()
    {
        for (auto& [k, v] : times)
            std::cout << k << '\t' << v << '\n';
    }
};

extern DurationCollector durationCollector;

} // namespace Antares
