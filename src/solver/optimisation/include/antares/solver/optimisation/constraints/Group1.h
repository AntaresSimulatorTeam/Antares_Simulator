// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AreaBalance.h"
#include "BindingConstraintHour.h"
#include "ConstraintGroup.h"
#include "FictitiousLoad.h"
#include "FlowDissociation.h"
#include "ShortTermStorageLevel.h"

class Group1 final: public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void BuildConstraints() override;

private:
    AreaBalanceData GetAreaBalanceData();
    FictitiousLoadData GetFictitiousLoadData();
    ShortTermStorageData GetShortTermStorageData();

    ShortTermStorageCumulativeConstraintData GetShortTermStorageCumulativeConstraintData();

    FlowDissociationData GetFlowDissociationData();
    BindingConstraintHourData GetBindingConstraintHourData();
};
