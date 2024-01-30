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
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "constraints/ConstraintGroup.h"
#include "ProblemMatrixEssential.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "constraints/PMinMaxDispatchableGenerationGroup.h"
#include "constraints/ConsistenceNumberOfDispatchableUnitsGroup.h"
#include "constraints/NbUnitsOutageLessThanNbUnitsStopGroup.h"
#include "constraints/NbDispUnitsMinBoundSinceMinUpTimeGroup.h"
#include "constraints/MinDownTimeGroup.h"

#include <antares/study/study.h>

using namespace Antares::Data;

class LinearProblemMatrixStartUpCosts : public ProblemMatrixEssential
{
public:
    explicit LinearProblemMatrixStartUpCosts(PROBLEME_HEBDO* problemeHebdo,
                                             bool Simulation,
                                             ConstraintBuilder& builder);

private:
    bool simulation_ = false;
    PMinMaxDispatchableGenerationGroup pMinMaxDispatchableGenerationGroup_;
    ConsistenceNumberOfDispatchableUnitsGroup consistenceNumberOfDispatchableUnitsGroup_;
    NbUnitsOutageLessThanNbUnitsStopGroup nbUnitsOutageLessThanNbUnitsStopGroup_;
    NbDispUnitsMinBoundSinceMinUpTimeGroup nbDispUnitsMinBoundSinceMinUpTimeGroup_;
    MinDownTimeGroup minDownTimeGroup_;
};