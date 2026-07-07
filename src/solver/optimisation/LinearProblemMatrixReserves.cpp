// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#include "antares/solver/optimisation/LinearProblemMatrixReserves.h"
using namespace Antares::Data;

LinearProblemMatrixReserves::LinearProblemMatrixReserves(PROBLEME_HEBDO* problemeHebdo,
                                                         bool Simulation,
                                                         ConstraintBuilder& builder):
    ProblemMatrixEssential(problemeHebdo),
    simulation_(Simulation),
    reserveParticipationGroup_(problemeHebdo, simulation_, builder)
{
    constraintgroups_ = {&reserveParticipationGroup_};
}
