#pragma once
#include "ConstraintBuilder.h"

template<typename T>
concept ValidReserveParticipation = std::is_same_v<T, RESERVE_PARTICIPATION_THERMAL>
                                    || std::is_same_v<T, RESERVE_PARTICIPATION_STSTORAGE>
                                    || std::is_same_v<T, RESERVE_PARTICIPATION_HYDRO>;

/*!
 * represent 'SymmetryReserveParticipation' Constraint type
 */
class SymmetryReserveParticipation: private ConstraintFactory
{
public:
    SymmetryReserveParticipation(ConstraintBuilder& builder, ReserveData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param symmetry : lists of symmetric participations
     * @param pdt : timestep
     */

    template<ValidReserveParticipation T>
    void add(int pays,
             const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>>& symmetry,
             int pdt);

private:
    template<ValidReserveParticipation T>
    void applyReserveParticipationSymmetry(
      int pdt,
      const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationRefWithName,
      const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationWithName);

    template<ValidReserveParticipation T>
    void setupConstraintNamer(
      int pays,
      int pdt,
      const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationRefWithName,
      const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationWithName);

    ReserveData& data;
};
