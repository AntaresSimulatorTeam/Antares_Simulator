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

   FONCTION: Appele par le branch and bound pour archiver la solution
             courante lorsqu'elle est admissible et que c'est la 
             meilleur trouvee, et pour restituer la solution optimale
             qui a ete archivee
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# include "bb_define.h"
# include "bb_fonctions.h"

void PNE_ModifierLaContrainteDeCoutMax( PROBLEME_PNE * );

/*----------------------------------------------------------------------------*/

void PNE_ModifierLaContrainteDeCoutMax( PROBLEME_PNE * Pne ) 
{
int Cnt; int Var; PROBLEME_SPX * Spx; double BCoutMax; int * TypeDeBorneSv;
double * CoutLineaire; double * X; int NombreDeVariables; int NombreDeContraintes;
double * Xopt; double * XminSv; double * XmaxSv; int CntSpx; int VarSpx;
BB * Bb; double AbsBCoutMax;

NombreDeVariables = Pne->NombreDeVariablesTrav ;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
TypeDeBorneSv = Pne->TypeDeBorneTravSv;
CoutLineaire = Pne->LTrav;
X = Pne->UTrav;
Xopt = Pne->UOpt;
XminSv = Pne->UminTravSv;
XmaxSv = Pne->UmaxTravSv;

Cnt = Pne->NumeroDeLaContrainteDeCoutMax;
/* Controle */
if ( Cnt != NombreDeContraintes - 1 ) {
  printf("Attention, erreur grave detectee dans PNE_ModifierLaContrainteDeCoutMax:\n");
	printf("   La contrainte de cout max n'est pas la derniere contrainte ce qui devrait etre le cas.\n");
}
else {
  /* On peut utiliser Bb->CoutDeLaMeilleureSolutionEntiere car ici on est appele par la partie
	   Branch and Bound */
	BCoutMax = 0.0;
  Bb = (BB *) Pne->ProblemeBbDuSolveur;
	if ( Bb != NULL ) {  
		BCoutMax = Pne->CoutOpt;		
		AbsBCoutMax = fabs( BCoutMax );
		if ( AbsBCoutMax > 1.e+7 ) BCoutMax -= 1.0;
		else if ( AbsBCoutMax > 1.e+5 ) BCoutMax -= 0.1;
		else if ( AbsBCoutMax > 1.e+3 ) BCoutMax -= 0.01;
 		else if ( AbsBCoutMax > 1.e+1 ) BCoutMax -= 0.001;
 		else if ( AbsBCoutMax > 1.e-1 ) BCoutMax -= 0.0001;
 		else if ( AbsBCoutMax > 1.e-3 ) BCoutMax -= 0.00001;
		else if ( AbsBCoutMax > 1.e-5 ) BCoutMax -= 0.000001;
   	else                            BCoutMax -= 0.0000001;
		
		/*printf("CoutOpt %e BCoutMax %e ecart %e\n",Pne->CoutOpt,BCoutMax,Pne->CoutOpt-BCoutMax);*/				
    Pne->BTrav[Cnt] = BCoutMax;
	}
  /* Attention, il faut aussi modifier dans le simplexe car le hot start standard de ce simplexe ne prend en compte
	   automatiquement que les changements de bornes */
  Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;
	if ( Spx != NULL ) {
    for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
	    /* La participation des variables fixes doit etre deduite de BCoutMax */
      if ( TypeDeBorneSv[Var] == VARIABLE_FIXE ) {
		    BCoutMax -= CoutLineaire[Var] * X[Var];
		  }	
    }	
	  /* On met a jour certaines valeurs dans le simplexe */
	  CntSpx = Spx->CorrespondanceCntEntreeCntSimplexe[Cnt];
    if ( CntSpx >= 0 && CntSpx < Spx->NombreDeContraintesDuProblemeSansCoupes ) {
      Spx->BAvantTranslationEtApresScaling[CntSpx] = BCoutMax * Spx->ScaleB[CntSpx];
	  }
	  else {
      printf("Attention, erreur grave detectee dans PNE_ModifierLaContrainteDeCoutMax:\n");
	    printf("   La contrainte de cout max n'existe pas dans le simplexe.\n");
	  }
	}
}
 
Var = Pne->NumeroDeLaVariableDEcartPourCoutMax;
/* Controle */
if ( Var < 0 ) {
  printf("Attention, erreur grave detectee dans PNE_ModifierLaContrainteDeCoutMax:\n");
	printf("   La variable d'ecart de la contrainte de cout max n'est pas la derniere variable ce qui devrait etre le cas.\n");
}
else {
  TypeDeBorneSv[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
  XminSv[Var] = 0.;  
  XmaxSv[Var] = LINFINI_PNE;	
  Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;
  if ( Spx != NULL ) {
    VarSpx = Spx->CorrespondanceVarEntreeVarSimplexe[Var];
    if ( VarSpx >= 0 ) {
		  Spx->XminEntree[VarSpx] = 0.0;
	  }
	  else {
      printf("Attention, erreur grave detectee dans PNE_ModifierLaContrainteDeCoutMax:\n");
	    printf("   La variable d'ecart de la contrainte de cout max n'existe pas dans le simplexe.\n");
	  }
  }	
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_ArchiverLaSolutionCourante( PROBLEME_PNE * Pne ) 
{
int Cnt; int Var; double * CoutLineaire; double * X; int NombreDeVariables;
int NombreDeContraintes; double * Xopt; double CoutOpt; double * VariablesDualesDesContraintes;
double * VariablesDualesDesContraintesEtDesCoupes;

NombreDeVariables = Pne->NombreDeVariablesTrav ;
NombreDeContraintes = Pne->NombreDeContraintesTrav;  
CoutLineaire = Pne->LTrav;
X = Pne->UTrav;
Xopt = Pne->UOpt;

Pne->YaUneSolutionEntiere = OUI_PNE;

CoutOpt = Pne->Z0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  Xopt[Var] = X[Var];
  CoutOpt += CoutLineaire[Var] * X[Var];  
}
Pne->CoutOpt = CoutOpt;

VariablesDualesDesContraintes = Pne->VariablesDualesDesContraintesTrav;
VariablesDualesDesContraintesEtDesCoupes = Pne->VariablesDualesDesContraintesTravEtDesCoupes;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  VariablesDualesDesContraintes[Cnt] = VariablesDualesDesContraintesEtDesCoupes[Cnt];
}

# if UTILISER_UNE_CONTRAINTE_DE_COUT_MAX == OUI_PNE
  if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
    PNE_ModifierLaContrainteDeCoutMax( Pne );
  }
# endif

/* On fait le reduced cost fixing au noeud racine */
if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
  PNE_ReducedCostFixingAuNoeudRacine( Pne );												
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_RestituerLaSolutionArchivee( PROBLEME_PNE * Pne ) 
{
memcpy( (char *) Pne->UTrav , (char *) Pne->UOpt , Pne->NombreDeVariablesTrav * sizeof( double ) );
return;
}

