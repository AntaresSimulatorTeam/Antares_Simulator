#pragma once
#include "ConstraintGroup.h"
#include "MinHydroPower.h"
#include "MaxHydroPower.h"

class MinMaxHydroPowerGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

        void Build() override;

private:
    MinHydroPowerData GetMinHydroPowerData(uint32_t pays);
    MaxHydroPowerData GetMaxHydroPowerData(uint32_t pays);
};