#pragma once
#include "ConstraintBuilder.h"

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
    void add(int cntCouplante);
};