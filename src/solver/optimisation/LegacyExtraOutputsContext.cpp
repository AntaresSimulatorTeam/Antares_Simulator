// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputsContext.h"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{
LegacyExtraOutputsContext::LegacyExtraOutputsContext(const PROBLEME_HEBDO& problemeHebdo)
{
    weekFirstTimeStep = static_cast<unsigned>(problemeHebdo.HeureDansLAnnee);
    // ConsommationsAbattues and ApportNaturelHoraire span the whole week and are
    // indexed by hour-in-week, just like ValeursDeNTC below; they are snapshotted
    // over [0, NombreDePasDeTemps) once and the consumer indexes them with
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

        // Thermal per-cluster data is keyed by "area$$cluster" because the
        // thermal anchor's component (the cluster name) is not unique across
        // areas. Emission factors feed the emissions outputs; the unit/min/
        // availability data feeds the margin outputs.
        const auto& paliers = problemeHebdo.PaliersThermiquesDuPays[pays];
        for (int palier = 0; palier < paliers.NombreDePaliersThermiques; ++palier)
        {
            const std::string key = std::string(areaName) + "$$"
                                    + paliers.NomsDesPaliersThermiques[palier];
            emissionFactorsByCluster[key] = paliers.emissionFactors[palier];

            ThermalMarginData margin;
            margin.unitSize = paliers.TailleUnitaireDUnGroupeDuPalierThermique[palier];
            margin.minStablePower = paliers.PminDuPalierThermiquePendantUneHeure[palier];
            const auto& disp = paliers.PuissanceDisponibleEtCout[palier];
            if (disp.PuissanceDisponibleDuPalierThermique.size() >= nPdt
                && disp.PuissanceMinDuPalierThermique.size() >= nPdt)
            {
                margin.availability.assign(disp.PuissanceDisponibleDuPalierThermique.begin(),
                                           disp.PuissanceDisponibleDuPalierThermique.begin()
                                             + static_cast<std::ptrdiff_t>(nPdt));
                margin.minGenPower.assign(disp.PuissanceMinDuPalierThermique.begin(),
                                          disp.PuissanceMinDuPalierThermique.begin()
                                            + static_cast<std::ptrdiff_t>(nPdt));
            }
            thermalMarginByCluster[key] = std::move(margin);
        }
    }
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
