#pragma once
#include "ConstraintGroup.h"
#include "PMinDispatchableGeneration.h"
#include "PMaxDispatchableGeneration.h"

class AbstractStartUpCostsGroup : public ConstraintGroup
{
public:
    AbstractStartUpCostsGroup(PROBLEME_HEBDO* problemeHebdo, bool simulation) :
     simulation_(simulation), ConstraintGroup(problemeHebdo)
    {
    }

        void Build() = 0;

    bool simulation_ = false;
    std::shared_ptr<StartUpCostsData> GetStartUpCostsDataFromProblemHebdo(uint32_t pays,
                                                                          int index,
                                                                          int pdt);
};