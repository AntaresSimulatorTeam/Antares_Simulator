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
    NbUnitsOutageLessThanNbUnitsStopData GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo();
};