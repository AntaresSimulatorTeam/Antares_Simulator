#pragma once
#include "../../constraints/ConstraintBuilder.h"

#include "../../../simulation/adequacy_patch_runtime_data.h"
struct CsrAreaBalanceData
{
    const std::vector<adqPatchParamsMode>& areaMode;
    const int hour;
    const std::vector<int>& IndexDebutIntercoOrigine;
    const std::vector<int>& IndexSuivantIntercoOrigine;
    const std::vector<int>& IndexDebutIntercoExtremite;
    const std::vector<int>& IndexSuivantIntercoExtremite;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;
    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
    std::map<int, int>& numberOfConstraintCsrAreaBalance;
    const uint32_t NombreDePays;
};

class CsrAreaBalance : private ConstraintFactory
{
public:
    CsrAreaBalance(ConstraintBuilder& builder, CsrAreaBalanceData& data) :
     ConstraintFactory(builder), data(data)
    {
    }
    void add();

private:
    CsrAreaBalanceData& data;
};