#pragma once
#include "ConstraintBuilder.h"
struct NbDispUnitsMinBoundSinceMinUpTimeData : public StartUpCostsData
{
    std::vector<int>& NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
};
/*!
 * represent 'Number of Dispatchable Units Min Bound Since Min Up Time' type
 */
class NbDispUnitsMinBoundSinceMinUpTime : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, std::shared_ptr<NbDispUnitsMinBoundSinceMinUpTimeData> data);
};