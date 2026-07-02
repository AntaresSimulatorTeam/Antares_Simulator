// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/HydroOutputsContext.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

HydroOutputsContext::HydroOutputsContext(const PROBLEME_HEBDO& problemeHebdo)
{
    // ConsommationsAbattues and ApportNaturelHoraire span the whole week and
    // are indexed by hour-in-week; they are snapshotted over
    // [0, NombreDePasDeTemps) once and the consumer indexes them with
    // hourInWeek = timeIndex - weekFirstTimeStep.
    const std::size_t nPdt = static_cast<std::size_t>(problemeHebdo.NombreDePasDeTemps);
    for (uint32_t pays = 0; pays < problemeHebdo.NombreDePays; ++pays)
    {
        const auto& areaName = problemeHebdo.NomsDesPays[pays];
        const auto& hydro = problemeHebdo.CaracteristiquesHydrauliques[pays];
        reservoirCapacityByArea[areaName] = hydro.TailleReservoir;

        // actual_load is the raw load series; ConsommationAbattueDuPays is the
        // residual load (load minus must-run generation, see
        // sim_calcul_economique.cpp), so the must-run part is added back.
        std::vector<double> load(nPdt);
        for (std::size_t pdt = 0; pdt < nPdt; ++pdt)
        {
            load[pdt] = problemeHebdo.ConsommationsAbattues[pdt].ConsommationAbattueDuPays[pays]
                        + problemeHebdo.AllMustRunGeneration[pdt].AllMustRunGenerationOfArea[pays];
        }
        loadByArea[areaName] = std::move(load);

        // Non-hydro areas leave ApportNaturelHoraire empty; only carry inflows
        // when the series covers the week (the HydroLevel anchor only exists for
        // areas with a reservoir anyway).
        if (hydro.ApportNaturelHoraire.size() >= nPdt)
        {
            std::vector<double> inflows(nPdt);
            for (std::size_t pdt = 0; pdt < nPdt; ++pdt)
            {
                inflows[pdt] = hydro.ApportNaturelHoraire[pdt];
            }
            inflowsByArea[areaName] = std::move(inflows);
        }
    }
}

} // namespace Antares::Optimization
