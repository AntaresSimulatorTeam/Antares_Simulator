// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <vector>

#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

struct CsrFictitiousLoadData
{
    const std::vector<adqPatchParamsMode>& areaMode;
    const int hour;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    const std::vector<bool>& DefaillanceNegativeUtiliserHydro;
    const std::vector<bool>& DefaillanceNegativeUtiliserConsoAbattue;
    const std::vector<bool>& DefaillanceNegativeUtiliserPMinThermique;
    std::map<int, int>& numberOfConstraintCsrFictitiousLoad;
    const uint32_t NombreDePays;
};

class CsrFictitiousLoad: private ConstraintFactory
{
public:
    CsrFictitiousLoad(ConstraintBuilder& builder, CsrFictitiousLoadData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    void add();

private:
    CsrFictitiousLoadData& data;
};
