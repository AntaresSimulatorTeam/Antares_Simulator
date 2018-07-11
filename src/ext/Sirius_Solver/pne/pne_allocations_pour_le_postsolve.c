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

   FONCTION: Allocation des structures pour le postsolve 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
 
# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_AllocationsPourLePostSolve( PROBLEME_PNE * Pne )       
{		         
int NombreDeVariables; int NombreDeContraintes;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

/* Preparation des infos sur les reductions qui seront utilisees dans le postsolve */
if ( Pne->NombreDOperationsDePresolve != 0 ) {
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
PNE_AllocTablesDeSubstitution( Pne );

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

void PNE_AllocTablesDeSubstitution( PROBLEME_PNE * Pne )  
{
int NombreDeVariables; int ilMax; 

NombreDeVariables = Pne->NombreDeVariablesTrav;
ilMax = Pne->TailleAlloueePourLaMatriceDesContraintes;

Pne->IndexLibreVecteurDeSubstitution = 0;
Pne->NbVariablesSubstituees = 0; 

Pne->NumeroDesVariablesSubstituees = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( Pne->NumeroDesVariablesSubstituees == NULL )  return;

Pne->ValeurDeLaConstanteDeSubstitution = (double *) malloc( NombreDeVariables * sizeof( double ) );
if ( Pne->ValeurDeLaConstanteDeSubstitution == NULL ) {
  free( Pne->NumeroDesVariablesSubstituees );
	return;
}

Pne->IndiceDebutVecteurDeSubstitution = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( Pne->IndiceDebutVecteurDeSubstitution == NULL ) {
  free( Pne->NumeroDesVariablesSubstituees ); free( Pne->ValeurDeLaConstanteDeSubstitution );
	return;
}

Pne->NbTermesVecteurDeSubstitution = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( Pne->NbTermesVecteurDeSubstitution == NULL ) {
  free( Pne->NumeroDesVariablesSubstituees ); free( Pne->ValeurDeLaConstanteDeSubstitution );
  free( Pne->IndiceDebutVecteurDeSubstitution );
	return;
}

Pne->CoeffDeSubstitution = (double *) malloc( ilMax * sizeof( double ) );
if ( Pne->CoeffDeSubstitution == NULL ) {
  free( Pne->NumeroDesVariablesSubstituees ); free( Pne->ValeurDeLaConstanteDeSubstitution );
  free( Pne->IndiceDebutVecteurDeSubstitution ); free( Pne->NbTermesVecteurDeSubstitution );
	return;
}

Pne->NumeroDeVariableDeSubstitution = (int *) malloc( ilMax * sizeof( int ) );
if ( Pne->NumeroDeVariableDeSubstitution == NULL ) {
  free( Pne->NumeroDesVariablesSubstituees ); free( Pne->ValeurDeLaConstanteDeSubstitution );
  free( Pne->IndiceDebutVecteurDeSubstitution ); free( Pne->NbTermesVecteurDeSubstitution );
  free( Pne->CoeffDeSubstitution );
	return;
}

Pne->CoutDesVariablesSubstituees = (double *) malloc( NombreDeVariables * sizeof( double ) );
if ( Pne->CoutDesVariablesSubstituees == NULL )  {
  free( Pne->NumeroDesVariablesSubstituees ); free( Pne->ValeurDeLaConstanteDeSubstitution );
  free( Pne->IndiceDebutVecteurDeSubstitution ); free( Pne->NbTermesVecteurDeSubstitution );
  free( Pne->CoeffDeSubstitution ); free( Pne->NumeroDeVariableDeSubstitution );
  return;
}

Pne->ContrainteDeLaSubstitution = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( Pne->ContrainteDeLaSubstitution == NULL )  {
  free( Pne->NumeroDesVariablesSubstituees ); free( Pne->ValeurDeLaConstanteDeSubstitution );
  free( Pne->IndiceDebutVecteurDeSubstitution ); free( Pne->NbTermesVecteurDeSubstitution );
  free( Pne->CoeffDeSubstitution ); free( Pne->NumeroDeVariableDeSubstitution );
  free( Pne->CoutDesVariablesSubstituees );
  return;
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_CleanPostSolve( PROBLEME_PNE * Pne )       
{
int ilMax; int NombreDeVariables; 

# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
  if ( Pne != NULL ) Pne->ProblemePrsDuSolveur = NULL;
  MEM_Quit( Presolve->Tas );
	return;
# endif

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


