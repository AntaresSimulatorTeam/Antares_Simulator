#pragma once
#include "new_constraint_builder.h"
struct HydroPowerData
{
    const bool& presenceHydro;
    const bool& TurbEntreBornes;
    const bool& presencePompage;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteEnergieHydraulique;
    const double& pumpingRatio;
};
/*!
 * represent 'Hydraulic Power' constraint type
 */

class HydroPower : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, std::shared_ptr<HydroPowerData> data);
};
