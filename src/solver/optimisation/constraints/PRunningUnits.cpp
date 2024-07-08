#include "antares/solver/optimisation/constraints/PRunningUnits.h"

void PRunningUnits::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 17 Quater
        // Participation to the reserve is bounded for a cluster
        // constraint : P - P^on = 0
        // P : Participation power
        // P^on : Participation of running units

        CAPACITY_RESERVATION capacityReservation
          = isUpReserve
              ? data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp[reserve]
              : data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION reserveParticipation
          = capacityReservation.AllReservesParticipation[cluster];

        int globalClusterIdx = data.thermalClusters[pays]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques[reserveParticipation.clusterIdInArea];

        builder.updateHourWithinWeek(pdt)
          .RunningClusterReserveParticipation(reserveParticipation.globalIndexClusterParticipation, 1.0)
          .ClusterReserveParticipation(reserveParticipation.globalIndexClusterParticipation, -1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.ParticipationOfRunningUnitsToReserve(builder.data.nombreDeContraintes,
                          reserveParticipation.clusterName,
                          capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.NbTermesContraintesPourLesReserves += 2;
        builder.data.nombreDeContraintes++;
    }
}
