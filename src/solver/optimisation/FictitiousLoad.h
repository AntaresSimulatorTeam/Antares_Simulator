
#pragma once
#include "new_constraint_builder.h"

struct FictitiousLoadData
{
    std::vector<int>& NumeroDeContraintePourEviterLesChargesFictives;
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays;
    const std::vector<bool>& DefaillanceNegativeUtiliserHydro;
};
class FictitiousLoad : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pdt, int pays, std::shared_ptr<FictitiousLoadData> data);
};
