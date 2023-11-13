#pragma once
#include "ConstraintBuilder.h"

struct MinDownTimeData : public StartUpCostsData
{
    std::vector<int>& NumeroDeContrainteDesContraintesDeDureeMinDArret;
};

/*!
 * represent 'MinDownTime' Constraint type
 */
class MinDownTime : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, std::shared_ptr<MinDownTimeData> data);
};