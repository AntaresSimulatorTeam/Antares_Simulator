// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <vector>

#include "antares/solver/adequacy-patch/gems-csr-adapter.h"
#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

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
    // Optional GEMS exchange contributions; nullptr when no GEMS model is active.
    const std::vector<Antares::AdequacyPatch::AreaFlowContribution>* gemsAreaFlowContribs = nullptr;
};

class CsrAreaBalance final: private ConstraintFactory
{
public:
    CsrAreaBalance(ConstraintBuilder& builder, CsrAreaBalanceData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    void add();

private:
    CsrAreaBalanceData& data;
};
