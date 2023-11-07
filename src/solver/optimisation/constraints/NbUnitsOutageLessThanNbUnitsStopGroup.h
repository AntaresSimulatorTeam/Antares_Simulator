#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "NbUnitsOutageLessThanNbUnitsStop.h"

/**
 * @brief Group of NbUnitsOutageLessThanNbUnitsStop constraints
 *
 */

class NbUnitsOutageLessThanNbUnitsStopGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

        void Build() override;

private:
    std::shared_ptr<NbUnitsOutageLessThanNbUnitsStopData>
      GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo(uint32_t pays, int index, int pdt);
};