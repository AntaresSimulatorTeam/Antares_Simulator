// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

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

class CsrFlowDissociation final: private ConstraintFactory
{
public:
    CsrFlowDissociation(ConstraintBuilder& builder, CsrFlowDissociationData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    void add();

private:
    CsrFlowDissociationData& data;
};
