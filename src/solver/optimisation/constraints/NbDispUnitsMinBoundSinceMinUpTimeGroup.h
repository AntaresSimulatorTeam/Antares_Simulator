#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "NbDispUnitsMinBoundSinceMinUpTime.h"

/**
 * @brief Group of NbDispUnitsMinBoundSinceMinUpTime constraints
 *
 */

class NbDispUnitsMinBoundSinceMinUpTimeGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

        void buildConstraints() override;

private:
    NbDispUnitsMinBoundSinceMinUpTimeData
      GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo();
};