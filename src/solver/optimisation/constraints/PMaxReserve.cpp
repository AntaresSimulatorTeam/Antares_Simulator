#include "antares/solver/optimisation/constraints/PMaxReserve.h"

void PMaxReserve::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 16 bis
        // Participation to the reserve is bounded for a cluster
        // constraint : P - M * B <= 0
        // P : Participation power
        // M : Number of running units in the cluster
        // B : Maximum accessible power if each unit running on the cluster for the reserve 

        CAPACITY_RESERVATION capacityReservation
          = isUpReserve
              ? data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp[reserve]
              : data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION reserveParticipation
          = capacityReservation.AllReservesParticipation[cluster];

        int globalClusterIdx = data.thermalClusters[pays]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

        builder.updateHourWithinWeek(pdt)
          .RunningClusterReserveParticipation(globalClusterIdx, 1.0)
          .NumberOfDispatchableUnits(globalClusterIdx, -reserveParticipation.maxPower)
          .lessThan();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
            namer.UpdateTimeStep(hourInTheYear);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.PMaxReserve(builder.data.nombreDeContraintes,
                              reserveParticipation.clusterName,
                              capacityReservation.reserveName);
        }
        builder.build();
    }
    else
    {
        builder.data.NbTermesContraintesPourLesReserves += 1;
        builder.data.nombreDeContraintes++;
    }
}
