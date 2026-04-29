// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinearProblemMatrixStartUpCosts.h"
using namespace Antares::Data;

LinearProblemMatrixStartUpCosts::LinearProblemMatrixStartUpCosts(PROBLEME_HEBDO* problemeHebdo,
                                                                 bool Simulation,
                                                                 ConstraintBuilder& builder):
    ProblemMatrixEssential(problemeHebdo),
    simulation_(Simulation),
    pMinMaxDispatchableGenerationGroup_(problemeHebdo, simulation_, builder),
    consistenceNumberOfDispatchableUnitsGroup_(problemeHebdo, simulation_, builder),
    nbUnitsOutageLessThanNbUnitsStopGroup_(problemeHebdo, simulation_, builder),
    nbDispUnitsMinBoundSinceMinUpTimeGroup_(problemeHebdo, simulation_, builder),
    minDownTimeGroup_(problemeHebdo, simulation_, builder)
{
    constraintgroups_ = {&pMinMaxDispatchableGenerationGroup_,
                         &consistenceNumberOfDispatchableUnitsGroup_,
                         &nbUnitsOutageLessThanNbUnitsStopGroup_,
                         &nbDispUnitsMinBoundSinceMinUpTimeGroup_,
                         &minDownTimeGroup_};
}
