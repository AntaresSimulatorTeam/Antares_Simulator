// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintGroup.h"
#include "PMaxDispatchableGeneration.h"
#include "PMinDispatchableGeneration.h"

class AbstractStartUpCostsGroup: public ConstraintGroup
{
public:
    AbstractStartUpCostsGroup(PROBLEME_HEBDO* problemeHebdo,
                              bool simulation,
                              ConstraintBuilder& builder);

    virtual void BuildConstraints() = 0;
    StartUpCostsData GetStartUpCostsDataFromProblemHebdo();

protected:
    bool simulation_ = false;
};
