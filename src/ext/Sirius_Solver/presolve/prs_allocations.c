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

   FONCTION: Allocation des structures pour le Presolve-> 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"   

# include "pne_define.h"

# include "prs_define.h"

# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PRS_AllocationsStructure( PRESOLVE * Presolve )       
{		         
int Var; int NbCntAlloc; PROBLEME_PNE * Pne; int NombreDeVariables; int NombreDeContraintes;
int NbT; int ic; int * ParColonnePremiereVariable; int * ParColonneVariableSuivante; int * Cdeb;
int * Csui; double * A; int * NumContrainte; char * ContrainteInactive; int * TypeDeBornePourPresolve;
int MxTrm; int il; int ilMax; int Cnt; int * ParLignePremiereContrainte; int * ParLigneContrainteSuivante;
int * Mdeb; int * NbTerm; int * Nuvar;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NbCntAlloc = Pne->NombreDeContraintesAllouees;
NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

Presolve->MinContrainte = (double *) malloc( NbCntAlloc * sizeof( double ) );
Presolve->MaxContrainte = (double *) malloc( NbCntAlloc * sizeof( double ) );
Presolve->MinContrainteCalcule = (char *)   malloc( NbCntAlloc * sizeof( char   ) );
Presolve->MaxContrainteCalcule = (char *)   malloc( NbCntAlloc * sizeof( char   ) );

Presolve->Lambda = (double *) malloc( NbCntAlloc * sizeof( double ) );
Presolve->LambdaMin = (double *) malloc( NbCntAlloc * sizeof( double ) );
Presolve->LambdaMax = (double *) malloc( NbCntAlloc * sizeof( double ) );
Presolve->ConnaissanceDeLambda = (char *) malloc( NbCntAlloc * sizeof( char ) );

Presolve->ContrainteInactive = (char *) malloc( NbCntAlloc * sizeof( char ) );

Presolve->ContrainteBornanteSuperieurement = (int *) malloc( NombreDeVariables * sizeof( int  ) );
Presolve->ContrainteBornanteInferieurement = (int *) malloc( NombreDeVariables * sizeof( int  ) );
/* Indicateurs pour obliger a conserver les bornes du presolve. Cela peut etre necessaire lorsqu'on supprime une
   contrainte bornante */
Presolve->ConserverLaBorneSupDuPresolve = (char *) malloc( NombreDeVariables * sizeof( char ) );
Presolve->ConserverLaBorneInfDuPresolve = (char *) malloc( NombreDeVariables * sizeof( char ) );

Presolve->ParColonnePremiereVariable = (int *) malloc( (NombreDeContraintes+1) * sizeof( int ) );
Presolve->ParColonneVariableSuivante = (int *) malloc( NombreDeVariables * sizeof( int ) );

Presolve->ParLignePremiereContrainte = (int *) malloc( (NombreDeVariables+1) * sizeof( int ) );
Presolve->ParLigneContrainteSuivante = (int *) malloc( NombreDeContraintes * sizeof( int ) );

Presolve->VariableEquivalente = (int *) malloc( NombreDeVariables  * sizeof( int ) );

/* Les bornes inf pour presolve et type de borne pour presolve sont toujours egales ou plus
   resserees que les valeurs natives */
Presolve->ValeurDeXPourPresolve = (double *) malloc( NombreDeVariables * sizeof( double ) );   
Presolve->BorneInfPourPresolve = (double *) malloc( NombreDeVariables  * sizeof( double ) );   
Presolve->BorneSupPourPresolve = (double *) malloc( NombreDeVariables  * sizeof( double ) );   
Presolve->TypeDeBornePourPresolve = (int *) malloc( NombreDeVariables  * sizeof( int ) );   
Presolve->TypeDeValeurDeBorneInf = (char *) malloc( NombreDeVariables  * sizeof( int ) );   
Presolve->TypeDeValeurDeBorneSup = (char *) malloc( NombreDeVariables  * sizeof( int ) );   

if ( Presolve->MinContrainte                    == NULL || Presolve->MaxContrainte                    == NULL || 
     Presolve->MinContrainteCalcule             == NULL || Presolve->MaxContrainteCalcule             == NULL ||
     Presolve->Lambda                           == NULL || Presolve->LambdaMin                        == NULL || 
     Presolve->LambdaMax                        == NULL || Presolve->ConnaissanceDeLambda             == NULL || 
     Presolve->ContrainteInactive               == NULL || Presolve->ContrainteBornanteSuperieurement == NULL ||
     Presolve->ParColonnePremiereVariable       == NULL || Presolve->ParColonneVariableSuivante       == NULL ||
     Presolve->ParLignePremiereContrainte       == NULL || Presolve->ParLigneContrainteSuivante       == NULL ||
     Presolve->VariableEquivalente              == NULL ||		 
		 Presolve->ContrainteBornanteInferieurement == NULL || Presolve->ConserverLaBorneSupDuPresolve    == NULL ||
     Presolve->ConserverLaBorneInfDuPresolve    == NULL || Presolve->ValeurDeXPourPresolve            == NULL ||
     Presolve->BorneInfPourPresolve             == NULL || Presolve->BorneSupPourPresolve             == NULL ||
     Presolve->TypeDeBornePourPresolve          == NULL || Presolve->TypeDeValeurDeBorneInf           == NULL || 
		 Presolve->TypeDeValeurDeBorneSup           == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PRS_AllocationsStructure \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

memset( (char *) Presolve->ConnaissanceDeLambda , LAMBDA_NON_INITIALISE, NombreDeContraintes * sizeof( char ) );	
memset( (char *) Presolve->ContrainteInactive, NON_PNE, NombreDeContraintes * sizeof( char ) );
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  Presolve->LambdaMin[Cnt] = -LINFINI_PNE;
	Presolve->LambdaMax[Cnt] = LINFINI_PNE;
}

memset( (char *) Presolve->ConserverLaBorneSupDuPresolve, NON_PNE, NombreDeVariables * sizeof( char ) );
memset( (char *) Presolve->ConserverLaBorneInfDuPresolve, NON_PNE, NombreDeVariables * sizeof( char ) );

memset( (char *) Presolve->VariableEquivalente, 0, NombreDeVariables * sizeof( int ) );

memcpy( (char *) Presolve->ValeurDeXPourPresolve, (char *) Pne->UTrav, NombreDeVariables * sizeof( double ) );
memcpy( (char *) Presolve->BorneInfPourPresolve, (char *) Pne->UminTrav, NombreDeVariables * sizeof( double ) );
memcpy( (char *) Presolve->BorneSupPourPresolve, (char *) Pne->UmaxTrav, NombreDeVariables * sizeof( double ) );
memcpy( (char *) Presolve->TypeDeBornePourPresolve, (char *) Pne->TypeDeBorneTrav, NombreDeVariables * sizeof( int ) );

memset( (char *) Presolve->TypeDeValeurDeBorneInf, VALEUR_NATIVE, NombreDeVariables * sizeof( char ) );
memset( (char *) Presolve->TypeDeValeurDeBorneSup, VALEUR_NATIVE, NombreDeVariables * sizeof( char ) );

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  Presolve->ContrainteBornanteSuperieurement[Var] = -1;
  Presolve->ContrainteBornanteInferieurement[Var] = -1;	
}

/* Classement des colonnes en fonction du nombre de termes */
ParColonnePremiereVariable = Presolve->ParColonnePremiereVariable;
ParColonneVariableSuivante = Presolve->ParColonneVariableSuivante;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ContrainteInactive = Presolve->ContrainteInactive;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
A = Pne->ATrav;
NumContrainte = Pne->NumContrainteTrav;
for ( NbT = 0 ; NbT <= NombreDeContraintes ; NbT++ ) ParColonnePremiereVariable[NbT] = -1;
MxTrm = -1;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) continue;
  NbT = 0;
	ic = Cdeb[Var];
  while ( ic >= 0 ) {
		if ( A[ic] != 0 ) {
      if ( ContrainteInactive[NumContrainte[ic]] != OUI_PNE ) NbT++;
		}
		ic = Csui[ic];
  }
	if ( NbT > MxTrm ) MxTrm = NbT;	
	ic = ParColonnePremiereVariable[NbT];	
	ParColonnePremiereVariable[NbT] = Var;	  
	ParColonneVariableSuivante[Var] = ic;	
}
Presolve->NbMaxTermesDesColonnes = MxTrm;

/* Classement des lignes en fonction du nombre de termes */
ParLignePremiereContrainte = Presolve->ParLignePremiereContrainte;
ParLigneContrainteSuivante = Presolve->ParLigneContrainteSuivante;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
for ( NbT = 0 ; NbT <= NombreDeVariables ; NbT++ ) ParLignePremiereContrainte[NbT] = -1;
MxTrm = -1;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
  NbT = 0;
	il = Mdeb[Cnt];
	ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
		if ( A[il] != 0 ) {
      if ( TypeDeBornePourPresolve[Nuvar[il]] != VARIABLE_FIXE ) NbT++;
		}
	  il++;
	}
	if ( NbT > MxTrm ) MxTrm = NbT;	
	il = ParLignePremiereContrainte[NbT];
 
	ParLignePremiereContrainte[NbT] = Cnt;	
	ParLigneContrainteSuivante[Cnt] = il;		
	
}
Presolve->NbMaxTermesDesLignes = MxTrm;


return;   

/* Partie transferee dans pne */

/* Preparation des infos sur les reductions qui seront utilisees dans le postsolve */
if ( Pne->NombreDOperationsDePresolve != 0 ) {
  printf("************ Nouveau presolve apres variable probing *************************\n");
	return;
}

Pne->NombreDOperationsDePresolve = 0; 
Pne->TailleTypeDOperationDePresolve = NombreDeVariables + NombreDeContraintes;

Pne->TypeDOperationDePresolve = (char *) malloc( Pne->TailleTypeDOperationDePresolve * sizeof( char ) );
if ( Pne->TypeDOperationDePresolve == NULL ) return;
Pne->IndexDansLeTypeDOperationDePresolve = (int *) malloc( Pne->TailleTypeDOperationDePresolve * sizeof( int ) );
if ( Pne->IndexDansLeTypeDOperationDePresolve == NULL ) return;

/* Colonnes colineaires */
Pne->NbCouplesDeVariablesColineaires = 0;
Pne->PremiereVariable = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( Pne->PremiereVariable == NULL ) return;
Pne->XminPremiereVariable = (double *) malloc( NombreDeVariables * sizeof( double ) );
if ( Pne->XminPremiereVariable == NULL ) return;
Pne->XmaxPremiereVariable = (double *) malloc( NombreDeVariables * sizeof( double ) );
if ( Pne->XmaxPremiereVariable == NULL ) return;

Pne->DeuxiemeVariable = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( Pne->DeuxiemeVariable == NULL ) return;
Pne->XminDeuxiemeVariable = (double *) malloc( NombreDeVariables * sizeof( double ) );
if ( Pne->XminDeuxiemeVariable == NULL ) return;
Pne->XmaxDeuxiemeVariable = (double *) malloc( NombreDeVariables * sizeof( double ) );
if ( Pne->XmaxDeuxiemeVariable == NULL ) return;

Pne->ValeurDeNu = (double *) malloc( NombreDeVariables * sizeof( double ) );
if ( Pne->ValeurDeNu == NULL ) return;

/* Variables non bornees substituees car colonne singleton ou variables substituees a
   l'aide d'un doubleton */
PRS_AllocTablesDeSubstitution( (void *) Pne );

/* Contraintes singleton */
Pne->NbLignesSingleton = 0;   
Pne->NumeroDeLaContrainteSingleton = (int *) malloc( NombreDeContraintes * sizeof( int ) );
if ( Pne->NumeroDeLaContrainteSingleton == NULL ) return;
Pne->VariableDeLaContrainteSingleton = (int *) malloc( NombreDeContraintes * sizeof( int ) );
if ( Pne->VariableDeLaContrainteSingleton == NULL ) return;
Pne->SecondMembreDeLaContrainteSingleton = (double *) malloc( NombreDeContraintes * sizeof( double ) );
if ( Pne->SecondMembreDeLaContrainteSingleton == NULL ) return;

/* Forcing constraints */
Pne->NbForcingConstraints = 0;
Pne->NumeroDeLaForcingConstraint = (int *) malloc( NombreDeContraintes * sizeof( int ) );
if ( Pne->NumeroDeLaForcingConstraint == NULL ) return;

/* Contraintes colineaires  */
Pne->NbSuppressionsDeContraintesColineaires = 0;
Pne->ContrainteConservee = (int *) malloc( NombreDeContraintes * sizeof( int ) );
if ( Pne->ContrainteConservee == NULL ) return;
Pne->ContrainteSupprimee = (int *) malloc( NombreDeContraintes * sizeof( int ) );
if ( Pne->ContrainteSupprimee == NULL ) return;

/* Contraintes desactivees */
Pne->NombreDeContraintesInactives = 0;
Pne->NumeroDesContraintesInactives = (int *) malloc( NombreDeContraintes * sizeof( int ) );
if ( Pne->NumeroDesContraintesInactives == NULL ) return;

return;
}

/*----------------------------------------------------------------------------*/

void PRS_LiberationStructure( PRESOLVE * Presolve )
{
PROBLEME_PNE * Pne; int ilMax; int NombreDeVariables; 

if ( Presolve == NULL ) return;

# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
	Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;
  if ( Pne != NULL ) Pne->ProblemePrsDuSolveur = NULL;
  MEM_Quit( Presolve->Tas );
	return;
# endif

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;
if ( Pne != NULL ) Pne->ProblemePrsDuSolveur = NULL;
else return;

free( Presolve->MinContrainte );
free( Presolve->MaxContrainte );
free( Presolve->MinContrainteCalcule );
free( Presolve->MaxContrainteCalcule );
free( Presolve->Lambda );
free( Presolve->LambdaMin );
free( Presolve->LambdaMax );
free( Presolve->ConnaissanceDeLambda );
free( Presolve->ContrainteInactive );
free( Presolve->ContrainteBornanteSuperieurement );
free( Presolve->ContrainteBornanteInferieurement );
free( Presolve->ConserverLaBorneSupDuPresolve );
free( Presolve->ConserverLaBorneInfDuPresolve );
free( Presolve->ParColonnePremiereVariable );
free( Presolve->ParColonneVariableSuivante );
free( Presolve->ParLignePremiereContrainte );
free( Presolve->ParLigneContrainteSuivante );
free( Presolve->VariableEquivalente );
free( Presolve->ValeurDeXPourPresolve );
free( Presolve->BorneInfPourPresolve );
free( Presolve->BorneSupPourPresolve );
free( Presolve->TypeDeBornePourPresolve );
free( Presolve->TypeDeValeurDeBorneInf );
free( Presolve->TypeDeValeurDeBorneSup );

free( Presolve );

return;
 
/* Partie transferee dans pne */

/* Recuperation de la place des reductions du presolve */

if ( Pne->NombreDOperationsDePresolve != 0 ) {
  Pne->TailleTypeDOperationDePresolve = Pne->NombreDOperationsDePresolve;
  Pne->TypeDOperationDePresolve = (char *) realloc( Pne->TypeDOperationDePresolve, Pne->TailleTypeDOperationDePresolve * sizeof( char ) );
  Pne->IndexDansLeTypeDOperationDePresolve = (int *) realloc( Pne->IndexDansLeTypeDOperationDePresolve, Pne->TailleTypeDOperationDePresolve * sizeof( int ) );	
}

if ( Pne->NumeroDesVariablesSubstituees != NULL ) {
  NombreDeVariables = Pne->NbVariablesSubstituees;
	ilMax = Pne->IndexLibreVecteurDeSubstitution;
  Pne->NumeroDesVariablesSubstituees = (int *) realloc( Pne->NumeroDesVariablesSubstituees, NombreDeVariables * sizeof( int ) );
  Pne->CoutDesVariablesSubstituees = (double *) realloc( Pne->CoutDesVariablesSubstituees, NombreDeVariables * sizeof( double ) );
  Pne->ContrainteDeLaSubstitution = (int *) realloc( Pne->ContrainteDeLaSubstitution, NombreDeVariables * sizeof( int ) );	
  Pne->ValeurDeLaConstanteDeSubstitution = (double *) realloc( Pne->ValeurDeLaConstanteDeSubstitution, NombreDeVariables * sizeof( double ) );
  Pne->IndiceDebutVecteurDeSubstitution = (int *) realloc( Pne->IndiceDebutVecteurDeSubstitution, NombreDeVariables * sizeof( int ) );
  Pne->NbTermesVecteurDeSubstitution = (int *) realloc( Pne->NbTermesVecteurDeSubstitution, NombreDeVariables * sizeof( int ) );
  Pne->CoeffDeSubstitution = (double *) realloc( Pne->CoeffDeSubstitution, ilMax * sizeof( double ) );
  Pne->NumeroDeVariableDeSubstitution = (int *) realloc( Pne->NumeroDeVariableDeSubstitution, ilMax * sizeof( int ) );
}
else {
  free( Pne->NumeroDesVariablesSubstituees );
  free( Pne->CoutDesVariablesSubstituees );
  free( Pne->ContrainteDeLaSubstitution );	
  free( Pne->ValeurDeLaConstanteDeSubstitution );
  free( Pne->IndiceDebutVecteurDeSubstitution );
  free( Pne->NbTermesVecteurDeSubstitution );
  free( Pne->CoeffDeSubstitution );
  free( Pne->NumeroDeVariableDeSubstitution );
	Pne->NumeroDesVariablesSubstituees = NULL;
	Pne->CoutDesVariablesSubstituees = NULL;
	Pne->ContrainteDeLaSubstitution = NULL;	
	Pne->ValeurDeLaConstanteDeSubstitution = NULL;
	Pne->IndiceDebutVecteurDeSubstitution = NULL;	
	Pne->NbTermesVecteurDeSubstitution = NULL;
	Pne->CoeffDeSubstitution = NULL;	
	Pne->NumeroDeVariableDeSubstitution = NULL;		
}

if ( Pne->NbCouplesDeVariablesColineaires != 0 ) {
  NombreDeVariables = Pne->NbCouplesDeVariablesColineaires;
  Pne->PremiereVariable     = (int *)    realloc( Pne->PremiereVariable    , NombreDeVariables * sizeof( int ) );
  Pne->XminPremiereVariable = (double *) realloc( Pne->XminPremiereVariable, NombreDeVariables * sizeof( double ) );
  Pne->XmaxPremiereVariable = (double *) realloc( Pne->XmaxPremiereVariable, NombreDeVariables * sizeof( double ) );
  Pne->DeuxiemeVariable     = (int *)    realloc( Pne->DeuxiemeVariable    , NombreDeVariables * sizeof( int ) ); 
  Pne->XminDeuxiemeVariable = (double *) realloc( Pne->XminDeuxiemeVariable, NombreDeVariables * sizeof( double ) );
  Pne->XmaxDeuxiemeVariable = (double *) realloc( Pne->XmaxDeuxiemeVariable, NombreDeVariables * sizeof( double ) );
  Pne->ValeurDeNu           = (double *) realloc( Pne->ValeurDeNu          , NombreDeVariables * sizeof( double ) ); 
}
else {
  free( Pne->PremiereVariable );
  free( Pne->XminPremiereVariable );
  free( Pne->XmaxPremiereVariable );
  free( Pne->DeuxiemeVariable );
  free( Pne->XminDeuxiemeVariable );
  free( Pne->XmaxDeuxiemeVariable );
  free( Pne->ValeurDeNu );	
  Pne->PremiereVariable = NULL;
  Pne->XminPremiereVariable = NULL;
  Pne->XmaxPremiereVariable = NULL;		
  Pne->DeuxiemeVariable = NULL;
  Pne->XminDeuxiemeVariable = NULL;
  Pne->XmaxDeuxiemeVariable = NULL;	
  Pne->ValeurDeNu = NULL;	
}

/* Contraintes singleton */
if ( Pne->NbLignesSingleton != 0 ) {
	ilMax = Pne->NbLignesSingleton;
  Pne->NumeroDeLaContrainteSingleton   = (int *) realloc( Pne->NumeroDeLaContrainteSingleton  , ilMax * sizeof( int ) );
  Pne->VariableDeLaContrainteSingleton = (int *) realloc( Pne->VariableDeLaContrainteSingleton , ilMax * sizeof( int ) );
  Pne->SecondMembreDeLaContrainteSingleton = (double *) realloc( Pne->SecondMembreDeLaContrainteSingleton , ilMax * sizeof( double ) );						
}
else {
  free( Pne->NumeroDeLaContrainteSingleton );
  free( Pne->VariableDeLaContrainteSingleton );
  free( Pne->SecondMembreDeLaContrainteSingleton );
  Pne->NumeroDeLaContrainteSingleton = NULL;
  Pne->VariableDeLaContrainteSingleton = NULL;
  Pne->SecondMembreDeLaContrainteSingleton = NULL;	
}

/* Forcing constraints */
if ( Pne->NbForcingConstraints != 0 ) {
	ilMax = Pne->NbForcingConstraints;
  Pne->NumeroDeLaForcingConstraint = (int *) realloc( Pne->NumeroDeLaForcingConstraint, ilMax * sizeof( int ) );
}
else {
  free( Pne->NumeroDeLaForcingConstraint );
	Pne->NumeroDeLaForcingConstraint = NULL;
}

/* Contraintes colineaires */
if ( Pne->NbSuppressionsDeContraintesColineaires > 0 ) {
 	ilMax = Pne->NbSuppressionsDeContraintesColineaires;
  Pne->ContrainteConservee = (int *) realloc( Pne->ContrainteConservee, ilMax * sizeof( int ) );
  Pne->ContrainteSupprimee = (int *) realloc( Pne->ContrainteSupprimee, ilMax * sizeof( int ) );
}
else {
  free( Pne->ContrainteConservee );
  free( Pne->ContrainteSupprimee );
	Pne->ContrainteConservee = NULL;
	Pne->ContrainteSupprimee = NULL;
}

/* Contraintes inactives */
if ( Pne->NombreDeContraintesInactives != 0 ) {
	ilMax = Pne->NombreDeContraintesInactives;
  Pne->NumeroDesContraintesInactives = (int *) realloc( Pne->NumeroDesContraintesInactives, ilMax * sizeof( int ) );
}
else {
  free( Pne->NumeroDesContraintesInactives );
	Pne->NumeroDesContraintesInactives = NULL;
}

return;
}


