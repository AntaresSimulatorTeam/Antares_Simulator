#pragma once
#include "../../constraints/new_constraint_builder.h"

#include "../../../simulation/adequacy_patch_runtime_data.h"
struct CsrAreaBalanceData
{
    const adqPatchParamsMode& areaMode;
    const int hour;
    const std::vector<int>& IndexDebutIntercoOrigine;
    const std::vector<int>& IndexSuivantIntercoOrigine;
    const std::vector<int>& IndexDebutIntercoExtremite;
    const std::vector<int>& IndexSuivantIntercoExtremite;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;
    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
    std::map<int, int> numberOfConstraintCsrAreaBalance;
};

class CsrAreaBalance : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    void add(uint32_t Area, std::shared_ptr<CsrAreaBalanceData> data);
};