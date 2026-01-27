// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace Antares::Optimisation
{

struct ConnectionVariable
{
    std::string name;
    unsigned indexInProblem;
};

class BendersDecomposition
{
public:
    BendersDecomposition() = default;
    void setCurrentProblemId(std::string id);
    void collectConnectionVariables(std::vector<std::string>&& varnames, unsigned varsCountInPb);

    const std::map<std::string, std::vector<ConnectionVariable>>& connections() const
    {
        return connectionVars_;
    }

private:
    std::map<std::string, std::vector<ConnectionVariable>> connectionVars_;
    std::string currentProblemId_ = "master";
};

class BendersDecompositionWriter
{
public:
    BendersDecompositionWriter(const BendersDecomposition& bd);
    void write(std::ostream& os) const;

private:
    const BendersDecomposition& bd_;
};

} // namespace Antares::Optimisation
