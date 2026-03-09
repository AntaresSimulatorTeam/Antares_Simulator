// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <limits>

/*
 pi_define.h doesn't include this header, yet it uses struct jmp_buf.
 It would be nice to remove this include, but would require to change pi_define.h,
 which isn't part of Antares
*/

#include <setjmp.h>
extern "C"
{
#include <pi_fonctions.h>
}

#include <antares/logs/logs.h>
#include "antares/optimization-options/options.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/utils/ortools_quadratic_wrapper.h"
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares;

// TODO : there are 2 SolveWithSirius(...) solving a quadratic problem by interior point.
// TODO : we should try to avoid code duplications.
static void SolveWithSirius(const Solver::Optimization::SingleOptimOptions& options,
                            PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    if (!options.solverParameters.empty())
    {
        logs.warning()
          << "Quadratic solver parameters are not supported by SIRIUS; they will be ignored.";
    }

    PROBLEME_POINT_INTERIEUR Probleme;

    double ToleranceSurLAdmissibilite = 1.e-5;
    int ChoixToleranceParDefautSurLAdmissibilite = OUI_PI;

    double ToleranceSurLaStationnarite = 1.e-5;
    int ChoixToleranceParDefautSurLaStationnarite = OUI_PI;

    double ToleranceSurLaComplementarite = 1.e-5;
    int ChoixToleranceParDefautSurLaComplementarite = OUI_PI;

    Probleme.NombreMaxDIterations = -1;
    Probleme.CoutQuadratique = ProblemeAResoudre->CoutQuadratique.data();
    Probleme.CoutLineaire = ProblemeAResoudre->CoutLineaire.data();
    Probleme.X = ProblemeAResoudre->X.data();
    Probleme.Xmin = ProblemeAResoudre->Xmin.data();
    Probleme.Xmax = ProblemeAResoudre->Xmax.data();
    Probleme.NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    Probleme.TypeDeVariable = ProblemeAResoudre->TypeDeVariable.data();

    // The problem has no binary variable
    // We use the fact that CoutsReduits is a vector of 1 element per variable, initialized to 0
    // TODO: make this cleaner
    Probleme.VariableBinaire = (char*)ProblemeAResoudre->CoutsReduits.data();

    Probleme.NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
    Probleme.IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne.data();
    Probleme.NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes.data();
    Probleme.IndicesColonnes = ProblemeAResoudre->IndicesColonnes.data();
    Probleme.CoefficientsDeLaMatriceDesContraintes = ProblemeAResoudre
                                                       ->CoefficientsDeLaMatriceDesContraintes
                                                       .data();
    Probleme.Sens = ProblemeAResoudre->Sens.data();
    Probleme.SecondMembre = ProblemeAResoudre->SecondMembre.data();

    Probleme.AffichageDesTraces = NON_PI;

    Probleme.UtiliserLaToleranceDAdmissibiliteParDefaut = ChoixToleranceParDefautSurLAdmissibilite;
    Probleme.ToleranceDAdmissibilite = ToleranceSurLAdmissibilite;

    Probleme.UtiliserLaToleranceDeStationnariteParDefaut
      = ChoixToleranceParDefautSurLaStationnarite;
    Probleme.ToleranceDeStationnarite = ToleranceSurLaStationnarite;

    Probleme.UtiliserLaToleranceDeComplementariteParDefaut
      = ChoixToleranceParDefautSurLaComplementarite;
    Probleme.ToleranceDeComplementarite = ToleranceSurLaComplementarite;

    Probleme.CoutsMarginauxDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes.data();

    Probleme.CoutsMarginauxDesContraintesDeBorneInf = ProblemeAResoudre->CoutsReduits.data();
    Probleme.CoutsMarginauxDesContraintesDeBorneSup = ProblemeAResoudre->CoutsReduits.data();

    PI_Quamin(&Probleme);

    ProblemeAResoudre->ExistenceDUneSolution = Probleme.ExistenceDUneSolution;
    if (ProblemeAResoudre->ExistenceDUneSolution == OUI_PI)
    {
        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
        {
            double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt)
            {
                *pt = ProblemeAResoudre->X[i];
            }
        }
    }
    else
    {
        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
        {
            double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt)
            {
                *pt = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
}

static void ProcessResult(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    if (ProblemeAResoudre->ExistenceDUneSolution == NON_PI)
    {
#ifndef NDEBUG

        {
            logs.info();

            logs.info() << "Here is the trace:";
            for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
            {
                logs.info().appendFormat("Variable %ld cout lineaire %e  cout quadratique %e",
                                         i,
                                         ProblemeAResoudre->CoutLineaire[i],
                                         ProblemeAResoudre->CoutQuadratique[i]);
            }
            for (int Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
            {
                logs.info().appendFormat("Constraint %ld sens %c B %e",
                                         Cnt,
                                         ProblemeAResoudre->Sens[Cnt],
                                         ProblemeAResoudre->SecondMembre[Cnt]);

                int il = ProblemeAResoudre->IndicesDebutDeLigne[Cnt];
                int ilMax = il + ProblemeAResoudre->NombreDeTermesDesLignes[Cnt];
                for (; il < ilMax; ++il)
                {
                    int var = ProblemeAResoudre->IndicesColonnes[il];
                    logs.info().appendFormat(
                      "      coeff %e var %ld xmin %e xmax %e type %ld",
                      ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[il],
                      var,
                      ProblemeAResoudre->Xmin[var],
                      ProblemeAResoudre->Xmax[var],
                      ProblemeAResoudre->TypeDeVariable[var]);
                }
            }
        }
#endif
    }
}

bool OPT_AppelDuSolveurQuadratique(const Solver::Optimization::SingleOptimOptions& options,
                                   PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    // as long as sirius quadratic optimization is not supported through or-tools, we have to keep
    // this code separate
    if (options.solverName.compare("sirius") == 0)
    {
        SolveWithSirius(options, ProblemeAResoudre);
    }
    else
    {
        SolveQuadraticProblemWithOrtools(options, ProblemeAResoudre);
    }
    ProcessResult(ProblemeAResoudre);
    return ProblemeAResoudre->ExistenceDUneSolution == OUI_PI;
}
