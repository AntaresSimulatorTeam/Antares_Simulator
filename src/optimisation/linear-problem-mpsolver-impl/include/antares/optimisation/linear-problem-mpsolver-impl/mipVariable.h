// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/optimisation/linear-problem-api/mipVariable.h>

namespace operations_research
{
class MPVariable; // forward declaration
}

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

class OrtoolsMipVariable final: public LinearProblemApi::IMipVariable
{
public:
    void setLb(double lb) override;
    void setUb(double ub) override;

    void setBounds(double lb, double ub) override;

    double getLb() const override;
    double getUb() const override;

    const std::string& getName() const override;

    bool isInteger() const override;

    const operations_research::MPVariable* getMpVar() const;
    LinearProblemApi::MipBasisStatus getMipBasisStatus() const override;
    double solutionValue() const override;
    double reducedCost() const override;
    ~OrtoolsMipVariable() override = default;

    explicit OrtoolsMipVariable(operations_research::MPVariable*);

private:
    operations_research::MPVariable* mpVar_;
};

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
