#pragma once
#include "ConstraintGroup.h"
#include "AreaHydroLevel.h"

class AreaHydroLevelGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<AreaHydroLevelData> GetAreaHydroLevelData(int pdt, uint32_t pays);
};