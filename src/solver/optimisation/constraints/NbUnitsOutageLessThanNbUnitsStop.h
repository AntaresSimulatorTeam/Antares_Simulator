#pragma once
#include "new_constraint_builder.h"
struct NbUnitsOutageLessThanNbUnitsStopData : public StartUpCostsData
{
    std::vector<int>& NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
};
/*!
 * represent 'NbUnitsOutageLessThanNbUnitsStop' type
 */
class NbUnitsOutageLessThanNbUnitsStop : private NewConstraintFactory
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
    void add(int pays, std::shared_ptr<NbUnitsOutageLessThanNbUnitsStopData> data);
};