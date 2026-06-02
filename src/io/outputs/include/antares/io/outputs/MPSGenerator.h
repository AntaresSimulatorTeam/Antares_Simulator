/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <algorithm>
#include <filesystem>
#include <fmt/format.h>
#include <ranges>

#include "antares/optimisation/linear-problem-api/linearProblem.h"

#include "ExportableName.h"

namespace Antares::IO::Outputs
{
class MPSGenerator
{
public:
    explicit MPSGenerator(const Antares::Optimisation::LinearProblemApi::ILinearProblem& lp,
                          const std::string& name,
                          bool keepNames);
    std::string run() const;

private:
    const Optimisation::LinearProblemApi::ILinearProblem& linearProblem_;
    std::string name_;
    std::vector<std::string> exportableConstraintsNames_;
    std::vector<std::string> exportableVariablesNames_;
    //--//
    void writeHeader(std::string& mps) const;
    void writeName(std::string& mps) const;
    void writeRows(std::string& mps) const;
    void writeColumns(std::string& mps) const;
    void writeRhs(std::string& mps) const;
    void writeRanges(std::string& mps) const;
    void writeBounds(std::string& mps) const;
    void writeEnd(std::string& mps) const;
};

class MPSFileWriter
{
public:
    static void write(const std::filesystem::path& filename, const std::string& content);
};
} // namespace Antares::IO::Outputs
