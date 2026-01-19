// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "ProblemMatrixEssential.h"
#include "constraints/AreaHydroLevelGroup.h"
#include "constraints/BindingConstraintDayGroup.h"
#include "constraints/BindingConstraintWeekGroup.h"
#include "constraints/ConstraintGroup.h"
#include "constraints/FinalStockGroup.h"
#include "constraints/Group1.h"
#include "constraints/HydraulicSmoothingGroup.h"
#include "constraints/HydroPowerGroup.h"
#include "constraints/MaxPumpingGroup.h"
#include "constraints/MinMaxHydroPowerGroup.h"

using namespace Antares::Data;

class LinearProblemMatrix final: public ProblemMatrixEssential
{
public:
    explicit LinearProblemMatrix(PROBLEME_HEBDO* problemeHebdo, ConstraintBuilder& builder);

    void Run() override;

private:
    ConstraintBuilder& builder_;
    Group1 group1_;
    BindingConstraintDayGroup bindingConstraintDayGroup_;
    BindingConstraintWeekGroup bindingConstraintWeekGroup_;
    HydroPowerGroup hydroPowerGroup_;
    HydraulicSmoothingGroup hydraulicSmoothingGroup_;
    MinMaxHydroPowerGroup minMaxHydroPowerGroup_;
    MaxPumpingGroup maxPumpingGroup_;
    AreaHydroLevelGroup areaHydroLevelGroup_;
    FinalStockGroup finalStockGroup_;
};
