#include "antares/solver/optimisation/constraints/HydroEnergyLevelReserveParticipation.h"

void HydroEnergyLevelReserveParticipation::add(int pays, int cluster, int reserve, int pdt)
{
    int globalClusterIdx = data.hydroOfArea[pays].GlobalHydroIndex;
    CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                  .areaCapacityReservations[reserve];

    if (capacityReservation.referenceActivationDuration > 0)
    {
        if (!data.Simulation)
        {
            // 15 (s) (1)
            // Participation of down reserves requires a sufficient level of stock
            //  Sum(P_{res,t_st} * R_{min,res} +/- J_res * R_{lambda,t_st}) <= n_min * R_up
            // R_t : stock level at time t
            // P_{res,t_st} : power participation for reserve down res at time t_st
            // R_{min,res} : max power participation ratio
            // R_up : max stock level
            {
                double sign = capacityReservation.type == ReserveType::UP ? -1. : 1.;

                RESERVE_PARTICIPATION_HYDRO& reserveParticipation = capacityReservation
                                                                      .AllHydroReservesParticipation
                                                                        [cluster];

                builder.updateHourWithinWeek(pdt);

                for (int t = 0; t < capacityReservation.referenceActivationDuration; t++)
                {
                    builder.HydroReserveParticipation(
                      capacityReservation.type,
                      reserveParticipation.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio,
                      t,
                      builder.data.NombreDePasDeTempsPourUneOptimisation);

                    builder.HydroLevel(globalClusterIdx,
                                       sign * capacityReservation.energyActivationRatio);
                }

                builder.lessThan();

                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .HydroEnergyLevelParticipation[reserveParticipation
                                                   .globalIndexClusterParticipation]
                  = builder.data.nombreDeContraintes;

                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.HydroEnergyLevelReserveParticipation(builder.data.nombreDeContraintes,
                                                           reserveParticipation.clusterName,
                                                           capacityReservation.reserveName);
                builder.build();
            }
        }
        else
        {
            builder.data.nombreDeContraintes += 1;
        }
    }
}
