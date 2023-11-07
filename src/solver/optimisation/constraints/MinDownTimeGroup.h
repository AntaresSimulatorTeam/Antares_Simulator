#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "MinDownTime.h"

/**
 * @brief Group of MinDownTime constraints
 *
 */

class MinDownTimeGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

        void Build() override;

private:
    std::shared_ptr<MinDownTimeData> GetMinDownTimeDataFromProblemHebdo(uint32_t pays,
                                                                        int index,
                                                                        int pdt);
};