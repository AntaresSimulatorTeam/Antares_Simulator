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

void LinearProblemMatrix::ExportStructures()
{
    if (problemeHebdo_->ExportStructure && problemeHebdo_->firstWeekOfSimulation)
    {
        OPT_ExportInterco(writer_, problemeHebdo_);
        OPT_ExportAreaName(writer_, problemeHebdo_->NomsDesPays);
    }
}
void LinearProblemMatrix::InitiliazeProblemAResoudreCounters()
{
    auto& ProblemeAResoudre = problemeHebdo_->ProblemeAResoudre;
    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
}

void LinearProblemMatrix::Run()
{
    InitiliazeProblemAResoudreCounters();

    LinearProblemMatrixEssential::Run();

    if (problemeHebdo_->OptimisationAvecCoutsDeDemarrage)
    {
        LinearProblemMatrixStartUpCosts(problemeHebdo_, false).Run();
    }

    return;
}
