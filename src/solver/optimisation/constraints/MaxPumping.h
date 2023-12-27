#pragma once
#include "ConstraintBuilder.h"
struct MaxPumpingData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    std::vector<int>& NumeroDeContrainteMaxPompage;
};
/*!
 * represent 'Max Pumping' constraint type
 */

class MaxPumping : private ConstraintFactory
{
public:
    MaxPumping(ConstraintBuilder& builder, MaxPumpingData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);

private:
    MaxPumpingData& data;
};
