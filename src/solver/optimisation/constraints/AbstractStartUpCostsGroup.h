#pragma once
#include "ConstraintGroup.h"
#include "PMinDispatchableGeneration.h"
#include "PMaxDispatchableGeneration.h"

class AbstractStartUpCostsGroup : public ConstraintGroup
{
public:
    AbstractStartUpCostsGroup(PROBLEME_HEBDO* problemeHebdo,
                              bool simulation,
                              ConstraintBuilder& builder);

    virtual void BuildConstraints() = 0;
    StartUpCostsData GetStartUpCostsDataFromProblemHebdo();

protected:
    bool simulation_ = false;
};
