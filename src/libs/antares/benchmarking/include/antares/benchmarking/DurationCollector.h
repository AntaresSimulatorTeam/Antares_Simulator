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
    virtual ~IDurationCollector() = default;
    virtual void addDuration(const std::string& name, int64_t duration) = 0;
};

class NullDurationCollector : public IDurationCollector
{
public:
    NullDurationCollector() = default;
    virtual ~NullDurationCollector() = default;
    void addDuration(const std::string& /* name */, int64_t /* duration */) override
    { /* Do nothing */
    }
};

class DurationCollector : public IDurationCollector
{
public:
    DurationCollector() = default;
    virtual ~DurationCollector() = default;

    void toFileContent(FileContent& file_content);
    void addDuration(const std::string& name, int64_t duration) override;

private:
    std::map<std::string, std::vector<int64_t>> duration_items_;

    // Durations can be added in a context of multi-threading, so we need to protect
    // these additions from thread concurrency
    std::mutex mutex_;
};

} // namespace Benchmarking
