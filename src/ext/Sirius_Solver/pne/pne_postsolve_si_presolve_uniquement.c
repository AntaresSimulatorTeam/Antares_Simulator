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

   FONCTION: Postsolve (apres le presolve) quand on ne demande que le
	           presoolve. Dans ce cas on ne recupere que les variables
						 fixees ou les bornes qui ont ete reduites.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "prs_define.h"

void PNE_PostSolveVariablesSubstitueesSiUniquementPresolve( PROBLEME_PNE * , PRESOLVE * , int );

/*----------------------------------------------------------------------------*/

void PNE_PostSolveSiUniquementPresolve( PROBLEME_PNE * Pne, PROBLEME_A_RESOUDRE * Probleme ) 
{
int i; int j; char * TypeDOperationDePresolve; int * IndexDansLeTypeDOperationDePresolve;
int NombreDeVariablesE; int * TypeDeBorneTravE; double * UmaxE; double * UminE;
int VarE; int * TypeDeBornePourPresolve; double * ValeurDeXPourPresolve; int * TypeDeBorneTrav;
double * BorneInfPourPresolve; double * BorneSupPourPresolve; int * CorrespondanceVarEntreeVarNouvelle;
double * UminTrav; double * UmaxTrav; PRESOLVE * Presolve; 

Presolve = (PRESOLVE *)	Pne->Controls->Presolve;

UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
CorrespondanceVarEntreeVarNouvelle = Pne->CorrespondanceVarEntreeVarNouvelle;  

TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;

/* Important: si on n'a demande que le presolve, il n'y a pas eu de translation de
   borne */
/* Dans ce mode d'appel une variable d'entree ne peut jamais etre du type
   VARIABLE_BORNEE_SUPERIEUREMENT. Les seuls types de bornes possibles sont:
     VARIABLE_NON_BORNE
	   VARIABLE_BORNEE_DES_DEUX_COTES
	   VARIABLE_BORNEE_INFERIEUREMENT
	 et on n'a donc jamais de variables a inverser */
	 
for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  if ( TypeDeBornePourPresolve[i] == VARIABLE_FIXE ) {
	  /* Attention: a ce stade on peut avoir des variables fixes qui
		   n'ont ete positionnees a fixes que parce qu'elles ont ete
			 substituees */
    UminTrav[i] = ValeurDeXPourPresolve[i]; 
    UmaxTrav[i] = ValeurDeXPourPresolve[i];		
	}
	else {
    UminTrav[i] = BorneInfPourPresolve[i]; 
    UmaxTrav[i] = BorneSupPourPresolve[i];
	}
}		

/* Recuperation des substitutions de variables et des variables colineaire */
/* Si presolve uniquement on ne teste pas les colonnes colineaires */
																			
TypeDOperationDePresolve = Pne->TypeDOperationDePresolve;
IndexDansLeTypeDOperationDePresolve = Pne->IndexDansLeTypeDOperationDePresolve;

for ( j = Pne->NombreDOperationsDePresolve - 1 ; j >= 0 ; j-- ) {
  i = IndexDansLeTypeDOperationDePresolve[j];	
  if ( TypeDOperationDePresolve[j] == SUPPRESSION_VARIABLE_NON_BORNEE ||
	     TypeDOperationDePresolve[j] == SUBSITUTION_DE_VARIABLE ) {		
    PNE_PostSolveVariablesSubstitueesSiUniquementPresolve( Pne, Presolve, i );				
	}
}


NombreDeVariablesE = Probleme->NombreDeVariables;
TypeDeBorneTravE = Probleme->TypeDeBorneDeLaVariable;
UmaxE = Probleme->Xmax;  
UminE = Probleme->Xmin;
CorrespondanceVarEntreeVarNouvelle = Pne->CorrespondanceVarEntreeVarNouvelle;  

for ( VarE = 0 ; VarE < NombreDeVariablesE ; VarE++ ) {
  i = CorrespondanceVarEntreeVarNouvelle[VarE];	
	if ( i < 0 ) continue;
  UmaxE[VarE] = UmaxTrav[i];  
  UminE[VarE] = UminTrav[i];
}

return;
}

/*----------------------------------------------------------------------------*/
/* Recuperation des substitutions de variables        */

void PNE_PostSolveVariablesSubstitueesSiUniquementPresolve( PROBLEME_PNE * Pne,
                                                            PRESOLVE * Presolve,
                                                            int i )
{
int Var; double S; double * CoeffDeSubstitution; int il; int ilMax; char VariableFixe;
int * NumeroDeVariableDeSubstitution; double Smin; double Smax; double * ValeurDeXPourPresolve;
double * BorneInfPourPresolve; double * BorneSupPourPresolve; int * TypeDeBornePourPresolve;
double c;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;

CoeffDeSubstitution = Pne->CoeffDeSubstitution;
NumeroDeVariableDeSubstitution = Pne->NumeroDeVariableDeSubstitution;

S = Pne->ValeurDeLaConstanteDeSubstitution[i];
Smin = S;
Smax = S;
VariableFixe = OUI_PNE;
il = Pne->IndiceDebutVecteurDeSubstitution[i];
ilMax = il + Pne->NbTermesVecteurDeSubstitution[i];
while ( il < ilMax ) {
  Var = NumeroDeVariableDeSubstitution[il];
	if ( TypeDeBornePourPresolve[Var] != VARIABLE_FIXE ) VariableFixe = NON_PNE;
	c = CoeffDeSubstitution[il];
  S += c * ValeurDeXPourPresolve[Var];
	if ( c > 0 ) {
    Smin += c * BorneInfPourPresolve[Var];
    Smax += c * BorneSupPourPresolve[Var];
	}
	else {
    Smin += c * BorneSupPourPresolve[Var];
    Smax += c * BorneInfPourPresolve[Var];		
	}
	il++;
}
Var = Pne->NumeroDesVariablesSubstituees[i];
/* Si c'est une variable remplacee elle a ete mise a VARIABLE_FIXE dans le presolve donc
   il se peut qu'elle ne soit pas fixe en realite.
	 Elle n'est fixe que si toutes les variables qui servent a la calculer sont fixes */
Pne->UTrav[Var] = S;
if ( VariableFixe == OUI_PNE ) {
  Pne->UminTrav[Var] = S;
  Pne->UmaxTrav[Var] = S;
}
else {
  Pne->UminTrav[Var] = Smin;
  Pne->UmaxTrav[Var] = Smax;
}

return;
}

/*----------------------------------------------------------------------------*/
