#pragma once
#include "../../constraints/ConstraintBuilder.h"

#include "../../../simulation/adequacy_patch_runtime_data.h"
struct CsrFlowDissociationData
{
    std::map<int, int>& numberOfConstraintCsrFlowDissociation;
    const uint32_t NombreDInterconnexions;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;

    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
    const int hour;
};
class CsrFlowDissociation : private ConstraintFactory
{
public:
    CsrFlowDissociation(ConstraintBuilder& builder, CsrFlowDissociationData& data) :
     ConstraintFactory(builder), data(data)
    {
    }
    void add();

private:
    CsrFlowDissociationData& data;
};
