// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
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
