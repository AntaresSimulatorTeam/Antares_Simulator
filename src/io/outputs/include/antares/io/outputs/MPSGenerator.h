// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <algorithm>
#include <filesystem>
#include <ranges>

#include "antares/optimisation/linear-problem-api/linearProblem.h"

#include "ExportableName.h"

namespace Antares::IO::Outputs
{
class MPSGenerator
{
public:
    explicit MPSGenerator(const Antares::Optimisation::LinearProblemApi::ILinearProblem& lp,
                          const std::string& name);
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

template<class T>
std::vector<std::string> ExtractNames(const std::vector<std::unique_ptr<T>>& elements)
{
    std::vector<std::string> names(elements.size());
    NameManager nameManager;
    std::ranges::transform(elements,
                           names.begin(),
                           [&nameManager](const std::unique_ptr<T>& element)
                           { return MakeMpsSafeUniqueName(element->getName(), nameManager); });
    return names;
}

class MPSFileWriter
{
public:
    static void write(const std::filesystem::path& filename, const std::string& content);
};
} // namespace Antares::IO::Outputs
