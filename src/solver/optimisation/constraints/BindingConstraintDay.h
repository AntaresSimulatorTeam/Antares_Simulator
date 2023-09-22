#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Daily Binding Constraint' type
 */
class BindingConstraintDay : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param cntCouplante : the binding constraint number
     */
    void add(int cntCouplante);
};
