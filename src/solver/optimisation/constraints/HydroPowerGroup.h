#pragma once
#include "ConstraintGroup.h"
#include "HydroPower.h"

class HydroPowerGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void Build() override;

private:
    HydroPowerData GetHydroPowerDataFromProblemHebdo(uint32_t pays);
};