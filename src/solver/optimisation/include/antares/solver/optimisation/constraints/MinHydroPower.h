#pragma once
#include "ConstraintBuilder.h"

struct MinHydroPowerData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMinEnergieHydraulique;
};
/*!
 * represent 'Min Hydraulic Power' constraint type
 */

class MinHydroPower : private ConstraintFactory
{
public:
    MinHydroPower(ConstraintBuilder& builder, MinHydroPowerData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);

private:
    MinHydroPowerData& data;
};
