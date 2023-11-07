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

        void Build() override;

private:
    std::shared_ptr<NbDispUnitsMinBoundSinceMinUpTimeData>
      GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo(uint32_t pays, int index, int pdt);
};