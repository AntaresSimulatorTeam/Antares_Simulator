#pragma once
#include "ConstraintGroup.h"
#include "MinHydroPower.h"
#include "MaxHydroPower.h"

class MinMaxHydroPowerGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

        void BuildConstraints() override;

private:
    MinHydroPowerData GetMinHydroPowerData();
    MaxHydroPowerData GetMaxHydroPowerData();
};