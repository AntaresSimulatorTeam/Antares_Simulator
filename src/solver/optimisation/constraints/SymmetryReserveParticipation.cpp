#include "antares/solver/optimisation/constraints/SymmetryReserveParticipation.h"

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
    auto refIndex = reserveParticipationRefWithName.reserveParticipation.get()
                      .globalIndexClusterParticipation;
    auto targetIndex = reserveParticipationWithName.reserveParticipation.get()
                         .globalIndexClusterParticipation;

    if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_THERMAL>)
    {
        auto refMaxPower = reserveParticipationRefWithName.reserveParticipation.get().maxPower;
        auto targetMaxPower = reserveParticipationWithName.reserveParticipation.get().maxPower;
        if (abs(refMaxPower) > 10e-4
            && abs(targetMaxPower) > 10e-4) // disableing symetries in case of zero division
        {
            builder.ThermalClusterReserveParticipation(refIndex, 1 / refMaxPower)
              .ThermalClusterReserveParticipation(targetIndex, -1 / targetMaxPower);
        }
    }
    else if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_STSTORAGE>)
    {
        auto refMaxRelease = reserveParticipationRefWithName.reserveParticipation.get().maxRelease;
        auto targetMaxRelease = reserveParticipationWithName.reserveParticipation.get().maxRelease;
        auto refMaxStore = reserveParticipationRefWithName.reserveParticipation.get().maxStore;
        auto targetMaxStore = reserveParticipationWithName.reserveParticipation.get().maxStore;
        if (abs(refMaxRelease) > 10e-4 && abs(targetMaxRelease) > 10e-4)
        {
            builder.STStorageReleaseClusterReserveParticipation(refIndex, 1 / refMaxRelease)
              .STStorageReleaseClusterReserveParticipation(targetIndex, -1 / targetMaxRelease);
        }
        if (abs(refMaxStore) > 10e-4 && abs(targetMaxStore) > 10e-4)
        {
            builder.STStorageStoreClusterReserveParticipation(refIndex, 1 / refMaxStore)
              .STStorageStoreClusterReserveParticipation(targetIndex, -1 / targetMaxStore);
        }
    }
    else if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_HYDRO>)
    {
        auto refMaxRelease = reserveParticipationRefWithName.reserveParticipation.get().maxRelease;
        auto targetMaxRelease = reserveParticipationWithName.reserveParticipation.get().maxRelease;
        auto refMaxStore = reserveParticipationRefWithName.reserveParticipation.get().maxStore;
        auto targetMaxStore = reserveParticipationWithName.reserveParticipation.get().maxStore;

        if (abs(refMaxRelease) > 10e-4 && abs(targetMaxRelease) > 10e-4)
        {
            builder.HydroReleaseReserveParticipation(refIndex, 1 / refMaxRelease)
              .HydroReleaseReserveParticipation(targetIndex, -1 / targetMaxRelease);
        }
        if (abs(refMaxStore) > 10e-4 && abs(targetMaxStore) > 10e-4)
        {
            builder.HydroStoreReserveParticipation(refIndex, 1 / refMaxStore)
              .HydroStoreReserveParticipation(targetIndex, -1 / targetMaxStore);
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
