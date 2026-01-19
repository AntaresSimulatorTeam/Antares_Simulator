// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "ProblemMatrixEssential.h"
#include "constraints/ExchangeBalanceGroup.h"

class QuadraticProblemMatrix final: public ProblemMatrixEssential
{
public:
    QuadraticProblemMatrix(PROBLEME_HEBDO* problem_hebdo, ConstraintBuilder& builder):
        ProblemMatrixEssential(problem_hebdo),
        exchangeBalanceGroup_(problem_hebdo, builder)
    {
        constraintgroups_ = {&exchangeBalanceGroup_};
    }

    void Run() override;

private:
    ExchangeBalanceGroup exchangeBalanceGroup_;
};
