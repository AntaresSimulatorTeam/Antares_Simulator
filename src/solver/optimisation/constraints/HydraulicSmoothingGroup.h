#pragma once
#include "ConstraintGroup.h"

class HydraulicSmoothingGroup : public ConstraintGroup
{
public:
    explicit HydraulicSmoothingGroup(ConstraintBuilder& builder, char typeDeLissageHydraulique) :
     ConstraintGroup(builder), typeDeLissageHydraulique_(typeDeLissageHydraulique)
    {
    }

    void Build() override;

private:
    char typeDeLissageHydraulique_;
};