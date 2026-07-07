// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/SymmetryReserveParticipation.h"

namespace
{
constexpr double EPSILON = 1e-4;
}

template<ValidReserveParticipation T>
void SymmetryReserveParticipation::add(
  int pays,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>>& symmetry,
  int pdt)
{
    if (!data.Simulation)
    {
        auto& reserveParticipationRefWithName = symmetry[0];

        for (size_t i = 1; i < symmetry.size(); ++i)
        {
            auto& reserveParticipationWithName = symmetry[i];

            applyReserveParticipationSymmetry<T>(pdt,
                                                 reserveParticipationRefWithName,
                                                 reserveParticipationWithName);
            builder.equalTo().build();

            setupConstraintNamer(pays,
                                 pdt,
                                 reserveParticipationRefWithName,
                                 reserveParticipationWithName);
        }
    }
    else
    {
        builder.data.nombreDeContraintes += symmetry.size() - 1;
    }
}

template<ValidReserveParticipation T>
void SymmetryReserveParticipation::applyReserveParticipationSymmetry(
  int pdt,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationRefWithName,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationWithName)
{
    builder.updateHourWithinWeek(pdt);
    const T& ref = reserveParticipationRefWithName.reserveParticipation;
    const T& target = reserveParticipationWithName.reserveParticipation;

    auto refIndex = ref.globalIndexClusterParticipation;
    auto targetIndex = target.globalIndexClusterParticipation;

    if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_THERMAL>)
    {
        // 15 (j)
        // Defining the symmetry between two clusters participating to the same reserve
        // The cluster participation ratio regarding its power is the same for both symmetrical
        // reserves constraint :
        // P_res1 / B_res1 = P_res2 / B_res2
        // P_res1 : Participation power of the cluster to the reserve 1
        // B_res1 : Maximum accessible power if each unit running on the cluster for the reserve 1
        // P_res2 : Participation power of the cluster to the reserve 2
        // B_res2 : Maximum accessible power if each unit running on the cluster for the reserve 2
        auto refMaxPower = ref.maxPower;
        auto targetMaxPower = target.maxPower;
        if (refMaxPower > EPSILON
            && targetMaxPower > EPSILON) // disabling symetries in case of zero division
        {
            builder.ThermalClusterReserveParticipation(refIndex, 1 / refMaxPower)
              .ThermalClusterReserveParticipation(targetIndex, -1 / targetMaxPower);
        }
    }
    else if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_STSTORAGE>)
    {
        // 15 (j)
        // Defining the symmetry between two STStorage clusters participating to the same reserve
        // The cluster participation ratio regarding its release and storage power is the same for
        // both symmetrical reserves constraint :
        // (H_res1+Π_res1)/(H^max_res1+Π^max_res1) = (H_res2+Π_res2)/(H^max_res2+Π^max_res2)
        // H_res1 : Release participation power of the cluster to the reserve 1
        // Π_res1 : Store participation power of the cluster to the reserve
        // 1 H^max_res1 : Maximum accessible release power of the cluster for the reserve 1
        // Π^max_res1 : Maximum accessible store power of the cluster for the reserve 1
        // H_res2 : Release participation power of the cluster to the reserve 2
        // Π_res2 : Store participation power of the cluster to the reserve 2
        // H^max_res2 : Maximum accessible release power of the cluster for the reserve 2
        // Π^max_res2 : Maximum accessible store power of the cluster for the reserve 2
        if ((ref.maxRelease + ref.maxStore) > EPSILON
            && (target.maxRelease + target.maxStore)
                 > EPSILON) // sum can be used as values are positive
        {
            double refCoeff = 1.0 / (ref.maxRelease + ref.maxStore);
            double targetCoeff = -1.0 / (target.maxRelease + target.maxStore);
            builder.STStorageReleaseClusterReserveParticipation(refIndex, refCoeff)
              .STStorageStoreClusterReserveParticipation(refIndex, refCoeff)
              .STStorageReleaseClusterReserveParticipation(targetIndex, targetCoeff)
              .STStorageStoreClusterReserveParticipation(targetIndex, targetCoeff);
        }
    }
    else if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_HYDRO>)
    {
        // 15 (j)
        //  Same principle as for the STStorage clusters
        if ((ref.maxRelease + ref.maxStore) > EPSILON
            && (target.maxRelease + target.maxStore) > EPSILON)
        {
            double refCoeff = 1.0 / (ref.maxRelease + ref.maxStore);
            double targetCoeff = -1.0 / (target.maxRelease + target.maxStore);
            builder.HydroReleaseReserveParticipation(refIndex, refCoeff)
              .HydroStoreReserveParticipation(refIndex, refCoeff)
              .HydroReleaseReserveParticipation(targetIndex, targetCoeff)
              .HydroStoreReserveParticipation(targetIndex, targetCoeff);
        }
    }
}

template<ValidReserveParticipation T>
void SymmetryReserveParticipation::setupConstraintNamer(
  int pays,
  int pdt,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationRefWithName,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationWithName)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;

    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.SymmetryReserveParticipation(
      builder.data.nombreDeContraintes,
      reserveParticipationRefWithName.reserveParticipation.get().clusterName,
      reserveParticipationRefWithName.reserveName,
      reserveParticipationWithName.reserveName);
}

template void SymmetryReserveParticipation::add<RESERVE_PARTICIPATION_THERMAL>(
  int,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<RESERVE_PARTICIPATION_THERMAL>>&,
  int);

template void SymmetryReserveParticipation::add<RESERVE_PARTICIPATION_STSTORAGE>(
  int,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<RESERVE_PARTICIPATION_STSTORAGE>>&,
  int);

template void SymmetryReserveParticipation::add<RESERVE_PARTICIPATION_HYDRO>(
  int,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<RESERVE_PARTICIPATION_HYDRO>>&,
  int);
