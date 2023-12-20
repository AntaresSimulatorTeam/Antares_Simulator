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

    virtual ~AbstractStartUpCostsGroup() = default;
    void BuildConstraints() override = 0;
    bool simulation_ = false;
    StartUpCostsData GetStartUpCostsDataFromProblemHebdo();
};