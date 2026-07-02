// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinksOutputsContext.h"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

LinksOutputsContext::LinksOutputsContext(const PROBLEME_HEBDO& problemeHebdo)
{
    const std::size_t nPdt = static_cast<std::size_t>(problemeHebdo.NombreDePasDeTemps);
    for (uint32_t interco = 0; interco < problemeHebdo.NombreDInterconnexions; ++interco)
    {
        std::string linkKey = std::string(
                                problemeHebdo
                                  .NomsDesPays[problemeHebdo.PaysOrigineDeLInterconnexion[interco]])
                              + "$$"

                              + problemeHebdo.NomsDesPays
                                  [problemeHebdo.PaysExtremiteDeLInterconnexion[interco]];
        std::vector<double> directCapacity(nPdt);
        std::vector<double> indirectCapacity(nPdt);
        std::vector<double> loopFlow(nPdt);
        for (std::size_t pdt = 0; pdt < nPdt; ++pdt)
        {
            directCapacity[pdt] = problemeHebdo.ValeursDeNTC[pdt]
                                    .ValeurDeNTCOrigineVersExtremite[interco];
            indirectCapacity[pdt] = problemeHebdo.ValeursDeNTC[pdt]
                                      .ValeurDeNTCExtremiteVersOrigine[interco];
            loopFlow[pdt] = problemeHebdo.ValeursDeNTC[pdt]
                              .ValeurDeLoopFlowOrigineVersExtremite[interco];
        }
        directCapacityByLink[linkKey] = std::move(directCapacity);
        indirectCapacityByLink[linkKey] = std::move(indirectCapacity);
        loopFlowByLink[std::move(linkKey)] = std::move(loopFlow);
    }
}

} // namespace Antares::Optimization
