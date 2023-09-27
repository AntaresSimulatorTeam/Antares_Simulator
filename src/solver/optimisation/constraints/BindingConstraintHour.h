#pragma once

#include "new_constraint_builder.h"
struct BindingConstraintHourData : public BindingConstraintData
{
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};
/*!
 * represent 'Hourly Binding Constraint' type
 */
class BindingConstraintHour : private NewConstraintFactory
{
    public:
        using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param cntCouplante : the binding constraint number
     */
        void add(int pdt, int cntCouplante, std::shared_ptr<BindingConstraintHourData> data);
};
