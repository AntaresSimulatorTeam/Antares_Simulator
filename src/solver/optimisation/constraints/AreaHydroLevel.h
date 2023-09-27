#pragma once
#include "new_constraint_builder.h"

struct AreaHydroLevelData
{
    std::vector<int>& NumeroDeContrainteDesNiveauxPays;
    const bool& SuiviNiveauHoraire;
    const double& PumpingRatio;
};

/*!
 * represent 'Area Hydraulic Level' constraint type
 */

class AreaHydroLevel : private NewConstraintFactory
{
    public:
        using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
        void add(int pays, int pdt, std::shared_ptr<AreaHydroLevelData> data);
};
