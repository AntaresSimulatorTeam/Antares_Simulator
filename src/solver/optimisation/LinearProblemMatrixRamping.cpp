// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinearProblemMatrixRamping.h"
using namespace Antares::Data;

LinearProblemMatrixRamping::LinearProblemMatrixRamping(PROBLEME_HEBDO* problemeHebdo,
                                                       bool Simulation,
                                                       ConstraintBuilder& builder):
    ProblemMatrixEssential(problemeHebdo),
    simulation_(Simulation),
    rampingIncreaseDecreaseRateGroup_(problemeHebdo, simulation_, builder),
    powerOutputVariationGroup_(problemeHebdo, simulation_, builder)
{
    constraintgroups_ = {&rampingIncreaseDecreaseRateGroup_, &powerOutputVariationGroup_};
}
