// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputsContext.h"

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

LegacyExtraOutputsContext::LegacyExtraOutputsContext(const PROBLEME_HEBDO& problemeHebdo):
    hydro(problemeHebdo),
    links(problemeHebdo),
    thermal(problemeHebdo)
{
    weekFirstTimeStep = static_cast<unsigned>(problemeHebdo.HeureDansLAnnee);
}

} // namespace Antares::Optimization
