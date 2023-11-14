/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "sim_structure_probleme_economique.h"
#include "ProblemMatrixEssential.h"
#include "constraints/ConstraintGroup.h"
#include "constraints/Group1.h"
#include "constraints/BindingConstraintDayGroup.h"
#include "constraints/BindingConstraintWeekGroup.h"
#include "constraints/HydroPowerGroup.h"
#include "constraints/HydraulicSmoothingGroup.h"
#include "constraints/MinMaxHydroPowerGroup.h"
#include "constraints/MaxPumpingGroup.h"
#include "constraints/AreaHydroLevelGroup.h"
#include "constraints/FinalStockGroup.h"

#include <antares/study/study.h>

using namespace Antares::Data;
class LinearProblemMatrix : public ProblemMatrixEssential
{
public:
    explicit LinearProblemMatrix(PROBLEME_HEBDO* problemeHebdo, Solver::IResultWriter& writer);

    void Run() override;
    void ExportStructures();

private:
    Solver::IResultWriter& writer_;
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