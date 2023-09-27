#pragma once
#include "ConstraintGroup.h"
#include "MinHydroPower.h"
#include "MaxHydroPower.h"

class MinMaxHydroPowerGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<MinHydroPowerData> GetMinHydroPowerData(uint32_t pays);
    std::shared_ptr<MaxHydroPowerData> GetMaxHydroPowerData(uint32_t pays);
};