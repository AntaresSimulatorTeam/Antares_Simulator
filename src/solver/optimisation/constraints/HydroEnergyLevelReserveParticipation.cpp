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
            // 15 (s)
            // Participation of reserves requires a sufficient level of stock
            //  Sum(P_{res,t_st} * R_{min,res} +/- J_res * R_{lambda,t_st}) <= n_min * R_up/down *
            //  J_res
            // R_t : stock level at time t
            // P_{res,t_st} : power participation for reserve down res at time t_st
            // R_{min,res} : max power participation ratio
            // R_up : max stock level
            // R_down : min stock level
            // J_res : maximum activation of res over several time steps
            // n_min : number of time steps that must have the min amount of stock to contract res
            {
                double sign = capacityReservation.type == ReserveType::UP ? -1. : 1.;

                RESERVE_PARTICIPATION_HYDRO& reserveParticipation = capacityReservation
                                                                      .AllHydroReservesParticipation
                                                                        [cluster];

                builder.updateHourWithinWeek(pdt);

                for (int t = 0; t < capacityReservation.referenceActivationDuration; t++)
                {
                    builder.HydroReserveParticipation( // P_{res,t_st}
                      capacityReservation.type,
                      reserveParticipation.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio, // R_{min,res}
                      t,
                      builder.data
                        .NombreDePasDeTempsPourUneOptimisation); // P_{res,t_st} * R_{min,res}

                    builder.HydroLevel( // R_{lambda,t_st}
                      globalClusterIdx,
                      sign * capacityReservation.energyActivationRatio, // +/- J_res
                      t,
                      builder.data
                        .NombreDePasDeTempsPourUneOptimisation); // +/- J_res * R_{lambda,t_st}
                } // Sum over t

                builder.lessThan();

                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .HydroEnergyLevelParticipation[reserveParticipation
                                                   .globalIndexClusterParticipation]
                  = builder.data
                      .nombreDeContraintes; // n_min * R_up/down * J_res constraint index for RHS

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
