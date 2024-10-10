/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/solver/utils/mps_utils.h"

#include <antares/study/study.h>
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares;
using namespace Antares::Data;

/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/
#include <algorithm>
#include <string>
#include <vector>

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_constants.h"
#include "antares/solver/utils/filename.h"
#include "antares/solver/utils/name_translator.h"

class ProblemConverter
{
public:
    void copyProbSimplexeToProbMps(PROBLEME_MPS* dest,
                                   PROBLEME_SIMPLEXE_NOMME* src,
                                   NameTranslator& nameTranslator)
    {
        // Variables
        dest->NbVar = src->NombreDeVariables;

        mVariableType.resize(src->NombreDeVariables);
        for (int var = 0; var < src->NombreDeVariables; var++)
        {
            mVariableType[var] = src->VariablesEntieres[var] ? SRS_INTEGER_VAR : SRS_CONTINUOUS_VAR;
        }

        dest->TypeDeVariable = mVariableType.data();
        dest->TypeDeBorneDeLaVariable = src->TypeDeVariable; // VARIABLE_BORNEE_DES_DEUX_COTES,
                                                             // VARIABLE_BORNEE_INFERIEUREMENT, etc.

        dest->Umax = src->Xmax;
        dest->Umin = src->Xmin;

        // Objective function
        dest->L = src->CoutLineaire;

        // Constraints (sparse)
        dest->NbCnt = src->NombreDeContraintes;
        dest->Mdeb = src->IndicesDebutDeLigne;
        dest->A = src->CoefficientsDeLaMatriceDesContraintes;
        dest->Nuvar = src->IndicesColonnes;
        dest->NbTerm = src->NombreDeTermesDesLignes;
        dest->B = src->SecondMembre;
        dest->SensDeLaContrainte = src->Sens;

        // Names
        dest->LabelDeLaVariable = nameTranslator.translate(src->VariableNames(), mVariableNames);
        dest->LabelDeLaContrainte = nameTranslator.translate(src->ConstraintNames(),
                                                             mConstraintNames);
    }

private:
    std::vector<int> mVariableType;
    std::vector<char*> mVariableNames;
    std::vector<char*> mConstraintNames;
};

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(PROBLEME_SIMPLEXE_NOMME* Prob,
                                               Solver::IResultWriter& writer,
                                               const std::string& filename)
{
    logs.info() << "Solver MPS File: `" << filename << "'";

    const auto tmpPath = generateTempPath(filename);

    auto mps = std::make_shared<PROBLEME_MPS>();
    {
        auto translator = NameTranslator::create(Prob->UseNamedProblems());
        ProblemConverter
          converter; // This object must not be destroyed until SRSwritempsprob has been run
        converter.copyProbSimplexeToProbMps(mps.get(), Prob, *translator);
        SRSwritempsprob(mps.get(), tmpPath.c_str());
    }

    writer.addEntryFromFile(filename, tmpPath);

    removeTemporaryFile(tmpPath);
}

// --------------------
// Full mps writing
// --------------------
fullMPSwriter::fullMPSwriter(PROBLEME_SIMPLEXE_NOMME* named_splx_problem, uint optNumber):
    I_MPS_writer(optNumber),
    named_splx_problem_(named_splx_problem)
{
}

void fullMPSwriter::runIfNeeded(Solver::IResultWriter& writer, const std::string& filename)
{
    OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(named_splx_problem_, writer, filename);
}

// ---------------------------------
// Full mps writing by or-tools
// ---------------------------------
fullOrToolsMPSwriter::fullOrToolsMPSwriter(MPSolver* solver, uint optNumber):
    I_MPS_writer(optNumber),
    solver_(solver)
{
}

void fullOrToolsMPSwriter::runIfNeeded(Solver::IResultWriter& writer, const std::string& filename)
{
    ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(solver_, writer, filename);
}

mpsWriterFactory::mpsWriterFactory(Data::mpsExportStatus exportMPS,
                                   bool exportMPSOnError,
                                   const int current_optim_number,
                                   PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                                   MPSolver* solver):
    export_mps_(exportMPS),
    export_mps_on_error_(exportMPSOnError),
    named_splx_problem_(named_splx_problem),
    solver_(solver),
    current_optim_number_(current_optim_number)
{
}

bool mpsWriterFactory::doWeExportMPS()
{
    switch (export_mps_)
    {
    case Data::mpsExportStatus::EXPORT_BOTH_OPTIMS:
        return true;
    case Data::mpsExportStatus::EXPORT_FIRST_OPTIM:
        return current_optim_number_ == PREMIERE_OPTIMISATION;
    case Data::mpsExportStatus::EXPORT_SECOND_OPTIM:
        return current_optim_number_ == DEUXIEME_OPTIMISATION;
    default:
        return false;
    }
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::create()
{
    if (doWeExportMPS())
    {
        return createFullmpsWriter();
    }

    return std::make_unique<nullMPSwriter>();
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::createOnOptimizationError()
{
    if (export_mps_on_error_ && not doWeExportMPS())
    {
        return createFullmpsWriter();
    }

    return std::make_unique<nullMPSwriter>();
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::createFullmpsWriter()
{
    return std::make_unique<fullOrToolsMPSwriter>(solver_, current_optim_number_);
}
