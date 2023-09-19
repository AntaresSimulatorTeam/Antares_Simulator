#pragma once
#include "ConstraintGroup.h"
#include "HydroPower.h"

class HydroPowerGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<HydroPowerData> GetHydroPowerDataFromProblemHebdo(uint32_t pays);
};