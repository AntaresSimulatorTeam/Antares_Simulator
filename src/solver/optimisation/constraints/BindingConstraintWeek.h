#pragma once
#include "ConstraintBuilder.h"

struct BindingConstraintWeekData : public BindingConstraintData
{
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};
/*!
 * represent 'Hourly Binding Constraint' type
 */

class BindingConstraintWeek : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param cntCouplante : the binding constraint number
     */
    void add(int cntCouplante, std::shared_ptr<BindingConstraintWeekData> data);
};
