#include "antares/solver/optimisation/constraints/POffUnits.h"

void POffUnits::add(int pays, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 16 quater
        // Sum of participations to reserves is inferior to unit max power times number of off units
        // Sum P^off  ≤ Umax (M - M^on)
        // P^off : total participation of turned off units to res
        // Umax : Max power of an unit
        // M : max number of running units in cluster
        // M : actual number of running units in cluster

        int globalClusterIdx = data.thermalClusters[pays]
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::UP))
            {
                if (capacityReservation.AllThermalReservesParticipation.contains(cluster))
                {
                    auto& reserveParticipation = capacityReservation.AllThermalReservesParticipation
                                                   .at(cluster);
                    builder.PowerOfOffUnitsParticipatingToReserve(
                      reserveParticipation.globalIndexClusterParticipation,
                      1);
                }
            }
            builder
              .NumberOfDispatchableUnits(
                globalClusterIdx,
                data.thermalClusters[pays].PmaxDUnGroupeDuPalierThermique[cluster])
              .lessThan();

            data.CorrespondanceCntNativesCntOptim[pdt]
              .reservesIndices.value()
              .maxPowerOffUnitsInThermalCluster[globalClusterIdx]
              = builder.data.nombreDeContraintes;

            ConstraintNamer namer(builder.data.NomDesContraintes);
            const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
            namer.UpdateTimeStep(hourInTheYear);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.POffUnitsUpperBound(builder.data.nombreDeContraintes,
                                      data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
            builder.build();
        }
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
