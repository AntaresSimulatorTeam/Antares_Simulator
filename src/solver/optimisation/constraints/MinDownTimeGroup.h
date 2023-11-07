#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "MinDownTime.h"

class MinDownTimeGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<MinDownTimeData> GetMinDownTimeDataFromProblemHebdo(uint32_t pays,
                                                                        int index,
                                                                        int pdt);
};