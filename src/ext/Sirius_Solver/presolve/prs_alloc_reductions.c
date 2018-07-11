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

   FONCTION: Alloc des structures pour les operations de reduction du
	           presolve.

						 RQ: transfere dans pne
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"
   
# define TRACES 0																												

/*----------------------------------------------------------------------------*/
void PRS_AllocTablesDeSubstitution( void * ProblemePne )  
{
int NombreDeVariables; int ilMax; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) ProblemePne;

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

