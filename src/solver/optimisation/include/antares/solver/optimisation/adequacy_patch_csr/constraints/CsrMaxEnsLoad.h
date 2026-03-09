// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <vector>

#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

struct CsrMaxEnsLoadData
{
    const std::vector<adqPatchParamsMode>& areaMode;
    const int hour;
    const uint32_t NombreDePays;
    std::map<int, int>& numberOfConstraintCsrMaxEnsLoad;
};

class CsrMaxEnsLoad: private ConstraintFactory
{
public:
    CsrMaxEnsLoad(ConstraintBuilder& builder, CsrMaxEnsLoadData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    void add();

private:
    CsrMaxEnsLoadData& data;
};
