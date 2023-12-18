#pragma once
#include "ConstraintGroup.h"
#include "AreaHydroLevel.h"

class AreaHydroLevelGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void BuildConstraints() override;

private:
    AreaHydroLevelData GetAreaHydroLevelData();
};