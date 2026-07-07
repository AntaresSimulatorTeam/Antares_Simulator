// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintGroup.h"

/**
 * @brief Group of reserve constraints
 *
 */

class ReserveParticipationGroup: public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;
    ReserveParticipationGroup(PROBLEME_HEBDO* problemeHebdo,
                              bool simulation,
                              ConstraintBuilder& builder);
    void BuildConstraints() override;

private:
    bool simulation_ = false;
    ReserveData GetReserveDataFromProblemHebdo();
};
