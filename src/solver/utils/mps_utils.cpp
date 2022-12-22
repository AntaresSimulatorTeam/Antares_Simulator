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
#include "filename.h"

using namespace Yuni;

#define SEP IO::Separator

void copyProbSimplexeToProbMps(PROBLEME_MPS *dest, PROBLEME_SIMPLEXE *src)
{
    dest->NbVar = src->NombreDeVariables;
    dest->NbCnt = src->NombreDeContraintes;

    dest->Mdeb = src->IndicesDebutDeLigne;
    dest->A = src->CoefficientsDeLaMatriceDesContraintes;
    dest->Nuvar = src->IndicesColonnes;
    dest->NbTerm = src->NombreDeTermesDesLignes;
    dest->B = src->SecondMembre;
    dest->SensDeLaContrainte = src->Sens;
	dest->VariablesDualesDesContraintes = src->CoutsMarginauxDesContraintes;

    dest->TypeDeVariable = src->TypeDeVariable;
    dest->TypeDeBorneDeLaVariable = src->TypeDeVariable;
    dest->U = src->X;
    dest->L = src->CoutLineaire;
    dest->Umax = src->Xmax;
    dest->Umin = src->Xmin;

}

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void* Prob, int optNumber, uint numSpace
    , Solver::IResultWriter::Ptr writer)
{
    const auto filename = getFilenameWithExtension("problem", "mps", numSpace, optNumber);
    const auto tmpPath = generateTempPath(filename);

    auto mps = std::make_shared<PROBLEME_MPS>();
    copyProbSimplexeToProbMps(mps.get(), (PROBLEME_SIMPLEXE*)Prob);
    SRSwritempsprob(mps.get(), tmpPath.c_str());

    writer->addEntryFromFile(filename, tmpPath);

    removeTemporaryFile(tmpPath);
}

// --------------------
// Full mps writing
// --------------------
fullMPSwriter::fullMPSwriter(PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                             int optNumber,
                             uint thread_number) :
 named_splx_problem_(named_splx_problem),
 current_optim_number_(optNumber),
 thread_number_(thread_number)
{
}
void fullMPSwriter::runIfNeeded(Solver::IResultWriter::Ptr writer)
{
    OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(
      (void*)named_splx_problem_, current_optim_number_, thread_number_, writer);
}

// ---------------------------------
// Full mps writing by or-tools
// ---------------------------------
fullOrToolsMPSwriter::fullOrToolsMPSwriter(MPSolver* solver, int optNumber, uint thread_number) :
 solver_(solver), current_optim_number_(optNumber), thread_number_(thread_number)
{
}
void fullOrToolsMPSwriter::runIfNeeded(Solver::IResultWriter::Ptr writer)
{
    // Make or-tools print the MPS files leads to a crash !
    ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(solver_, thread_number_, current_optim_number_, writer);
}

mpsWriterFactory::mpsWriterFactory(PROBLEME_HEBDO* ProblemeHebdo,
                                   int NumIntervalle,
                                   PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                                   bool ortoolsUsed,
                                   MPSolver* solver,
                                   uint thread_number) :
 pb_hebdo_(ProblemeHebdo),
 num_intervalle_(NumIntervalle),
 named_splx_problem_(named_splx_problem),
 ortools_used_(ortoolsUsed),
 solver_(solver),
 thread_number_(thread_number)
{
    current_optim_number_ = pb_hebdo_->numeroOptimisation[num_intervalle_];
    export_mps_ = pb_hebdo_->ExportMPS;
    export_mps_on_error_ = pb_hebdo_->exportMPSOnError;
    is_first_week_of_year_ = pb_hebdo_->firstWeekOfSimulation;
}

bool mpsWriterFactory::doWeExportMPS()
{
    switch (export_mps_)
    {
    case Data::mpsExportStatus::EXPORT_BOTH_OPTIMS:
        return true;
    case Data::mpsExportStatus::EXPORT_FIRST_OPIM:
        return current_optim_number_ == PREMIERE_OPTIMISATION;
    case Data::mpsExportStatus::EXPORT_SECOND_OPIM:
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
        return std::make_unique<fullOrToolsMPSwriter>(
          solver_, current_optim_number_, thread_number_);
    }
    else
    {
        return std::make_unique<fullMPSwriter>(
          named_splx_problem_, current_optim_number_, thread_number_);
    }
}
