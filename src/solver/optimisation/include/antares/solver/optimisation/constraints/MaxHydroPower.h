#pragma once
#include "ConstraintBuilder.h"

struct MaxHydroPowerData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMaxEnergieHydraulique;
};
/*!
 * represent 'Max Hydraulic Power' constraint type
 */
class MaxHydroPower : private ConstraintFactory
{
public:
    MaxHydroPower(ConstraintBuilder& builder, MaxHydroPowerData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);

private:
    MaxHydroPowerData& data;
};
