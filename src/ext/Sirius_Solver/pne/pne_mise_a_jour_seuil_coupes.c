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

   FONCTION: Mise a jour du seuil utilise pour les coupes.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_MiseAJourSeuilCoupes( PROBLEME_PNE * Pne, char TypeDeCoupe, double * Seuil )
{
BB * Bb; char ProblemeUniquementEnVariablesEntieres; 

Bb = (BB *) Pne->ProblemeBbDuSolveur;
ProblemeUniquementEnVariablesEntieres = NON_PNE;
if ( Pne->NombreDeVariablesEntieresTrav == Pne->NombreDeVariablesNonFixes ) ProblemeUniquementEnVariablesEntieres = OUI_PNE;

if ( TypeDeCoupe == COUPE_KNAPSACK ) {
  if ( ProblemeUniquementEnVariablesEntieres == OUI_PNE ) *Seuil = 0.1 * SEUIL_VIOLATION_KNAPSACK;
  else {
    /* Si pb mixte */
	  if ( Bb->AverageK == NON_INITIALISE ) *Seuil = SEUIL_VIOLATION_KNAPSACK;
	  else {
      if ( Bb->AverageK > 0.25 * Bb->AverageG && Bb->NoeudEnExamen != Bb->NoeudRacine ) *Seuil = Pne->SeuilDeViolationK;
      else *Seuil = SEUIL_VIOLATION_KNAPSACK;
	  }
  }
}
else if ( TypeDeCoupe == COUPE_MIR_MARCHAND_WOLSEY ) {
	if ( Bb->AverageK == NON_INITIALISE ) *Seuil = SEUIL_VIOLATION_MIR_MARCHAND_WOLSEY;
  else {
    if ( Bb->AverageK > 0.25 * Bb->AverageG ) *Seuil = Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY;
    else *Seuil = SEUIL_VIOLATION_MIR_MARCHAND_WOLSEY;
  }
}
else if ( TypeDeCoupe == COUPE_CLIQUE ) {
  if ( ProblemeUniquementEnVariablesEntieres == OUI_PNE ) *Seuil = 0.1 * SEUIL_VIOLATION_CLIQUES;
  else {
    /* Si pb mixte */
	  if ( Bb->AverageK == NON_INITIALISE ) *Seuil = SEUIL_VIOLATION_CLIQUES;
    else {		
      if ( Bb->AverageK > 0.25 * Bb->AverageG && Bb->NoeudEnExamen != Bb->NoeudRacine ) *Seuil = Pne->SeuilDeViolationCliques;
      else *Seuil = SEUIL_VIOLATION_CLIQUES;
	  }
  }
}
else if ( TypeDeCoupe == COUPE_IMPLICATION ) {
  if ( ProblemeUniquementEnVariablesEntieres == OUI_PNE ) *Seuil = 0.1 * SEUIL_VIOLATION_IMPLICATIONS;  
  else {
    /* Si pb mixte */
	  if ( Bb->AverageK == NON_INITIALISE ) *Seuil = SEUIL_VIOLATION_IMPLICATIONS;		
    else {		
      if ( Bb->AverageK > 0.25 * Bb->AverageG && Bb->NoeudEnExamen != Bb->NoeudRacine ) *Seuil = Pne->SeuilDeViolationImplications;
      else *Seuil = SEUIL_VIOLATION_IMPLICATIONS;
	  }
  }
}
else if ( TypeDeCoupe == COUPE_DE_BORNE_VARIABLE ) {
  if ( ProblemeUniquementEnVariablesEntieres == OUI_PNE ) *Seuil = 0.1 * SEUIL_VIOLATION_BORNES_VARIABLES;  
  else {
    /* Si pb mixte */
	  if ( Bb->AverageK == NON_INITIALISE ) *Seuil = SEUIL_VIOLATION_BORNES_VARIABLES;		
    else {		
      if ( Bb->AverageK > 0.25 * Bb->AverageG && Bb->NoeudEnExamen != Bb->NoeudRacine ) *Seuil = Pne->SeuilDeViolationBornesVariables;
      else *Seuil = SEUIL_VIOLATION_BORNES_VARIABLES;
	  }
  }
}
else {
  printf("PNE_MiseAJourSeuilCoupes bug: TypeDeCoupe %d inconnu\n",TypeDeCoupe);
	exit(0);
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_MiseAJourDesSeuilDeSelectionDesCoupes( PROBLEME_PNE * Pne )
{
BB * Bb; char ProblemeUniquementEnVariablesEntieres; double CoeffMin; double CoeffMax;

/* Seuil pour les Knapsack afin d'eviter d'en avoir trop */ 
Bb = Pne->ProblemeBbDuSolveur;
ProblemeUniquementEnVariablesEntieres = NON_PNE;
if ( Pne->NombreDeVariablesEntieresTrav == Pne->NombreDeVariablesNonFixes ) ProblemeUniquementEnVariablesEntieres = OUI_PNE;

if ( Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes > 0 ) {
	Bb->AverageG = (int) ceil(Bb->NombreTotalDeGDuPoolUtilisees/Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes);
  Bb->AverageK = (int) ceil(Bb->NombreTotalDeKDuPoolUtilisees/Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes);
}

if ( Bb->AverageK != NON_INITIALISE ) {
  if ( Bb->AverageK > 0.25 * Bb->AverageG ) {
    if ( Pne->NombreDeK > 0 ) Pne->SeuilDeViolationK = Pne->SommeViolationsK / Pne->NombreDeK;
    if ( Pne->NombreDeMIR_MARCHAND_WOLSEY > 0 ) Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY = Pne->SommeViolationsMIR_MARCHAND_WOLSEY / Pne->NombreDeMIR_MARCHAND_WOLSEY;
    if ( Pne->NombreDeCliques > 0 ) Pne->SeuilDeViolationCliques = Pne->SommeViolationsCliques / Pne->NombreDeCliques;
    if ( Pne->NombreDImplications > 0 ) Pne->SeuilDeViolationImplications = Pne->SommeViolationsImplications / Pne->NombreDImplications;
    if ( Pne->NombreDeBornesVariables > 0 ) Pne->SeuilDeViolationBornesVariables = Pne->SommeViolationsBornesVariables / Pne->NombreDeBornesVariables;
	}
}

CoeffMin = 1.e-2;
CoeffMax = 1.e+2;
if ( Pne->SeuilDeViolationK > CoeffMax * SEUIL_VIOLATION_KNAPSACK ) Pne->SeuilDeViolationK = CoeffMax * SEUIL_VIOLATION_KNAPSACK;
else if( Pne->SeuilDeViolationK < CoeffMin * SEUIL_VIOLATION_KNAPSACK ) Pne->SeuilDeViolationK = CoeffMin * SEUIL_VIOLATION_KNAPSACK;

if ( Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY > CoeffMax * SEUIL_VIOLATION_MIR_MARCHAND_WOLSEY ) Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY = CoeffMax * SEUIL_VIOLATION_MIR_MARCHAND_WOLSEY;
else if ( Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY < CoeffMin * SEUIL_VIOLATION_MIR_MARCHAND_WOLSEY  ) Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY = CoeffMin * SEUIL_VIOLATION_MIR_MARCHAND_WOLSEY;

if ( Pne->SeuilDeViolationCliques > CoeffMax * SEUIL_VIOLATION_CLIQUES ) Pne->SeuilDeViolationCliques = CoeffMax * SEUIL_VIOLATION_CLIQUES;
else if ( Pne->SeuilDeViolationCliques < CoeffMin * SEUIL_VIOLATION_CLIQUES ) Pne->SeuilDeViolationCliques = CoeffMin * SEUIL_VIOLATION_CLIQUES;

if ( Pne->SeuilDeViolationImplications > CoeffMax * SEUIL_VIOLATION_IMPLICATIONS ) Pne->SeuilDeViolationImplications = CoeffMax * SEUIL_VIOLATION_IMPLICATIONS;
else if ( Pne->SeuilDeViolationImplications < CoeffMin * SEUIL_VIOLATION_IMPLICATIONS ) Pne->SeuilDeViolationImplications = CoeffMin * SEUIL_VIOLATION_IMPLICATIONS;

if ( Pne->SeuilDeViolationBornesVariables > CoeffMax * SEUIL_VIOLATION_BORNES_VARIABLES ) Pne->SeuilDeViolationBornesVariables = CoeffMax * SEUIL_VIOLATION_BORNES_VARIABLES;
else if ( Pne->SeuilDeViolationBornesVariables < CoeffMin * SEUIL_VIOLATION_BORNES_VARIABLES ) Pne->SeuilDeViolationBornesVariables = CoeffMin * SEUIL_VIOLATION_BORNES_VARIABLES;

/*
printf("SeuilDeViolationK %e  SeuilDeViolationMIR_MARCHAND_WOLSEY %e SeuilDeViolationCliques %e SeuilDeViolationImplications %e SeuilDeViolationBornesVariables %e\n",
        Pne->SeuilDeViolationK,Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY,Pne->SeuilDeViolationCliques,
				Pne->SeuilDeViolationImplications,Pne->SeuilDeViolationBornesVariables);
*/

return;
}
