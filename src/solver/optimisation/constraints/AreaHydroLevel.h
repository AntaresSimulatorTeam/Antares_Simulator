#pragma once
#include "ConstraintBuilder.h"

struct AreaHydroLevelData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
};

/*!
 * represent 'Area Hydraulic Level' constraint type
 */

class AreaHydroLevel : private ConstraintFactory
{
public:
    AreaHydroLevel(ConstraintBuilder& builder, AreaHydroLevelData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(int pays, int pdt);

private:
    AreaHydroLevelData& data;
};
