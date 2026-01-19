// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "constraint_builder_utils.h"

class ConstraintGroup
{
public:
    explicit ConstraintGroup(PROBLEME_HEBDO* problemeHebdo, ConstraintBuilder& builder):
        problemeHebdo_(problemeHebdo),
        builder_(builder)
    {
    }

    virtual ~ConstraintGroup() = default;
    virtual void BuildConstraints() = 0;
    PROBLEME_HEBDO* problemeHebdo_;
    ConstraintBuilder& builder_;
};
