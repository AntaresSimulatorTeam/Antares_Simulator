// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/POutBounds.h"

void POutBounds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.thermalClusters[pays]
                             .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

    if (!data.Simulation)
    {
        // 17 ter
        // Power output is bounded by must-run commitments and power availability, reserves must fit
        // within the bounds P_down + Sum(P^on_re-) <= P <= P_up - Sum(P^on_re+) P^on_re- :
        // Participation of running units in cluster θ to Down reserves P^on_re+ : Participation of
        // running units in cluster θ to Up reserves P : Power output from cluster θ P_down :
        // Minimal power output demanded from cluster θ P_up : Maximal power output from cluster θ

        // 17 ter (1) : Sum(P^on_re-) - P <= - P_down
        {
            builder.updateHourWithinWeek(pdt);

            ReserveData::addRunningThermalClusterReserveParticipationToBuilder(data,
                                                                               pays,
                                                                               cluster,
                                                                               ReserveType::DOWN,
                                                                               builder);

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, -1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .thermalClusterPOutBoundMin[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutBoundMin(builder.data.nombreDeContraintes,
                                   data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
                builder.build();
            }
        }

        // 17 ter (2) : P + Sum(P^on_re+) <= P_up
        {
            builder.updateHourWithinWeek(pdt);

            ReserveData::addRunningThermalClusterReserveParticipationToBuilder(data,
                                                                               pays,
                                                                               cluster,
                                                                               ReserveType::UP,
                                                                               builder);

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .thermalClusterPOutBoundMax[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutBoundMax(builder.data.nombreDeContraintes,
                                   data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForThermalReserves(
          pays,
          cluster);
    }
}
