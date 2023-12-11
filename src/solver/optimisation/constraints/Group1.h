#pragma once
#include "ConstraintGroup.h"
#include "AreaBalance.h"
#include "FictitiousLoad.h"
#include "ShortTermStorageLevel.h"
#include "FlowDissociation.h"
#include "BindingConstraintHour.h"

class Group1 : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void BuildConstraints() override;

private:
    AreaBalanceData GetAreaBalanceData();
    FictitiousLoadData GetFictitiousLoadData();
    ShortTermStorageLevelData GetShortTermStorageLevelData();
    FlowDissociationData GetFlowDissociationData();
    BindingConstraintHourData GetBindingConstraintHourData();
};