#include "antares/solver/optimisation/constraints/POffUnits.h"

void POffUnits::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 16 quater
        // Participation to reserves remains within limits set by minimum stable power and maximum capacity threasholds 
        // l * M^off ≤ P^off ≤ M^off * P^off_max 
        // l : minimum stable power output when running 
        // M^off : number of off units paricipating to reserves 
        // P^off : Participation of off units to reserves
        // P^off_max : Max Participation of off units

        CAPACITY_RESERVATION capacityReservation
          = data.areaReserves[pays].areaCapacityReservationsUp[reserve];

        auto& reserveParticipation = capacityReservation.AllThermalReservesParticipation[cluster];

        int globalClusterIdx
          = data.thermalClusters[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques
              [reserveParticipation.clusterIdInArea];

        // 16 quater (1) : l * M^off - P^off ≤ 0
        {
            builder.updateHourWithinWeek(pdt)
              .NumberOfOffUnitsParticipatingToReserve(
                reserveParticipation.globalIndexClusterParticipation,
                data.thermalClusters[pays].pminDUnGroupeDuPalierThermique[cluster])
              .OffThermalClusterReserveParticipation(
                reserveParticipation.globalIndexClusterParticipation, -1)
              .lessThan();

            ConstraintNamer namer(builder.data.NomDesContraintes);
            const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
            namer.UpdateTimeStep(hourInTheYear);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.POffUnitsLowerBound(builder.data.nombreDeContraintes,
                                      reserveParticipation.clusterName,
                                      capacityReservation.reserveName);
            builder.build();
        }

        // 16 quater (2) : P^off - M^off * P^off_max ≤ 0
        {
            builder.updateHourWithinWeek(pdt)
              .OffThermalClusterReserveParticipation(
                reserveParticipation.globalIndexClusterParticipation, 1)
              .NumberOfOffUnitsParticipatingToReserve(
                reserveParticipation.globalIndexClusterParticipation,
                -reserveParticipation.maxPowerOff)
              .lessThan();

            ConstraintNamer namer(builder.data.NomDesContraintes);
            const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
            namer.UpdateTimeStep(hourInTheYear);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.POffUnitsUpperBound(builder.data.nombreDeContraintes,
                                      reserveParticipation.clusterName,
                                      capacityReservation.reserveName);
            builder.build();
        }
    }
    else
    {
        builder.data.NbTermesContraintesPourLesReserves += 4;
        builder.data.nombreDeContraintes += 2;
    }
}
