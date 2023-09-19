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

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<AreaBalanceData> GetAreaBalanceData(int pdt, uint32_t pays);
    std::shared_ptr<FictitiousLoadData> GetFictitiousLoadData(int pdt, uint32_t pays);
    std::shared_ptr<ShortTermStorageLevelData> GetShortTermStorageLevelData(int pdt);
    std::shared_ptr<FlowDissociationData> GetFlowDissociationData(int pdt);
    std::shared_ptr<BindingConstraintHourData> GetBindingConstraintHourData(int pdt,
                                                                            int cntCouplante);
};