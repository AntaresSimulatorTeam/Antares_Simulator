// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_InitialiserLesCoutsLineaireReserves(PROBLEME_HEBDO* problemeHebdo,
                                             const int PremierPdtDeLIntervalle,
                                             const int DernierPdtDeLIntervalle);
