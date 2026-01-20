// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "constraints/ConstraintGroup.h"

class ProblemMatrixEssential
{
public:
    explicit ProblemMatrixEssential(PROBLEME_HEBDO* problemeHebdo);

    virtual void Run();

    void InitializeProblemAResoudreCounters();
    PROBLEME_HEBDO* problemeHebdo_;
    std::vector<ConstraintGroup*> constraintgroups_;
};
