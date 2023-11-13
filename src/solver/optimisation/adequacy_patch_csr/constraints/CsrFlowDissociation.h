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
};
class CsrFlowDissociation : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int hour, std::shared_ptr<CsrFlowDissociationData> data);
};
