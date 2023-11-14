#pragma once
#include "../../constraints/ConstraintBuilder.h"

#include "../../../simulation/adequacy_patch_runtime_data.h"

struct CsrBindingConstraintHourData
{
    const std::vector<CONTRAINTES_COUPLANTES> MatriceDesContraintesCouplantes;
    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;
    const int hour;
    std::map<int, int>& numberOfConstraintCsrHourlyBinding;
};

class CsrBindingConstraintHour : private ConstraintFactory
{
public:
    CsrBindingConstraintHour(ConstraintBuilder& builder,
                             CsrBindingConstraintHourData& data) :
     ConstraintFactory(builder), data(data)
    {
    }
    void add(int CntCouplante);

private:
    CsrBindingConstraintHourData& data;
};