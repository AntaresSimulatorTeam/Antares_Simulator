#pragma once
#include "new_constraint_builder.h"
struct MaxPumpingData
{
    const bool& PresenceDePompageModulable;
    std::vector<int>& NumeroDeContrainteMaxPompage;
};
/*!
 * represent 'Max Pumping' constraint type
 */

class MaxPumping : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, std::shared_ptr<MaxPumpingData> data);
};
