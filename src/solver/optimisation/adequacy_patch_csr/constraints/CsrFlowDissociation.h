#pragma once
#include "../../constraints/new_constraint_builder.h"

struct CsrFlowDissociationData
{
    std::map<int, int>& numberOfConstraintCsrFlowDissociation;
    const uint32_t NombreDInterconnexions;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;

    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
};
class CsrFlowDissociation : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    void add(int hour, std::shared_ptr<CsrFlowDissociationData> data);
};