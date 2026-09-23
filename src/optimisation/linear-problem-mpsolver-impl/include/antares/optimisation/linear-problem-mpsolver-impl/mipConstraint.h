// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/optimisation/linear-problem-api/mipConstraint.h>

namespace operations_research
{
class MPConstraint; // forward declaration
}

namespace Antares::LinearProblem::MpsolverImpl
{

class OrtoolsMipConstraint final: public Api::IMipConstraint
{
public:
    void setLb(double lb) override;
    void setUb(double ub) override;

    void setBounds(double lb, double ub) override;
    void setCoefficient(Api::IMipVariable* var, double coefficient) override;

    [[nodiscard]] double getLb() const override;
    [[nodiscard]] double getUb() const override;

    [[nodiscard]] double getCoefficient(const Api::IMipVariable* var) const override;
    [[nodiscard]] std::vector<std::pair<int, double>> getCoefficients() const override;
    [[nodiscard]] double dual() const override;

    [[nodiscard]] const std::string& getName() const override;
    Api::MipBasisStatus getMipBasisStatus() const override;
    ~OrtoolsMipConstraint() override = default;

    explicit OrtoolsMipConstraint(operations_research::MPConstraint* mpConstraint);

private:
    operations_research::MPConstraint* mpConstraint_;
};

} // namespace Antares::LinearProblem::MpsolverImpl
