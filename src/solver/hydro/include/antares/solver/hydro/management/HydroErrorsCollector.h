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
#include <string>
#include <vector>

namespace Antares
{
class HydroErrorsCollector
{
public:
    HydroErrorsCollector() = default;
    void Collect(const std::string& area_name, const std::string& message);
    void Collect(const std::string& message);
    bool CheckForFatalErrors() const;

private:
    std::unordered_map<std::string, uint> area_errors_counter_;
    bool errors_limit_reached_ = false;
    bool stop_ = false;
    // std::vector<std::string> fatal_errors_;
};
} // namespace Antares
