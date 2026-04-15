#include "antares/solver/optimisation/constraints/OffUnitsThermalParticipatingToReserves.h"

void OffUnitsThermalParticipatingToReserves::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 16 ter
        // The max power of off units participating to Reserves is bounded by the number of units
        // and max power of each unit : P^off +(Pmax^off . M)  <= (Pmax^off . M^on) with P^off:
        // total participation of turned off units to res Pmax^off : max participation for each off
        // unit M : Number of units in the cluster M^on : Number of running units in the cluster
        //

        CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                      .areaCapacityReservations[reserve];

        auto& reserveParticipation = capacityReservation.AllThermalReservesParticipation[cluster];

        int globalClusterIdx = data.thermalClusters[pays]
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques
                                   [reserveParticipation.clusterIdInArea];

        builder.updateHourWithinWeek(pdt)
          .OffThermalClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            1.0) // P^off
          .NumberOfDispatchableUnits(globalClusterIdx, reserveParticipation.maxPowerOff)
          .lessThan();

        data.CorrespondanceCntNativesCntOptim[pdt]
          .reservesIndices.value()
          .powerOffGroupUnitsInThermalClusterParticipating[reserveParticipation
                                                             .globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.ParticipationOfOffUnitsToReserve(builder.data.nombreDeContraintes,
                                               reserveParticipation.clusterName,
                                               capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
