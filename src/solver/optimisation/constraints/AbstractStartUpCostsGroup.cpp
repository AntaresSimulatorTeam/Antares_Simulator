// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/AbstractStartUpCostsGroup.h"

AbstractStartUpCostsGroup::AbstractStartUpCostsGroup(PROBLEME_HEBDO* problemeHebdo,
                                                     bool simulation,
                                                     ConstraintBuilder& builder):
    ConstraintGroup(problemeHebdo, builder),
    simulation_(simulation)
{
}

StartUpCostsData AbstractStartUpCostsGroup::GetStartUpCostsDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_};
}
