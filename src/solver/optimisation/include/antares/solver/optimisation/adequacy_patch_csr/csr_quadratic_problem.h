// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "../constraints/ConstraintBuilder.h"
class HourlyCSRProblem;

namespace Antares::Solver::Optimization
{

class CsrQuadraticProblem final
{
public:
    CsrQuadraticProblem(PROBLEME_HEBDO* p,
                        PROBLEME_ANTARES_A_RESOUDRE& pa,
                        HourlyCSRProblem& hourly):
        problemeHebdo_(p),
        problemeAResoudre_(pa),
        hourlyCsrProblem_(hourly)
    {
    }

    void buildConstraintMatrix();

private:
    PROBLEME_HEBDO* problemeHebdo_;
    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre_;
    HourlyCSRProblem& hourlyCsrProblem_;

    void setConstraintsOnFlows(ConstraintBuilder& builder);
    void setNodeBalanceConstraints(ConstraintBuilder& builder);
    void setBindingConstraints(ConstraintBuilder& builder);
};

} // namespace Antares::Solver::Optimization
