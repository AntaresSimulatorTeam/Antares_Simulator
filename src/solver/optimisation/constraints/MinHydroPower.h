#pragma once
#include "new_constraint_builder.h"

struct MinHydroPowerData
{
    const bool& presenceHydro;
    const bool& TurbEntreBornes;
    const bool& PresenceDePompageModulable;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMinEnergieHydraulique;
};
/*!
 * represent 'Min Hydraulic Power' constraint type
 */

class MinHydroPower : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, std::shared_ptr<MinHydroPowerData> data);
};
