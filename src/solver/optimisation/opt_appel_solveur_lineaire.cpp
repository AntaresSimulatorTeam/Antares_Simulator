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

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

extern "C"
{
# include "../ext/Sirius_Solver/simplexe/spx_definition_arguments.h"
# include "../ext/Sirius_Solver/simplexe/spx_fonctions.h"

# include "../ext/Sirius_Solver/pne/pne_definition_arguments.h"
# include "../ext/Sirius_Solver/pne/pne_fonctions.h"

}

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>


using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

# ifdef _MSC_VER
#	define SNPRINTF sprintf_s
# else
#	define SNPRINTF snprintf
# endif

bool OPT_AppelDuSimplexe( PROBLEME_HEBDO * , uint, int );
bool OPT_AppelDuSolveurPne( PROBLEME_HEBDO * , uint , int );


bool OPT_AppelDuSolveurLineaire( PROBLEME_HEBDO * ProblemeHebdo, uint numSpace, int NumIntervalle )
{

	if ( ProblemeHebdo->SolveurDuProblemeLineaire == ANTARES_SIMPLEXE )
	{
		if ( OPT_AppelDuSimplexe( ProblemeHebdo, numSpace, NumIntervalle ) == true )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if ( OPT_AppelDuSolveurPne( ProblemeHebdo, numSpace, NumIntervalle ) == true )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}



bool OPT_AppelDuSimplexe( PROBLEME_HEBDO * ProblemeHebdo, uint numSpace, int NumIntervalle )
{
	int Var; int Cnt; double * pt; int il; int ilMax; int Classe; char PremierPassage;
	double CoutOpt; PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre; PROBLEME_SIMPLEXE Probleme;
	PROBLEME_SPX * ProbSpx;
	ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
	PremierPassage = OUI_ANTARES;
	Classe = ProblemeAResoudre->NumeroDeClasseDeManoeuvrabiliteActiveEnCours;
	ProbSpx = (PROBLEME_SPX *) ((ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite[Classe])->ProblemeSpx[(int) NumIntervalle]);

	RESOLUTION:

	if ( ProbSpx == NULL )
	{
		Probleme.Contexte            = SIMPLEXE_SEUL;
		Probleme.BaseDeDepartFournie = NON_SPX;
	}
	else
	{
		if (ProblemeHebdo->ReinitOptimisation == OUI_ANTARES )
		{	
			if ( ProbSpx != NULL )
			{
				SPX_LibererProbleme( ProbSpx );
				(ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite[Classe])->ProblemeSpx[(int) NumIntervalle] = NULL;			
			}	
			ProbSpx  = NULL;
			Probleme.Contexte = SIMPLEXE_SEUL;
			Probleme.BaseDeDepartFournie = NON_SPX;	
		}
		else
		{
			Probleme.Contexte            = BRANCH_AND_BOUND_OU_CUT_NOEUD;
			Probleme.BaseDeDepartFournie = UTILISER_LA_BASE_DU_PROBLEME_SPX;
			SPX_ModifierLeVecteurCouts( ProbSpx, ProblemeAResoudre->CoutLineaire, ProblemeAResoudre->NombreDeVariables );
			SPX_ModifierLeVecteurSecondMembre( ProbSpx, ProblemeAResoudre->SecondMembre, ProblemeAResoudre->Sens, ProblemeAResoudre->NombreDeContraintes );
		}
	}


	Probleme.NombreMaxDIterations = -1; 
	Probleme.DureeMaxDuCalcul     = -1.;

	Probleme.CoutLineaire      = ProblemeAResoudre->CoutLineaire;
	Probleme.X                 = ProblemeAResoudre->X;
	Probleme.Xmin              = ProblemeAResoudre->Xmin;
	Probleme.Xmax              = ProblemeAResoudre->Xmax;
	Probleme.NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
	Probleme.TypeDeVariable    = ProblemeAResoudre->TypeDeVariable;

	Probleme.NombreDeContraintes                   = ProblemeAResoudre->NombreDeContraintes;
	Probleme.IndicesDebutDeLigne                   = ProblemeAResoudre->IndicesDebutDeLigne;
	Probleme.NombreDeTermesDesLignes               = ProblemeAResoudre->NombreDeTermesDesLignes;
	Probleme.IndicesColonnes                       = ProblemeAResoudre->IndicesColonnes;
	Probleme.CoefficientsDeLaMatriceDesContraintes = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes;
	Probleme.Sens                                  = ProblemeAResoudre->Sens;
	Probleme.SecondMembre                          = ProblemeAResoudre->SecondMembre;

	Probleme.ChoixDeLAlgorithme = SPX_DUAL;
	Probleme.TypeDePricing               = PRICING_STEEPEST_EDGE ;

	if (PremierPassage == NON_ANTARES)
		Probleme.FaireDuScaling = NON_SPX;
	if (PremierPassage == OUI_ANTARES)
		Probleme.FaireDuScaling = OUI_SPX;

	Probleme.StrategieAntiDegenerescence = AGRESSIF;

	Probleme.PositionDeLaVariable       = ProblemeAResoudre->PositionDeLaVariable;
	Probleme.NbVarDeBaseComplementaires = 0; 
	Probleme.ComplementDeLaBase         = ProblemeAResoudre->ComplementDeLaBase;

	Probleme.LibererMemoireALaFin = NON_SPX;

	Probleme.UtiliserCoutMax = NON_SPX;
	Probleme.CoutMax = 0.0;

	Probleme.CoutsMarginauxDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes;
	Probleme.CoutsReduits                 = ProblemeAResoudre->CoutsReduits;

	# ifndef NDEBUG
	if ( PremierPassage == OUI_ANTARES )
		Probleme.AffichageDesTraces = NON_SPX;
	else
		Probleme.AffichageDesTraces = OUI_SPX;
	# else
	Probleme.AffichageDesTraces = NON_SPX;
	# endif

	Probleme.NombreDeContraintesCoupes = 0;


	ProbSpx = SPX_Simplexe( &Probleme , ProbSpx );


	if ( ProbSpx != NULL )
	{  
		(ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite[Classe])->ProblemeSpx[NumIntervalle] = (void *) ProbSpx;
	}

	if ( ProblemeHebdo->ExportMPS == OUI_ANTARES)
		OPT_EcrireJeuDeDonneesLineaireAuFormatMPS( (void *) &Probleme, numSpace, ANTARES_SIMPLEXE );

	ProblemeAResoudre->ExistenceDUneSolution = Probleme.ExistenceDUneSolution;



	if ( ProblemeAResoudre->ExistenceDUneSolution != OUI_SPX && PremierPassage == OUI_ANTARES && ProbSpx != NULL )
	{
		if ( ProblemeAResoudre->ExistenceDUneSolution != SPX_ERREUR_INTERNE )
		{
			SPX_LibererProbleme( ProbSpx );
			logs.info() << " Solver: Standard resolution failed"; 
			logs.info() << " Solver: Retry in safe mode";			//second trial w/o scaling

			if (Logs::Verbosity::Debug::enabled)
			{
				logs.info() << " solver: resetting";
			}
			ProbSpx = NULL;
			PremierPassage = NON_ANTARES;
			goto RESOLUTION;
		}

		else
		{
			logs.info(); 
			logs.error() << "Internal error: insufficient memory";
			logs.info(); 
			AntaresSolverEmergencyShutdown();
			return false;
		}
	}

	if ( ProblemeAResoudre->ExistenceDUneSolution == OUI_SPX )
	{
		if (PremierPassage == NON_ANTARES)
		{
			logs.info() << " Solver: Safe resolution succeeded";
		}
		CoutOpt = 0.0;
	
		for ( Var = 0 ; Var < ProblemeAResoudre->NombreDeVariables ; Var++ )
		{
			CoutOpt+= ProblemeAResoudre->CoutLineaire[Var] * ProblemeAResoudre->X[Var];		
		
			pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
			if ( pt != NULL )
				*pt = ProblemeAResoudre->X[Var];

			pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[Var];
			if ( pt != NULL )
				*pt = ProblemeAResoudre->CoutsReduits[Var];
		}

		if (ProblemeHebdo->numeroOptimisation[NumIntervalle] == PREMIERE_OPTIMISATION)
			ProblemeHebdo->coutOptimalSolution1[NumIntervalle] = CoutOpt;
		else
			ProblemeHebdo->coutOptimalSolution2[NumIntervalle] = CoutOpt;

	
		for ( Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++ )
		{
			pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt];
			if ( pt != NULL )
				*pt = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
		}
	}

	else
	{
		if (PremierPassage == NON_ANTARES)
		{
			logs.info() << " Solver: Safe resolution failed";
		}

		logs.error() << "Infeasible linear problem encountered. Possible causes:";
		logs.error() << "* binding constraints,";
		logs.error() << "* last resort shedding status,";
		logs.error() << "* negative hurdle costs on lines with infinite capacity,";
		logs.error() << "* Hydro reservoir impossible to manage with cumulative options \"hard bounds without heuristic\"";
	
	
		if (0)
		{
			logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
			logs.info() << "Here is the trace:";

			for (Cnt = 0 ; Cnt < ProblemeAResoudre->NombreDeContraintes; ++Cnt)
			{
				logs.info().appendFormat(
						"Constraint %ld sens %c B %e",
						Cnt,ProblemeAResoudre->Sens[Cnt],ProblemeAResoudre->SecondMembre[Cnt]);

				il = ProblemeAResoudre->IndicesDebutDeLigne[Cnt];
				ilMax = il + ProblemeAResoudre->NombreDeTermesDesLignes[Cnt];
				while (il < ilMax)
				{
					Var = ProblemeAResoudre->IndicesColonnes[il];
					logs.info().appendFormat("      il %ld coeff %e var %ld xmin %e xmax %e type %ld cout %e",
							il,  ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[il],
							Var, ProblemeAResoudre->Xmin[Var],
							ProblemeAResoudre->Xmax[Var],
							ProblemeAResoudre->TypeDeVariable[Var], ProblemeAResoudre->CoutLineaire[Var]);
					il++;
				}
			}

			logs.info() << LOG_UI_DISPLAY_MESSAGES_ON;
		}

	
		if ( ProblemeHebdo->ExportMPS == NON_ANTARES)
		{
			OPT_EcrireJeuDeDonneesLineaireAuFormatMPS( (void *) &Probleme, numSpace, ANTARES_SIMPLEXE );
		}

		return false;
	}

	return true;
}




bool OPT_AppelDuSolveurPne( PROBLEME_HEBDO * ProblemeHebdo, uint numSpace, int NumIntervalle )
{
	int Var; int Cnt; double * pt; int il; int ilMax; double CoutOpt; PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;
	int * TypeEntierOuReel; PROBLEME_A_RESOUDRE ProblemePourPne; double u;
	ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
	TypeEntierOuReel = (int *) ProblemeAResoudre->CoutsReduits;

	for ( Var = 0 ; Var < ProblemeAResoudre->NombreDeVariables ; Var++ )
		TypeEntierOuReel[Var] = REEL;



	ProblemePourPne.NombreDeVariables       = ProblemeAResoudre->NombreDeVariables;
	ProblemePourPne.TypeDeVariable          = TypeEntierOuReel;
	ProblemePourPne.TypeDeBorneDeLaVariable = ProblemeAResoudre->TypeDeVariable; 
	ProblemePourPne.X                       = ProblemeAResoudre->X;
	ProblemePourPne.Xmax                    = ProblemeAResoudre->Xmax;
	ProblemePourPne.Xmin                    = ProblemeAResoudre->Xmin;
	ProblemePourPne.CoutLineaire            = ProblemeAResoudre->CoutLineaire;
	ProblemePourPne.NombreDeContraintes                   = ProblemeAResoudre->NombreDeContraintes;
	ProblemePourPne.SecondMembre                          = ProblemeAResoudre->SecondMembre;
	ProblemePourPne.Sens                                  = ProblemeAResoudre->Sens;
	ProblemePourPne.IndicesDebutDeLigne                   = ProblemeAResoudre->IndicesDebutDeLigne;
	ProblemePourPne.NombreDeTermesDesLignes               = ProblemeAResoudre->NombreDeTermesDesLignes;
	ProblemePourPne.CoefficientsDeLaMatriceDesContraintes = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes;
	ProblemePourPne.IndicesColonnes                       = ProblemeAResoudre->IndicesColonnes;
	ProblemePourPne.VariablesDualesDesContraintes         = ProblemeAResoudre->CoutsMarginauxDesContraintes;
	ProblemePourPne.SortirLesDonneesDuProbleme = NON_PNE;
	ProblemePourPne.AlgorithmeDeResolution     = SIMPLEXE;   
	ProblemePourPne.CoupesLiftAndProject       = NON_PNE; 
	ProblemePourPne.AffichageDesTraces = NON_PNE;
	ProblemePourPne.FaireDuPresolve = OUI_PNE ; 
	if ( ProblemeAResoudre->NumeroDOptimisation == DEUXIEME_OPTIMISATION )
	{
	  ProblemePourPne.FaireDuPresolve = NON_PNE;
	}

	ProblemePourPne.TempsDExecutionMaximum       = 0;  
	ProblemePourPne.NombreMaxDeSolutionsEntieres = -1;   
	ProblemePourPne.ToleranceDOptimalite         = 1.e-4; 


	PNE_Solveur( &ProblemePourPne );


	if ( ProblemeHebdo->ExportMPS == OUI_ANTARES)
		OPT_EcrireJeuDeDonneesLineaireAuFormatMPS( (void *) &ProblemePourPne, numSpace, ANTARES_PNE );

	ProblemeAResoudre->ExistenceDUneSolution = ProblemePourPne.ExistenceDUneSolution;

	if ( ProblemeAResoudre->ExistenceDUneSolution == ARRET_CAR_ERREUR_INTERNE )
	{
		logs.info(); 
		logs.error() << "Internal error: insufficient memory";
		logs.info(); 
		AntaresSolverEmergencyShutdown();
		return false;
	}


	bool sol_opt_trouvee = (ProblemeAResoudre->ExistenceDUneSolution == SOLUTION_OPTIMALE_TROUVEE);
	sol_opt_trouvee = (sol_opt_trouvee || ProblemeAResoudre->ExistenceDUneSolution == SOLUTION_OPTIMALE_TROUVEE_MAIS_QUELQUES_CONTRAINTES_SONT_VIOLEES);
	if (sol_opt_trouvee)
	{
		CoutOpt = 0.0;

		for ( Var = 0 ; Var < ProblemeAResoudre->NombreDeVariables ; Var++ )
			ProblemeAResoudre->CoutsReduits[Var] = ProblemeAResoudre->CoutLineaire[Var];

		for ( Cnt = 0 ; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++ )
		{
			il = ProblemeAResoudre->IndicesDebutDeLigne[Cnt];
			ilMax = il + ProblemeAResoudre->NombreDeTermesDesLignes[Cnt];
			u = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
			while ( il < ilMax )
			{
				Var = ProblemeAResoudre->IndicesColonnes[il];
				ProblemeAResoudre->CoutsReduits[Var] -= u * ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[il];
				il++;
			}
		}
	
		for ( Var = 0 ; Var < ProblemeAResoudre->NombreDeVariables ; Var++ )
		{
			CoutOpt+= ProblemeAResoudre->CoutLineaire[Var] * ProblemeAResoudre->X[Var];		
		
			pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
			if ( pt != NULL )
				*pt = ProblemeAResoudre->X[Var];

			pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[Var];
			if ( pt != NULL )
				*pt = ProblemeAResoudre->CoutsReduits[Var];
		}


		if (ProblemeHebdo->numeroOptimisation[NumIntervalle] == PREMIERE_OPTIMISATION)
			ProblemeHebdo->coutOptimalSolution1[NumIntervalle] = CoutOpt;
		else
			ProblemeHebdo->coutOptimalSolution2[NumIntervalle] = CoutOpt;

	
		for ( Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++ )
		{
			pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt];
			if ( pt != NULL )
				*pt = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
		}
	
	}
	else
	{
		logs.error() << "Infeasible linear problem encountered. Possible causes:";
		logs.error() << "* binding constraints,";
		logs.error() << "* last resort shedding status,";
		logs.error() << "* negative hurdle costs on lines with infinite capacity,";
		logs.error() << "* Hydro reservoir impossible to manage with cumulative options \"hard bounds without heuristic\"";

		if (0)
		{
			logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
			logs.info() << "Here is the trace:";
			for (Cnt = 0 ; Cnt < ProblemeAResoudre->NombreDeContraintes; ++Cnt)
			{
				logs.info().appendFormat("Constraint %ld sens %c B %e",
					Cnt, ProblemeAResoudre->Sens[Cnt], ProblemeAResoudre->SecondMembre[Cnt]);

				il = ProblemeAResoudre->IndicesDebutDeLigne[Cnt];
				ilMax = il + ProblemeAResoudre->NombreDeTermesDesLignes[Cnt];
				while (il < ilMax)
				{
					Var = ProblemeAResoudre->IndicesColonnes[il];
					logs.info().appendFormat("      il %ld coeff %e var %ld xmin %e xmax %e type %ld cout %e",
							il,  ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[il],
							Var, ProblemeAResoudre->Xmin[Var],
							ProblemeAResoudre->Xmax[Var],
							ProblemeAResoudre->TypeDeVariable[Var], ProblemeAResoudre->CoutLineaire[Var]);
					il++;
				}
			}
		
			logs.info() << LOG_UI_DISPLAY_MESSAGES_ON;
		}

		if ( ProblemeHebdo->ExportMPS == NON_ANTARES)
		{	
			OPT_EcrireJeuDeDonneesLineaireAuFormatMPS( (void *) &ProblemePourPne, numSpace, ANTARES_PNE );	
		}
	
		return false;
	}

	return true;
}


void OPT_EcrireResultatFonctionObjectiveAuFormatTXT(void * Prob, uint numSpace, int NumeroDeLIntervalle)
{
	FILE * Flot; double CoutOptimalDeLaSolution; PROBLEME_HEBDO * Probleme;

	Probleme = (PROBLEME_HEBDO *) Prob;		
	
	CoutOptimalDeLaSolution = 0.;
	if (Probleme->numeroOptimisation[NumeroDeLIntervalle] == PREMIERE_OPTIMISATION)
		CoutOptimalDeLaSolution = Probleme->coutOptimalSolution1[NumeroDeLIntervalle];
	else
		CoutOptimalDeLaSolution = Probleme->coutOptimalSolution2[NumeroDeLIntervalle];
	
	
	auto& study = *Data::Study::Current::Get();
	Flot = study.createCriterionFileIntoOutput(numSpace);
	if (!Flot)
		exit(2);

	fprintf(Flot,"* Optimal criterion value :   %11.10e\n", CoutOptimalDeLaSolution);	

	fclose( Flot );

	return;
}



void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS( void * Prob, uint numSpace, char Type )
{
	FILE * Flot; int Cnt; int Var; int il; int ilk; int ilMax; char * Nombre;
	int * Cder; int * Cdeb; int * NumeroDeContrainte; int * Csui; double CoutOpt;
	PROBLEME_SIMPLEXE * Probleme; PROBLEME_A_RESOUDRE * ProblemePourPne;


	int NombreDeVariables; int * TypeDeBorneDeLaVariable;
	double * Xmax; double * Xmin; double * CoutLineaire; int NombreDeContraintes;
	double * SecondMembre; char * Sens; int * IndicesDebutDeLigne;
	int * NombreDeTermesDesLignes;	double * CoefficientsDeLaMatriceDesContraintes;
	int * IndicesColonnes; int ExistenceDUneSolution; double * X;


	if ( Type == ANTARES_SIMPLEXE )
	{
		Probleme = (PROBLEME_SIMPLEXE *) Prob;

		ExistenceDUneSolution	  = Probleme->ExistenceDUneSolution;            
		if ( ExistenceDUneSolution == OUI_SPX )
			ExistenceDUneSolution = OUI_ANTARES;	

		NombreDeVariables       = Probleme->NombreDeVariables;
		TypeDeBorneDeLaVariable = Probleme->TypeDeVariable;
		Xmax                    = Probleme->Xmax;
		Xmin                    = Probleme->Xmin;
		X                       = Probleme->X;
		CoutLineaire            = Probleme->CoutLineaire;
		NombreDeContraintes                   = Probleme->NombreDeContraintes;
		SecondMembre                          = Probleme->SecondMembre;
		Sens                                  = Probleme->Sens;
		IndicesDebutDeLigne                   = Probleme->IndicesDebutDeLigne;
		NombreDeTermesDesLignes               = Probleme->NombreDeTermesDesLignes;
		CoefficientsDeLaMatriceDesContraintes = Probleme->CoefficientsDeLaMatriceDesContraintes;
		IndicesColonnes                       = Probleme->IndicesColonnes;
	}

	else
	{
		ProblemePourPne = (PROBLEME_A_RESOUDRE *) Prob;

		ExistenceDUneSolution	  = ProblemePourPne->ExistenceDUneSolution;            
		if ( ExistenceDUneSolution == SOLUTION_OPTIMALE_TROUVEE )
			ExistenceDUneSolution = OUI_ANTARES;

		NombreDeVariables       = ProblemePourPne->NombreDeVariables;
		TypeDeBorneDeLaVariable = ProblemePourPne->TypeDeBorneDeLaVariable; 
		Xmax                    = ProblemePourPne->Xmax;
		Xmin                    = ProblemePourPne->Xmin;
		X                       = ProblemePourPne->X;
		CoutLineaire            = ProblemePourPne->CoutLineaire;
		NombreDeContraintes                   = ProblemePourPne->NombreDeContraintes;
		SecondMembre                          = ProblemePourPne->SecondMembre;
		Sens                                  = ProblemePourPne->Sens;
		IndicesDebutDeLigne                   = ProblemePourPne->IndicesDebutDeLigne;
		NombreDeTermesDesLignes               = ProblemePourPne->NombreDeTermesDesLignes;
		CoefficientsDeLaMatriceDesContraintes = ProblemePourPne->CoefficientsDeLaMatriceDesContraintes;
		IndicesColonnes                       = ProblemePourPne->IndicesColonnes;	
	}


	if ( ExistenceDUneSolution == OUI_ANTARES )
	{
		CoutOpt = 0;
		for ( Var = 0 ; Var < NombreDeVariables ; Var++ )
			CoutOpt += CoutLineaire[Var] * X[Var];
	}


	for ( ilMax = -1 , Cnt = 0 ; Cnt < NombreDeContraintes; Cnt++ )
	{
		if ( ( IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1 ) > ilMax )
		{
			ilMax = IndicesDebutDeLigne[Cnt] + NombreDeTermesDesLignes[Cnt] - 1;
		}
	}

	ilMax+= NombreDeContraintes; 

	Cder               = (int *) malloc( NombreDeVariables * sizeof( int ) );
	Cdeb               = (int *) malloc( NombreDeVariables * sizeof( int ) );
	NumeroDeContrainte = (int *) malloc( ilMax             * sizeof( int ) );
	Csui               = (int *) malloc( ilMax             * sizeof( int ) );
	Nombre             = (char *) malloc( 1024 );

	if ( Cder == NULL || Cdeb == NULL || NumeroDeContrainte == NULL || Csui == NULL || Nombre == NULL )
	{
		logs.fatal() << "Not enough memory";
		AntaresSolverEmergencyShutdown();
	}


	for ( Var = 0 ; Var < NombreDeVariables ; Var++ )
		Cdeb[Var] = -1;

	for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ )
	{
		il    = IndicesDebutDeLigne[Cnt];
		ilMax = il + NombreDeTermesDesLignes[Cnt];
		while ( il < ilMax )
		{
			Var = IndicesColonnes[il];
			if ( Cdeb[Var] < 0 )
			{
				Cdeb              [Var] = il;
				NumeroDeContrainte[il]  = Cnt;
				Csui              [il]  = -1;
				Cder              [Var] = il;
			}
			else
			{
				ilk                     = Cder[Var];
				Csui              [ilk] = il;
				NumeroDeContrainte[il]  = Cnt;
				Csui              [il]  = -1;
				Cder              [Var] = il;
			}

			il++;
		}
	}

	free( Cder );

	auto& study = *Data::Study::Current::Get();
	Flot = study.createMPSFileIntoOutput(numSpace);

	if (!Flot)
		exit(2);

	fprintf(Flot, "* Number of variables:   %d\n", NombreDeVariables);
	fprintf(Flot, "* Number of constraints: %d\n", NombreDeContraintes);
	fprintf(Flot, "NAME          Pb Solve\n");
	fprintf(Flot, "ROWS\n");
	fprintf(Flot, " N  OBJECTIF\n");

	for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ )
	{
		if ( Sens[Cnt] == '=' )
		{
			fprintf(Flot, " E  R%07d\n", Cnt);
		}
		else if (  Sens[Cnt] == '<' )
		{
			fprintf(Flot, " L  R%07d\n",Cnt);
		}
		else if (  Sens[Cnt] == '>' )
		{
			fprintf(Flot, " G  R%07d\n", Cnt);
		}
		else
		{
			fprintf(Flot, "PNE_EcrireJeuDeDonneesMPS : le sens de la contrainte %c ne fait pas partie des sens reconnus\n", Sens[Cnt]);
			AntaresSolverEmergencyShutdown(); 
			exit(0);
		}
	}


	fprintf(Flot, "COLUMNS\n");
	for ( Var = 0 ; Var < NombreDeVariables ; Var++ )
	{
		if ( CoutLineaire[Var] != 0.0 )
		{
			SNPRINTF(Nombre, 1024, "%-.10lf", CoutLineaire[Var]);
			fprintf(Flot, "    C%07d  OBJECTIF  %s\n", Var, Nombre);
		}

		il = Cdeb[Var];
		while ( il >= 0 )
		{
			SNPRINTF(Nombre, 1024, "%-.10lf", CoefficientsDeLaMatriceDesContraintes[il]);
			fprintf(Flot, "    C%07d  R%07d  %s\n", Var, NumeroDeContrainte[il],Nombre);
			il = Csui[il];
		}
	}

	fprintf(Flot, "RHS\n");
	for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ )
	{
		if ( SecondMembre[Cnt] != 0.0 )
		{
			SNPRINTF(Nombre, 1024, "%-.9lf",SecondMembre[Cnt]);
			fprintf(Flot, "    RHSVAL    R%07d  %s\n", Cnt, Nombre);
		}
	}

	fprintf(Flot, "BOUNDS\n");

	for ( Var = 0 ; Var < NombreDeVariables ; Var++ )
	{
		if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_FIXE )
		{
			SNPRINTF(Nombre, 1024, "%-.9lf", Xmin[Var]);
     
			fprintf(Flot, " FX BNDVALUE  C%07d  %s\n", Var, Nombre);
			continue;
		}
  
		if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES )
		{
			if ( Xmin[Var] != 0.0 )
			{
				SNPRINTF(Nombre, 1024, "%-.9lf", Xmin[Var]);
				fprintf(Flot," LO BNDVALUE  C%07d  %s\n",Var,Nombre);
			}

			SNPRINTF(Nombre, 1024, "%-.9lf", Xmax[Var]);
			fprintf(Flot, " UP BNDVALUE  C%07d  %s\n", Var, Nombre);
		}
		
		if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_INFERIEUREMENT )
		{
			if ( Xmin[Var] != 0.0 )
			{
				SNPRINTF(Nombre, 1024, "%-.9lf", Xmin[Var]);
				fprintf(Flot, " LO BNDVALUE  C%07d  %s\n", Var, Nombre);
			}
		}

		if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_BORNEE_SUPERIEUREMENT )
		{
			fprintf(Flot, " MI BNDVALUE  C%07d\n", Var);
			if ( Xmax[Var] != 0.0 )
			{
				SNPRINTF(Nombre, 1024, "%-.9lf", Xmax[Var]);
				fprintf(Flot, " UP BNDVALUE  C%07d  %s\n", Var, Nombre);
			}
		}

		if ( TypeDeBorneDeLaVariable[Var] == VARIABLE_NON_BORNEE )
		{
			fprintf(Flot, " FR BNDVALUE  C%07d\n", Var);
		}
	}

	fprintf(Flot, "ENDATA\n");

	free ( Cdeb );
	free ( NumeroDeContrainte );
	free ( Csui );
	free ( Nombre );

	fclose( Flot );

	return;
}
