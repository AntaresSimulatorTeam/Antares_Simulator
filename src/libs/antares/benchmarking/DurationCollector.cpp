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
#include <numeric>
#include <string>
#include "antares/benchmarking/DurationCollector.h"

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
        const int64_t duration_sum = accumulate(durations.begin(), durations.end(), (int64_t)0);

        file_content.addDurationItem(name, (unsigned int)duration_sum, (int)durations.size());
    }
}
} // namespace Benchmarking
