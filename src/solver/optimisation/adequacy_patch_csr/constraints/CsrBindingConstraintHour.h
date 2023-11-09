#pragma once
#include "../../constraints/new_constraint_builder.h"

#include "../../../simulation/adequacy_patch_runtime_data.h"

struct CsrBindingConstraintHourData
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;
    const int hour;

    std::map<int, int>& numberOfConstraintCsrHourlyBinding;
};

class CsrBindingConstraintHour : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    void add(int CntCouplante, std::shared_ptr<CsrBindingConstraintHourData> data);
};