#include <antares/study.h>
#include <antares/emergency.h>

#include "../simulation/simulation.h"

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

static void printHeader(Clob& buffer, int NombreDeVariables, int NombreDeContraintes)
{
    buffer.appendFormat("* Number of variables:   %d\n", NombreDeVariables);
    buffer.appendFormat("* Number of constraints: %d\n", NombreDeContraintes);
    buffer.appendFormat("NAME          Pb Solve\n");
}

static void printColumnsObjective(Clob& buffer,
                                  int NombreDeVariables,
                                  const int* NumeroDeContrainte,
                                  const double* CoefficientsDeLaMatriceDesContraintes,
                                  const int* Cdeb,
                                  const int* Csui,
                                  const double* CoutLineaire)
{
    char printBuffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];
    int il;

    buffer.appendFormat("COLUMNS\n");
    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (CoutLineaire && CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", CoutLineaire[Var]);
            buffer.appendFormat("    C%07d  OBJECTIF  %s\n", Var, printBuffer);
        }

        il = Cdeb[Var];
        while (il >= 0)
        {
            SNPRINTF(printBuffer,
                     OPT_APPEL_SOLVEUR_BUFFER_SIZE,
                     "%-.10lf",
                     CoefficientsDeLaMatriceDesContraintes[il]);
            buffer.appendFormat("    C%07d  R%07d  %s\n", Var, NumeroDeContrainte[il], printBuffer);
            il = Csui[il];
        }
    }
}

static void printBounds(Clob& buffer,
                        int NombreDeVariables,
                        const int* TypeDeBorneDeLaVariable,
                        const double* Xmin,
                        const double* Xmax)
{
    char printBuffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    buffer.appendFormat("BOUNDS\n");

    for (int Var = 0; Var < NombreDeVariables; Var++)
    {
        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_FIXE)
        {
            SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
            buffer.appendFormat(" FX BNDVALUE  C%07d  %s\n", Var, printBuffer);
            continue;
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                buffer.appendFormat(" LO BNDVALUE  C%07d  %s\n", Var, printBuffer);
            }

            SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
            buffer.appendFormat(" UP BNDVALUE  C%07d  %s\n", Var, printBuffer);
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_INFERIEUREMENT)
        {
            if (Xmin[Var] != 0.0)
            {
                SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmin[Var]);
                buffer.appendFormat(" LO BNDVALUE  C%07d  %s\n", Var, printBuffer);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_SUPERIEUREMENT)
        {
            buffer.appendFormat(" MI BNDVALUE  C%07d\n", Var);
            if (Xmax[Var] != 0.0)
            {
                SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", Xmax[Var]);
                buffer.appendFormat(" UP BNDVALUE  C%07d  %s\n", Var, printBuffer);
            }
        }

        if (TypeDeBorneDeLaVariable[Var] == VARIABLE_NON_BORNEE)
        {
            buffer.appendFormat(" FR BNDVALUE  C%07d\n", Var);
        }
    }
}

static void printRHS(Clob& buffer, int NombreDeContraintes, const double* SecondMembre)
{
    char printBuffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    buffer.appendFormat("RHS\n");
    for (int Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (SecondMembre[Cnt] != 0.0)
        {
            SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.9lf", SecondMembre[Cnt]);
            buffer.appendFormat("    RHSVAL    R%07d  %s\n", Cnt, printBuffer);
        }
    }
}

void OPT_dump_spx_fixed_part(const PROBLEME_SIMPLEXE* Pb, int optNumber, uint numSpace)
{
    Clob buffer;
    int Cnt;
    int Var;
    int il;
    int ilk;
    int ilMax;
    int* Cder;
    int* Cdeb;
    int* NumeroDeContrainte;
    int* Csui;

    for (ilMax = -1, Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        if ((Pb->IndicesDebutDeLigne[Cnt] + Pb->NombreDeTermesDesLignes[Cnt] - 1) > ilMax)
        {
            ilMax = Pb->IndicesDebutDeLigne[Cnt] + Pb->NombreDeTermesDesLignes[Cnt] - 1;
        }
    }

    ilMax += Pb->NombreDeContraintes;

    if (ilMax < 0)
    {
        logs.fatal() << "Invalid size detected";
        return;
    }

    Cder = (int*)malloc(Pb->NombreDeVariables * sizeof(int));
    Cdeb = (int*)malloc(Pb->NombreDeVariables * sizeof(int));
    NumeroDeContrainte = (int*)malloc(ilMax * sizeof(int));
    Csui = (int*)malloc(ilMax * sizeof(int));

    if (Cder == nullptr || Cdeb == nullptr || NumeroDeContrainte == nullptr || Csui == nullptr)
    {
        logs.fatal() << "Not enough memory";
        AntaresSolverEmergencyShutdown(2);
    }

    for (Var = 0; Var < Pb->NombreDeVariables; Var++)
        Cdeb[Var] = -1;

    for (Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        il = Pb->IndicesDebutDeLigne[Cnt];
        ilMax = il + Pb->NombreDeTermesDesLignes[Cnt];
        while (il < ilMax)
        {
            Var = Pb->IndicesColonnes[il];
            if (Cdeb[Var] < 0)
            {
                Cdeb[Var] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }
            else
            {
                ilk = Cder[Var];
                Csui[ilk] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }

            il++;
        }
    }

    free(Cder);

    printHeader(buffer, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    buffer.appendFormat("ROWS\n");
    buffer.appendFormat(" N  OBJECTIF\n");

    for (Cnt = 0; Cnt < Pb->NombreDeContraintes; Cnt++)
    {
        if (Pb->Sens[Cnt] == '=')
        {
            buffer.appendFormat(" E  R%07d\n", Cnt);
        }
        else if (Pb->Sens[Cnt] == '<')
        {
            buffer.appendFormat(" L  R%07d\n", Cnt);
        }
        else if (Pb->Sens[Cnt] == '>')
        {
            buffer.appendFormat(" G  R%07d\n", Cnt);
        }
        else
        {
            buffer.appendFormat(
              "Writing fixed part of MPS data : le sens de la contrainte %c ne fait pas "
              "partie des sens reconnus\n",
              Pb->Sens[Cnt]);
            AntaresSolverEmergencyShutdown();
        }
    }

    printColumnsObjective(buffer,
                          Pb->NombreDeVariables,
                          NumeroDeContrainte,
                          Pb->CoefficientsDeLaMatriceDesContraintes,
                          Cdeb,
                          Csui,
                          nullptr);

    buffer.appendFormat("ENDATA\n");

    auto study = Data::Study::Current::Get();
    auto filename = getFilenameWithExtension("problem-fixed-part", "mps", numSpace, optNumber);
    auto writer = study->resultWriter;
    writer->addEntryFromBuffer(filename, buffer);

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);
}

void OPT_dump_spx_variable_part(const PROBLEME_SIMPLEXE* Pb, int optNumber, uint numSpace)
{
    Clob buffer;
    int Var;
    char printBuffer[OPT_APPEL_SOLVEUR_BUFFER_SIZE];

    printHeader(buffer, Pb->NombreDeVariables, Pb->NombreDeContraintes);

    buffer.appendFormat("COLUMNS\n");
    for (Var = 0; Var < Pb->NombreDeVariables; Var++)
    {
        if (Pb->CoutLineaire[Var] != 0.0)
        {
            SNPRINTF(printBuffer, OPT_APPEL_SOLVEUR_BUFFER_SIZE, "%-.10lf", Pb->CoutLineaire[Var]);
            buffer.appendFormat("    C%07d  OBJECTIF  %s\n", Var, printBuffer);
        }
    }

    printRHS(buffer, Pb->NombreDeContraintes, Pb->SecondMembre);

    printBounds(buffer, Pb->NombreDeVariables, Pb->TypeDeVariable, Pb->Xmin, Pb->Xmax);

    buffer.appendFormat("ENDATA\n");

    auto study = Data::Study::Current::Get();
    auto filename = getFilenameWithExtension("problem-variable-part", "mps", numSpace, optNumber);
    auto writer = study->resultWriter;
    writer->addEntryFromBuffer(filename, buffer);
}

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void* Prob, int optNumber, uint numSpace)
{
    Clob buffer;
    int Cnt;
    int Var;
    int il;
    int ilk;
    int ilMax;
    int* Cder;
    int* Cdeb;
    int* NumeroDeContrainte;
    int* Csui;
    double CoutOpt;
    PROBLEME_SIMPLEXE* Probleme;

    int NombreDeVariables;
    double* CoutLineaire;
    int NombreDeContraintes;
    char* Sens;
    int* IndicesDebutDeLigne;
    int* NombreDeTermesDesLignes;
    int* IndicesColonnes;
    int ExistenceDUneSolution;
    double* X;

    Probleme = (PROBLEME_SIMPLEXE*)Prob;

    ExistenceDUneSolution = Probleme->ExistenceDUneSolution;
    if (ExistenceDUneSolution == OUI_SPX)
        ExistenceDUneSolution = OUI_ANTARES;

    NombreDeVariables = Probleme->NombreDeVariables;
    X = Probleme->X;
    CoutLineaire = Probleme->CoutLineaire;
    NombreDeContraintes = Probleme->NombreDeContraintes;
    Sens = Probleme->Sens;
    IndicesDebutDeLigne = Probleme->IndicesDebutDeLigne;
    NombreDeTermesDesLignes = Probleme->NombreDeTermesDesLignes;
    IndicesColonnes = Probleme->IndicesColonnes;

    if (ExistenceDUneSolution == OUI_ANTARES)
    {
        CoutOpt = 0;
        for (Var = 0; Var < NombreDeVariables; Var++)
            CoutOpt += CoutLineaire[Var] * X[Var];
    }

    for (ilMax = -1, Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if ((IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1) > ilMax)
        {
            ilMax = IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1;
        }
    }

    ilMax += NombreDeContraintes;

    Cder = (int*)malloc(NombreDeVariables * sizeof(int));
    Cdeb = (int*)malloc(NombreDeVariables * sizeof(int));
    NumeroDeContrainte = (int*)malloc(ilMax * sizeof(int));
    Csui = (int*)malloc(ilMax * sizeof(int));

    if (Cder == nullptr || Cdeb == nullptr || NumeroDeContrainte == nullptr || Csui == nullptr)
    {
        logs.fatal() << "Not enough memory";
        AntaresSolverEmergencyShutdown();
    }

    for (Var = 0; Var < NombreDeVariables; Var++)
        Cdeb[Var] = -1;

    for (Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        il = IndicesDebutDeLigne[Cnt];
        ilMax = il + NombreDeTermesDesLignes[Cnt];
        while (il < ilMax)
        {
            Var = IndicesColonnes[il];
            if (Cdeb[Var] < 0)
            {
                Cdeb[Var] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }
            else
            {
                ilk = Cder[Var];
                Csui[ilk] = il;
                NumeroDeContrainte[il] = Cnt;
                Csui[il] = -1;
                Cder[Var] = il;
            }

            il++;
        }
    }

    free(Cder);

    printHeader(buffer, NombreDeVariables, NombreDeContraintes);
    buffer.appendFormat("ROWS\n");
    buffer.appendFormat(" N  OBJECTIF\n");

    for (Cnt = 0; Cnt < NombreDeContraintes; Cnt++)
    {
        if (Sens[Cnt] == '=')
        {
            buffer.appendFormat(" E  R%07d\n", Cnt);
        }
        else if (Sens[Cnt] == '<')
        {
            buffer.appendFormat(" L  R%07d\n", Cnt);
        }
        else if (Sens[Cnt] == '>')
        {
            buffer.appendFormat(" G  R%07d\n", Cnt);
        }
        else
        {
            logs.error() << "OPT_EcrireJeuDeDonneesMPS : Wrong direction for constraint no. "
                         << Sens[Cnt];
            AntaresSolverEmergencyShutdown();
        }
    }

    printColumnsObjective(buffer,
                          Probleme->NombreDeVariables,
                          NumeroDeContrainte,
                          Probleme->CoefficientsDeLaMatriceDesContraintes,
                          Cdeb,
                          Csui,
                          Probleme->CoutLineaire);

    printRHS(buffer, Probleme->NombreDeContraintes, Probleme->SecondMembre);

    printBounds(buffer,
                Probleme->NombreDeVariables,
                Probleme->TypeDeVariable,
                Probleme->Xmin,
                Probleme->Xmax);

    buffer.appendFormat("ENDATA\n");

    auto study = Data::Study::Current::Get();
    auto filename = getFilenameWithExtension("problem", "mps", numSpace, optNumber);
    auto writer = study->resultWriter;
    writer->addEntryFromBuffer(filename, buffer);

    free(Cdeb);
    free(NumeroDeContrainte);
    free(Csui);
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
void fullMPSwriter::runIfNeeded()
{
    OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(
      (void*)named_splx_problem_, current_optim_number_, thread_number_);
}

// ---------------------------------
// Full mps writing by or-tools
// ---------------------------------
fullOrToolsMPSwriter::fullOrToolsMPSwriter(MPSolver* solver, int optNumber, uint thread_number) :
 solver_(solver), current_optim_number_(optNumber), thread_number_(thread_number)
{
}
void fullOrToolsMPSwriter::runIfNeeded()
{
    // Make or-tools print the MPS files leads to a crash !
    ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(solver_, thread_number_, current_optim_number_);
}

// ---------------------------------
// mps written under split form
// ---------------------------------
splitMPSwriter::splitMPSwriter(PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                               int optNumber,
                               uint thread_nb,
                               bool simu_1st_week) :
 named_splx_problem_(named_splx_problem),
 current_optim_number_(optNumber),
 thread_nb_(thread_nb),
 simu_1st_week_(simu_1st_week)
{
}

void splitMPSwriter::runIfNeeded()
{
    if (simu_1st_week_)
        OPT_dump_spx_fixed_part(named_splx_problem_, current_optim_number_, thread_nb_);

    OPT_dump_spx_variable_part(named_splx_problem_, current_optim_number_, thread_nb_);
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
    split_mps_ = pb_hebdo_->SplitExportedMPS;
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
    if (doWeExportMPS() && split_mps_)
    {
        return std::make_unique<splitMPSwriter>(
          named_splx_problem_, current_optim_number_, thread_number_, is_first_week_of_year_);
    }
    if (doWeExportMPS() && not split_mps_)
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
