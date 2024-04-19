#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"

void ReserveSatisfaction::add(int pays, int reserve, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 24

        // Sum(P_θ) - S + J^+ -J^-
        // P : Participation power from cluster θ to the reserve res
        // S : Internal reserve res need for the area
        // J^+ : Amount of internal excess reserve for the reserve res
        // J^+ : Amount of internal unsatified reserve for the reserve res

        builder.updateHourWithinWeek(pdt);

        CAPACITY_RESERVATION capacityReservation
          = isUpReserve
              ? data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp[reserve]
              : data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown[reserve];

        for (size_t cluster = 0; cluster < capacityReservation.AllReservesParticipation.size(); cluster++)
        {
            if (capacityReservation.AllReservesParticipation[cluster].maxPower != CLUSTER_NOT_PARTICIPATING)
                builder.RunningClusterReserveParticipation(
                  data.thermalClusters[pays]
                    .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster],
                  1);
        }

        capacityReservation.need[pdt];

        builder.equalTo();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
            namer.UpdateTimeStep(hourInTheYear);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.ReserveSatisfaction(
              builder.data.nombreDeContraintes,
              capacityReservation.reserveName);
        }
        builder.build();
    }
    else
    {
        builder.data.NbTermesContraintesPourLesReserves += 1;

        builder.data.nombreDeContraintes += 1;
    }
}
