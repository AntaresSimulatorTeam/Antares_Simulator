#include "antares/solver/optimisation/constraints/OffUnitsThermalParticipatingToReserves.h"

void OffUnitsThermalParticipatingToReserves::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 16 ter
        // The number of off units participating to Reserves is bounded by the number of units and number of running units
        // constraint : M^off + M^on <= M
        // M^off : Number of off units participating to the reserve
        // M : Number of units in the cluster
        // M^on : Number of running units in the cluster

        CAPACITY_RESERVATION capacityReservation
          = data.areaReserves[pays].areaCapacityReservationsUp[reserve];

        auto& reserveParticipation = capacityReservation.AllThermalReservesParticipation[cluster];

        int globalClusterIdx
          = data.thermalClusters[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques
              [reserveParticipation.clusterIdInArea];

        builder.updateHourWithinWeek(pdt)
          .NumberOfOffUnitsParticipatingToReserve(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .NumberOfDispatchableUnits(globalClusterIdx, 1.0)
          .lessThan();

        data.CorrespondanceCntNativesCntOptim[pdt]
          .nbOffGroupUnitsParticipatingToReservesInThermalClusterConstraintIndex
            [reserveParticipation.globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.NumberOfOffUnitsParticipatingToReserve(builder.data.nombreDeContraintes,
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
