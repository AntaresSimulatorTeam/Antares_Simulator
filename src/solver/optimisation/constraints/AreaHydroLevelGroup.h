#pragma once
#include "ConstraintGroup.h"
#include "AreaHydroLevel.h"

class AreaHydroLevelGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void Build() override;

private:
    AreaHydroLevelData GetAreaHydroLevelData(int pdt, uint32_t pays);
};