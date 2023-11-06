#pragma once
#include "ConstraintGroup.h"
#include "PMinDispatchableGeneration.h"
#include "PMaxDispatchableGeneration.h"

class PMinMaxDispatchableGenerationGroup : public ConstraintGroup
{
public:
    PMinMaxDispatchableGenerationGroup(PROBLEME_HEBDO* problemeHebdo, bool simulation) :
     simulation_(simulation), ConstraintGroup(problemeHebdo)
    {
    }

    /*TODO Rename this*/
    void Build() override;

private:
    bool simulation_ = false;
    std::shared_ptr<PMinDispatchableGenerationData>
      GetPMinDispatchableGenerationDataFromProblemHebdo(uint32_t pays, int index, int pdt);
    std::shared_ptr<PMaxDispatchableGenerationData>
      GetPMaxDispatchableGenerationDataFromProblemHebdo(uint32_t pays, int index, int pdt);
};