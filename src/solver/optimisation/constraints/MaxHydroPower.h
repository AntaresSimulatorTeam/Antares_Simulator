#pragma once
#include "new_constraint_builder.h"

struct MaxHydroPowerData
{
    const bool& presenceHydro;
    const bool& TurbEntreBornes;
    const bool& PresenceDePompageModulable;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMaxEnergieHydraulique;
};
/*!
 * represent 'Max Hydraulic Power' constraint type
 */
class MaxHydroPower : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, std::shared_ptr<MaxHydroPowerData> data);
};
