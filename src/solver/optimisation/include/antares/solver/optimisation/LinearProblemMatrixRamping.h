// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "ProblemMatrixEssential.h"
#include "constraints/ConstraintGroup.h"
#include "constraints/PowerOutputVariationGroup.h"
#include "constraints/RampingIncreaseDecreaseRateGroup.h"

using namespace Antares::Data;

class LinearProblemMatrixRamping: public ProblemMatrixEssential
{
public:
    explicit LinearProblemMatrixRamping(PROBLEME_HEBDO* problemeHebdo,
                                        bool Simulation,
                                        ConstraintBuilder& builder);

private:
    bool simulation_ = false;
    RampingIncreaseDecreaseRateGroup rampingIncreaseDecreaseRateGroup_;
    PowerOutputVariationGroup powerOutputVariationGroup_;
};
