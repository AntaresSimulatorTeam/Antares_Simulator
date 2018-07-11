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
/***********************************************************************************

   FONCTION: Chainage des termes de la transposee des contraintes
                   
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif

/*------------------------------------------------------------------------*/
/*         Chainage des termes de la transposee des contraintes           */ 

void PI_InitATransposee( PROBLEME_PI * Pi , int TypeChainage )
{
int Var   ; int Cnt   ; int il; int ilk; int ilMax; int Colonne;
int * Cder; int * Csui; int * NumeroDeContrainte    ; int ilC;

Cder               = (int *) malloc( Pi->NombreDeVariables    * sizeof( int ) );
NumeroDeContrainte = (int *) malloc( Pi->NbTermesAllouesPourA * sizeof( int ) );
Csui               = (int *) malloc( Pi->NbTermesAllouesPourA * sizeof( int ) );
if ( Cder == NULL  || NumeroDeContrainte == NULL || Csui == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_InitATransposee \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) Pi->Cdeb[Var] = -1;

for ( Cnt = 0 ; Cnt < Pi->NombreDeContraintes ; Cnt++ ) {
  il = Pi->Mdeb[Cnt];
  ilMax = il + Pi->NbTerm[Cnt];
  while ( il < ilMax ) {
    Colonne = Pi->Indcol[il];
    if ( Pi->Cdeb[Colonne] < 0 ) {
      Pi->Cdeb[Colonne]       = il;
      NumeroDeContrainte[il] = Cnt;
      Csui[il]               = -1;
      Cder[Colonne]          = il;
    }
    else {
      ilk = Cder[Colonne];
      Csui[ilk]              = il;
      NumeroDeContrainte[il] = Cnt;
      Csui[il]               = -1;
      Cder[Colonne]          = il;
    }      
    il++;
  }
}

if ( TypeChainage == COMPACT ) {
  /* Attention Cdeb devient temporairement Cder */
  memcpy( (char *) Cder , (char *) Pi->Cdeb , Pi->NombreDeVariables * sizeof( int ) );

  for ( ilC = 0 , Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
    Pi->Cdeb   [Var] = ilC;	        
    Pi->CNbTerm[Var] = 0;
    il = Cder[Var];
    while ( il >= 0 ) {
      Pi->ACol              [ilC] = Pi->A[il];
      Pi->NumeroDeContrainte[ilC] = NumeroDeContrainte[il];
      Pi->CNbTerm[Var]++;
      ilC++;
      il = Csui[il];
    }
  }
}
else {
  memcpy( (char *) Pi->NumeroDeContrainte , (char *) NumeroDeContrainte , Pi->NbTermesAllouesPourA * sizeof( int ) );
  memcpy( (char *) Pi->Csui               , (char *) Csui               , Pi->NbTermesAllouesPourA * sizeof( int ) );
}

free( Cder );
free( NumeroDeContrainte );
free( Csui );
Cder               = NULL;
NumeroDeContrainte = NULL;
Csui               = NULL;

return;
}

