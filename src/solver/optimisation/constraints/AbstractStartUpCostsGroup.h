#pragma once
#include "ConstraintGroup.h"
#include "PMinDispatchableGeneration.h"
#include "PMaxDispatchableGeneration.h"

class AbstractStartUpCostsGroup : public ConstraintGroup
{
public:
    AbstractStartUpCostsGroup(PROBLEME_HEBDO* problemeHebdo,
                              bool simulation,
                              ConstraintBuilder& builder) :
     simulation_(simulation), ConstraintGroup(problemeHebdo, builder)
    {
    }

        void BuildConstraints() = 0;

    bool simulation_ = false;
    StartUpCostsData GetStartUpCostsDataFromProblemHebdo();
};