#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "NbUnitsOutageLessThanNbUnitsStop.h"

class NbUnitsOutageLessThanNbUnitsStopGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<NbUnitsOutageLessThanNbUnitsStopData>
      GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo(uint32_t pays, int index, int pdt);
};