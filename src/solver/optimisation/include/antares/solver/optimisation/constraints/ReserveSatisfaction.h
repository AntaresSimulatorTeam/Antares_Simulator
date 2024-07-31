#pragma once
#include "ConstraintBuilder.h"

/*
 * represent 'ReserveSatisfaction' Constraint type
 */
class ReserveSatisfaction : private ConstraintFactory
{
public:
    ReserveSatisfaction(ConstraintBuilder& builder, ReserveData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param reserve : index of the reserve inside area
     * @param pdt : timestep
     * @param isUpReserve : true if ReserveUp, false if ReserveDown
     */
    void add(int pays, int reserve, int pdt, bool isUpReserve);

private:
    ReserveData& data;
};
