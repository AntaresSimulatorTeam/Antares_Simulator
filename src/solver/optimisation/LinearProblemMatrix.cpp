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

#include "LinearProblemMatrix.h"
#include "opt_export_structure.h"
#include "../utils/filename.h"
#include "opt_fonctions.h"
#include "LinearProblemMatrixStartUpCosts.h"

using namespace Antares::Data;

LinearProblemMatrix::LinearProblemMatrix(ConstraintBuilder& builder,
                                         Solver::IResultWriter& writer,
                                         bool optimisationAvecCoutsDeDemarrage,
                                         char typeDeLissageHydraulique) :
 ProblemMatrixEssential(builder),
 writer_(writer),
 optimisationAvecCoutsDeDemarrage_(optimisationAvecCoutsDeDemarrage),
 group1_(builder),
 bindingConstraintDayGroup_(builder),
 bindingConstraintWeekGroup_(builder),
 hydroPowerGroup_(builder),
 hydraulicSmoothingGroup_(builder, typeDeLissageHydraulique),
 minMaxHydroPowerGroup_(builder),
 maxPumpingGroup_(builder),
 areaHydroLevelGroup_(builder),
 finalStockGroup_(builder)
{
    constraintgroups_ = {&group1_,
                         &bindingConstraintDayGroup_,
                         &bindingConstraintWeekGroup_,
                         &hydroPowerGroup_,
                         &hydraulicSmoothingGroup_,
                         &minMaxHydroPowerGroup_,
                         &maxPumpingGroup_,
                         &areaHydroLevelGroup_,
                         &finalStockGroup_};
}
void LinearProblemMatrix::Run()
{
    ProblemMatrixEssential::Run();

    if (optimisationAvecCoutsDeDemarrage_)
    {
        LinearProblemMatrixStartUpCosts(builder_, false).Run();
    }

    return;
}
