// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "ProblemMatrixEssential.h"
#include "constraints/ConsistenceNumberOfDispatchableUnitsGroup.h"
#include "constraints/ConstraintGroup.h"
#include "constraints/MinDownTimeGroup.h"
#include "constraints/NbDispUnitsMinBoundSinceMinUpTimeGroup.h"
#include "constraints/NbUnitsOutageLessThanNbUnitsStopGroup.h"
#include "constraints/PMinMaxDispatchableGenerationGroup.h"

using namespace Antares::Data;

class LinearProblemMatrixStartUpCosts final: public ProblemMatrixEssential
{
public:
    explicit LinearProblemMatrixStartUpCosts(PROBLEME_HEBDO* problemeHebdo,
                                             bool Simulation,
                                             ConstraintBuilder& builder);

private:
    bool simulation_ = false;
    PMinMaxDispatchableGenerationGroup pMinMaxDispatchableGenerationGroup_;
    ConsistenceNumberOfDispatchableUnitsGroup consistenceNumberOfDispatchableUnitsGroup_;
    NbUnitsOutageLessThanNbUnitsStopGroup nbUnitsOutageLessThanNbUnitsStopGroup_;
    NbDispUnitsMinBoundSinceMinUpTimeGroup nbDispUnitsMinBoundSinceMinUpTimeGroup_;
    MinDownTimeGroup minDownTimeGroup_;
};
