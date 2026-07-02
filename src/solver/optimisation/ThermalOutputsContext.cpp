// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ThermalOutputsContext.h"

#include <string>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

ThermalClusterData::ThermalClusterData(
  std::array<double, Data::Pollutant::POLLUTANT_MAX> emissionFactors,
  double unitSize,
  double minStablePower,
  std::span<const double> availability,
  std::span<const double> minGenPower):
    emissionFactors{emissionFactors},
    unitSize{unitSize},
    minStablePower{minStablePower},
    availability{availability},
    minGenPower{minGenPower}
{
}

ThermalOutputsContext::ThermalOutputsContext(const PROBLEME_HEBDO& problemeHebdo)
{
    // Thermal per-cluster data is keyed by "area$$cluster" because the
    // thermal anchor's component (the cluster name) is not unique across
    // areas. Emission factors feed the emissions outputs; the unit/min/
    // availability data feeds the margin outputs.
    for (uint32_t pays = 0; pays < problemeHebdo.NombreDePays; ++pays)
    {
        const auto& areaName = problemeHebdo.NomsDesPays[pays];
        const auto& paliers = problemeHebdo.PaliersThermiquesDuPays[pays];
        for (int palier = 0; palier < paliers.NombreDePaliersThermiques; ++palier)
        {
            const std::string key = std::string(areaName) + "$$"
                                    + paliers.NomsDesPaliersThermiques[palier];

            const auto& disp = paliers.PuissanceDisponibleEtCout[palier];
            ThermalClusterData cluster(paliers.emissionFactors[palier],
                                       paliers.TailleUnitaireDUnGroupeDuPalierThermique[palier],
                                       paliers.PminDuPalierThermiquePendantUneHeure[palier],
                                       disp.PuissanceDisponibleDuPalierThermique,
                                       disp.PuissanceMinDuPalierThermique);
            byCluster.emplace(key, cluster);
        }
    }
}

} // namespace Antares::Optimization
