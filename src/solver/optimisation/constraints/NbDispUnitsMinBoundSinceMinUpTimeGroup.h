#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "NbDispUnitsMinBoundSinceMinUpTime.h"

class NbDispUnitsMinBoundSinceMinUpTimeGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<NbDispUnitsMinBoundSinceMinUpTimeData>
      GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo(uint32_t pays, int index, int pdt);
};