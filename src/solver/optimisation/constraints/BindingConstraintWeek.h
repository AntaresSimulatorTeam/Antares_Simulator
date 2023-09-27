#pragma once
#include "new_constraint_builder.h"

struct BindingConstraintWeekData : public BindingConstraintData
{
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};
/*!
 * represent 'Hourly Binding Constraint' type
 */

class BindingConstraintWeek : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param cntCouplante : the binding constraint number
     */
    void add(int cntCouplante, std::shared_ptr<BindingConstraintWeekData> data);
};
