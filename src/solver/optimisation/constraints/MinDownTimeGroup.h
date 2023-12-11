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

        void buildConstraints() override;

private:
    MinDownTimeData GetMinDownTimeDataFromProblemHebdo();
};