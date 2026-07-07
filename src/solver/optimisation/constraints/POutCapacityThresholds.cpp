// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/POutCapacityThresholds.h"

void POutCapacityThresholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.thermalClusters[pays]
                             .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

    if (!data.Simulation)
    {
        // 17 bis
        // Power output remains within limits set by minimum stable power and maximum capacity
        // thresholds l * M + Sum(P^on_re-) <= P <= u * M - Sum(P^on_re+) l : minimum stable power
        // output when running u : maximum stable power output when running M : number of running
        // units in cluster θ P^on_re- : Participation of running units in cluster θ to Down
        // reserves P^on_re+ : Participation of running units in cluster θ to Up reserves P : Power
        // output from cluster θ

        // 17 bis (1) : l * M + Sum(P^on_re-) - P <= 0
        {
            builder.updateHourWithinWeek(pdt);

            ReserveData::addRunningThermalClusterReserveParticipationToBuilder(data,
                                                                               pays,
                                                                               cluster,
                                                                               ReserveType::DOWN,
                                                                               builder);

            if (builder.NumberOfVariables() > 0)
            {
                builder
                  .NumberOfDispatchableUnits(
                    globalClusterIdx,
                    data.thermalClusters[pays].pminDUnGroupeDuPalierThermique[cluster])
                  .DispatchableProduction(globalClusterIdx, -1)
                  .lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutCapacityThresholdInf(
                  builder.data.nombreDeContraintes,
                  data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
                builder.build();
            }
        }

        // 17 bis (2) : P - u * M + Sum(P^on_re+) <= 0
        {
            builder.updateHourWithinWeek(pdt);

            ReserveData::addRunningThermalClusterReserveParticipationToBuilder(data,
                                                                               pays,
                                                                               cluster,
                                                                               ReserveType::UP,
                                                                               builder);

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, 1)
                  .NumberOfDispatchableUnits(
                    globalClusterIdx,
                    -data.thermalClusters[pays].PmaxDUnGroupeDuPalierThermique[cluster])
                  .lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutCapacityThresholdSup(
                  builder.data.nombreDeContraintes,
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
