#include <antares/study.h>
#include <antares/emergency.h>

#include "../simulation/simulation.h"

#include "ortools_utils.h"
#include "mps_utils.h"

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

constexpr size_t OPT_APPEL_SOLVEUR_BUFFER_SIZE = 256;

/*
** Copyright 2007-2018 RTE
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
#include <antares/study.h>
#include <string>
#include <vector>
#include <algorithm>
#include "filename.h"

using namespace Yuni;

#define SEP IO::Separator

class ProblemConverter
{
public:
    void copyProbSimplexeToProbMps(PROBLEME_MPS* dest, PROBLEME_SIMPLEXE_NOMME* src)
    {
        // Variables
        dest->NbVar = src->NombreDeVariables;

        mVariableType.resize(src->NombreDeVariables);
        // TODO[FOM] use actual variable types when MIP resolution is integrated
        std::fill(mVariableType.begin(), mVariableType.end(), SRS_CONTINUOUS_VAR);
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
    }

private:
    std::vector<int> mVariableType;
};

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(
    PROBLEME_SIMPLEXE_NOMME* Prob,
    Solver::IResultWriter::Ptr writer,
    const std::string& filename)
{
    logs.info() << "Solver MPS File: `" << filename << "'";

    const auto tmpPath = generateTempPath(filename);

    auto mps = std::make_shared<PROBLEME_MPS>();
    {
        ProblemConverter
          converter; // This object must not be destroyed until SRSwritempsprob has been run
        converter.copyProbSimplexeToProbMps(mps.get(), Prob);
        SRSwritempsprob(mps.get(), tmpPath.c_str());
    }

    writer->addEntryFromFile(filename, tmpPath);

    removeTemporaryFile(tmpPath);
}

// --------------------
// Full mps writing
// --------------------
fullMPSwriter::fullMPSwriter(PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                             uint optNumber) :
    I_MPS_writer(optNumber),
    named_splx_problem_(named_splx_problem)
{}

void fullMPSwriter::runIfNeeded(Solver::IResultWriter::Ptr writer, const std::string & filename)
{
    OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(named_splx_problem_, writer, filename);
}

// ---------------------------------
// Full mps writing by or-tools
// ---------------------------------
fullOrToolsMPSwriter::fullOrToolsMPSwriter(MPSolver* solver,
                                           uint optNumber) :
    I_MPS_writer(optNumber),
    solver_(solver)
{
}
void fullOrToolsMPSwriter::runIfNeeded(Solver::IResultWriter::Ptr writer, const std::string & filename)
{
  ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(solver_,
                                                writer,
                                                filename);
}

mpsWriterFactory::mpsWriterFactory(PROBLEME_HEBDO* ProblemeHebdo,
                                   const int current_optim_number,
                                   PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                                   bool ortoolsUsed,
                                   MPSolver* solver) :
 pb_hebdo_(ProblemeHebdo),
 named_splx_problem_(named_splx_problem),
 ortools_used_(ortoolsUsed),
 solver_(solver),
 current_optim_number_(current_optim_number)
{
    export_mps_ = pb_hebdo_->ExportMPS;
    export_mps_on_error_ = pb_hebdo_->exportMPSOnError;
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
    if (ortools_used_)
    {
        return std::make_unique<fullOrToolsMPSwriter>(solver_,
                                                      current_optim_number_);
    }
    else
    {
        return std::make_unique<fullMPSwriter>(named_splx_problem_,
                                               current_optim_number_);
    }
}
