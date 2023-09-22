#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Flow Dissociation' constraint type
 */
class FlowDissociation : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param interco : interconnection number
     */
    void add(int pdt, int interco);
};
