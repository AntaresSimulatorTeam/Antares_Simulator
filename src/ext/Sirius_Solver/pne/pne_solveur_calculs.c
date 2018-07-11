/*
** Copyright 2007-2018 RTE
** Author: Robert Gonzalez
**
** This file is part of Sirius_Solver.
** This program and the accompanying materials are made available under the
** terms of the Eclipse Public License 2.0 which is available at
** http://www.eclipse.org/legal/epl-2.0.
**
** This Source Code may also be made available under the following Secondary
** Licenses when the conditions for such availability set forth in the Eclipse
** Public License, v. 2.0 are satisfied: GNU General Public License, version 3
** or later, which is available at <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: EPL-2.0 OR GPL-3.0
*/
/***********************************************************************

   FONCTION: Solveur de PLNE 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# include "prs_define.h"

/*----------------------------------------------------------------------------*/

void PNE_SolveurCalculs( PROBLEME_A_RESOUDRE * Probleme , PROBLEME_PNE * Pne )
{
int i; int NbVarLibres; int NbVarEntieresLibres; int ic; int Nz;

/*                                      */ 
int NombreDeVariables; int * TypeDeVariable; int * TypeDeBorneDeLaVariable; double * X;   
double * Xmax; double * Xmin; double * CoutLineaire; int NombreDeContraintes;  
double * SecondMembre; char * Sens; int * IndicesDebutDeLigne; int * NombreDeTermesDesLignes;	        
double * CoefficientsDeLaMatriceDesContraintes; int * IndicesColonnes; int * YaUneSolution; 
double * VariablesDualesDesContraintes;
/*                                                        */

NombreDeVariables       = Probleme->NombreDeVariables;    
TypeDeVariable          = Probleme->TypeDeVariable;   
TypeDeBorneDeLaVariable = Probleme->TypeDeBorneDeLaVariable;
X 	                = Probleme->X; 
Xmax 	                = Probleme->Xmax;  
Xmin 	                = Probleme->Xmin;  
CoutLineaire 	        = Probleme->CoutLineaire;              
NombreDeContraintes                   = Probleme->NombreDeContraintes;
SecondMembre  	                      = Probleme->SecondMembre;           
Sens	                              = Probleme->Sens;
IndicesDebutDeLigne                   = Probleme->IndicesDebutDeLigne;	                 
NombreDeTermesDesLignes               = Probleme->NombreDeTermesDesLignes;	        
CoefficientsDeLaMatriceDesContraintes = Probleme->CoefficientsDeLaMatriceDesContraintes;
IndicesColonnes                       = Probleme->IndicesColonnes;
VariablesDualesDesContraintes = Probleme->VariablesDualesDesContraintes;
YaUneSolution                 = &Probleme->ExistenceDUneSolution;

Pne->YaUneSolution        = OUI_PNE;
Pne->YaUneSolutionEntiere = NON_PNE;

Pne->SolveurPourLeProblemeRelaxe = Probleme->AlgorithmeDeResolution;
if ( Pne->SolveurPourLeProblemeRelaxe != SIMPLEXE && Pne->SolveurPourLeProblemeRelaxe != POINT_INTERIEUR ) {
  printf("Choix de l'algorithme de resolution incorrect. Choisir SIMPLEXE ou POINT_INTERIEUR \n");
  exit(0);
}
if ( Pne->SolveurPourLeProblemeRelaxe == POINT_INTERIEUR ) {
  printf("Attention, algorithme choisi: point interieur => toutes les variables sont considerees comme etant continue\n");
}

Pne->AffichageDesTraces = Probleme->AffichageDesTraces;
if ( Pne->AffichageDesTraces == OUI_PNE && Pne->Controls == NULL ) {
  printf("\n");
  printf(" ----------------------------------------------------------\n");
  printf("|                                                          |\n");
  printf("|               Starting PNE_Solveur                       |\n");
  printf("|                                                          |\n"); 
  printf(" ---------------------------------------------------------- \n");
  printf("\n");
}
Pne->FaireDuPresolve              = Probleme->FaireDuPresolve;
Pne->TempsDExecutionMaximum       = Probleme->TempsDExecutionMaximum;
Pne->NombreMaxDeSolutionsEntieres = Probleme->NombreMaxDeSolutionsEntieres;
Pne->ToleranceDOptimalite         = Probleme->ToleranceDOptimalite;

/* Allocations du probleme */
PNE_AllocProbleme( Pne,
                   NombreDeVariables      , TypeDeVariable , TypeDeBorneDeLaVariable,
                   Xmax                   , Xmin           , NombreDeContraintes    , IndicesDebutDeLigne,
                   NombreDeTermesDesLignes, IndicesColonnes, CoefficientsDeLaMatriceDesContraintes );
									 
/* Transfert du probleme dans le struct du solveur et initialisation de parametres de controle */
PNE_InitialiserLaPne( Pne, Probleme );
if ( Pne->YaUneSolution != OUI_PNE ) {
  /*printf("Absence de solution detectee par le PRESOLVE\n");*/
  goto FinDuBranchAndBound; 
}

if ( Pne->Controls != NULL ) {
  if ( Pne->Controls->PresolveUniquement == OUI_PNE ) {
    if ( Pne->Controls->Presolve != NULL ) {
      PNE_PostSolveSiUniquementPresolve( Pne, Probleme ); 
      PRS_LiberationStructure( (PRESOLVE *)	Pne->Controls->Presolve );
	  }
	  goto LibererLeProbleme;  
	}
}

/* Pour la connexion au branch and bound pur */
for ( Pne->NombreDeVariablesEntieresTrav = 0 , i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  if ( Pne->TypeDeVariableTrav[i] == ENTIER ) {
    /* printf("PNE_Solveur variable entiere numero %d \n",i); */
    Pne->NumerosDesVariablesEntieresTrav[Pne->NombreDeVariablesEntieresTrav] = i;
    Pne->NombreDeVariablesEntieresTrav++;
  }
}

NbVarEntieresLibres = 0;
NbVarLibres = 0;
for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  if ( Pne->TypeDeBorneTrav[i] != VARIABLE_FIXE ) {
	  Pne->NumeroDesVariablesNonFixes[NbVarLibres] = i;
	  NbVarLibres++;
		if ( Pne->TypeDeVariableTrav[i] == ENTIER ) NbVarEntieresLibres++;
	}
}

Pne->NombreDeVariablesNonFixes = NbVarLibres;
Pne->NombreDeVariablesEntieresNonFixes = NbVarEntieresLibres;

if ( Pne->NombreDeVariablesEntieresTrav == NbVarLibres) Pne->YaQueDesVariablesEntieres = OUI_PNE;
else Pne->YaQueDesVariablesEntieres = NON_PNE;

/* S'il n'y a pas de contrainte on fixe les variables */
if ( Pne->NombreDeContraintesTrav <= 0 ) {
  for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
    if ( Pne->TypeDeBorneTrav[i] == VARIABLE_FIXE ) continue;
    if ( Pne->LTrav[i] > 0. ) {
      if ( Pne->TypeDeBorneTrav[i] == VARIABLE_BORNEE_INFERIEUREMENT ||
           Pne->TypeDeBorneTrav[i] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
        Pne->UTrav[i] = Pne->UminTrav[i];
      }
      else {
        Pne->YaUneSolution = NON_PNE;
	      break;
      }
    }
    else {
      if ( Pne->TypeDeBorneTrav[i] == VARIABLE_BORNEE_SUPERIEUREMENT ||
           Pne->TypeDeBorneTrav[i] == VARIABLE_BORNEE_DES_DEUX_COTES ) {   
        Pne->UTrav[i] = Pne->UmaxTrav[i];
      }
      else if ( Pne->LTrav[i] == 0.0 ) {
        if ( Pne->TypeDeBorneTrav[i] == VARIABLE_BORNEE_INFERIEUREMENT ) {
          Pne->UTrav[i] = Pne->UminTrav[i];	
	}
	else if ( Pne->TypeDeBorneTrav[i] == VARIABLE_NON_BORNEE ) {
          Pne->UTrav[i] = 0.0;	
	}
	else {
          Pne->YaUneSolution = NON_PNE;
	  break;
	}
      }
      else {
        Pne->YaUneSolution = NON_PNE;
	break;
      }
    }
  }	
}

if ( Pne->YaUneSolution != OUI_PNE ) goto FinDuBranchAndBound;
	
if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( Pne->FaireDuPresolve == OUI_PNE ) {
    printf("End of presolve       ->");
    printf(" rows: %6d",Pne->NombreDeContraintesTrav);
    printf(" columns(unknowns): %6d",NbVarLibres);
    printf(" including %d binaries",NbVarEntieresLibres);
    printf("\n");
  }
	/* Calcul du nombre de termes non nuls dans la matrice */
	Nz = 0;
  for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
    if ( Pne->TypeDeBorneTrav[i] == VARIABLE_FIXE ) continue;
    ic = Pne->CdebTrav[i];
    while ( ic >= 0 ) {
		  if ( Pne->ATrav[ic] != 0.0 ) Nz++;
      ic = Pne->CsuiTrav[ic];		  
		}
  }
  printf("Non zeros             -> %d", Nz);
  printf("\n\n");  
}

if ( NbVarLibres > 0 && Pne->NombreDeContraintesTrav > 0 ) { 
  Pne->YaUneSolution = BB_BranchAndBound( Pne,
                                          Pne->TempsDExecutionMaximum,
                                          Pne->NombreMaxDeSolutionsEntieres,
				  	                              Pne->ToleranceDOptimalite, 
                                          Pne->NombreDeVariablesTrav, 
                                          Pne->NombreDeContraintesTrav, 
                                          Pne->NombreDeVariablesEntieresTrav, 
                                          Pne->AffichageDesTraces,
                                          Pne->NumerosDesVariablesEntieresTrav ); 
}
else Pne->YaUneSolution = OUI;

if ( Pne->YaUneSolution == OUI ) {
  Pne->YaUneSolution = SOLUTION_OPTIMALE_TROUVEE;
}
else if ( Pne->YaUneSolution == ARRET_CAR_TEMPS_MAXIMUM_ATTEINT ) {
  Pne->YaUneSolution = ARRET_PAR_LIMITE_DE_TEMPS_AVEC_SOLUTION_ADMISSIBLE_DISPONIBLE; 
}
else if ( Pne->YaUneSolution == BB_ERREUR_INTERNE ) {
  Pne->YaUneSolution = ARRET_CAR_ERREUR_INTERNE; 
}
else {
  Pne->YaUneSolution = PAS_DE_SOLUTION_TROUVEE;
}

FinDuBranchAndBound:

if ( Pne->YaUneSolution == SOLUTION_OPTIMALE_TROUVEE || 
     Pne->YaUneSolution == ARRET_PAR_LIMITE_DE_TEMPS_AVEC_SOLUTION_ADMISSIBLE_DISPONIBLE ) { 
  #if VERBOSE_PNE	
    printf(" ******************************************** \n");   
    if ( Pne->YaUneSolution == OUI ) {		       
      printf(" OPTIMUM ATTEINT \n"); 
    }
    else if ( Pne->YaUneSolution == ARRET_CAR_TEMPS_MAXIMUM_ATTEINT ) {
      printf(" SOLUTION ADMISSIBLE TROUVEE  \n"); 
    }
  #endif
	
  PNE_RecupererLaSolutionEtCalculerLeCritere( Pne, Probleme );

}

LibererLeProbleme:

*YaUneSolution = Pne->YaUneSolution;

PNE_LibereProbleme( Pne );

return;
}
