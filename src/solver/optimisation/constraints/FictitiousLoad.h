
#pragma once
#include "new_constraint_builder.h"

struct FictitiousLoadData
{
    std::vector<int>& NumeroDeContraintePourEviterLesChargesFictives;
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays;
    const std::vector<bool>& DefaillanceNegativeUtiliserHydro;
};


/*!
 * represent 'Fictitious Load' constraint type
 */
class FictitiousLoad : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(int pdt, int pays, std::shared_ptr<FictitiousLoadData> data);
};
