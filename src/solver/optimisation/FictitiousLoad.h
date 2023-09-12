
#pragma once
#include "constraint_builder.h"
struct FictitiousLoadData
{
    std::vector<int>& NumeroDeContraintePourEviterLesChargesFictives;
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays;
    const std::vector<bool>& DefaillanceNegativeUtiliserHydro;
};
class FictitiousLoad : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int pays, FictitiousLoadData& data);
};
