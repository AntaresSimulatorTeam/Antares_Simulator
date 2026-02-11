// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>

#include <antares/solver/optim-model-filler/BendersDecomposition.h>

namespace Antares::Optimisation
{

void BendersDecomposition::setCurrentProblemId(std::string id)
{
    currentProblemId_ = id;
}

void BendersDecomposition::collectCouplingVariables(std::vector<std::string>&& varnames,
                                                    unsigned varsCountInPb)
{
    std::vector<std::string> names = std::move(varnames);
    unsigned nbVars = names.size();
    unsigned startIndexInPb = varsCountInPb - nbVars;
    unsigned varIndex = startIndexInPb;
    for (const auto& name: names)
    {
        connectionVars_[currentProblemId_].emplace_back(name, varIndex);
        varIndex++;
    }
}

BendersDecompositionWriter::BendersDecompositionWriter(const BendersDecomposition& bd):
    bd_(bd)
{
}

void BendersDecompositionWriter::write(std::ostream& os) const
{
    os << getBendersDecomposition();
}

std::string BendersDecompositionWriter::getBendersDecomposition() const
{
    std::string structure;
    for (const auto& [problemId, v]: bd_.couplings())
    {
        for (const auto& [variableName, variableIndex]: v)
        {
            fmt::format_to(std::back_inserter(structure),
                           "{}\t{}\t{}\n",
                           problemId,
                           variableName,
                           variableIndex);
        }
    }
    return structure;
}

} // namespace Antares::Optimisation
