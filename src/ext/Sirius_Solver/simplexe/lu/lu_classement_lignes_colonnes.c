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

   FONCTION: Decomposition LU de la base.
             Classement des lignes et des colonnes en fonction du
	     nombre de termes.
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif

/*--------------------------------------------------------------------------------------------------*/
/*  Classement de toutes les lignes et de toutes les colonnes en fonction de leur nombre de termes  */

void LU_ClasserToutesLesLignesEtToutesLesColonnes( MATRICE * Matrice )
{
int i; int il; int ilDeb; double X; char FaireDuPivotageDiagonal; double * Elm; int * IndiceColonne;
int * Ldeb; int * LNbTerm; int * CNbTerm;

FaireDuPivotageDiagonal = Matrice->FaireDuPivotageDiagonal;

if ( FaireDuPivotageDiagonal == OUI_LU ) {
  memset( (char *) Matrice->AbsValeurDuTermeDiagonal , 0 , Matrice->Rang * sizeof( double ) );
}


Ldeb          = Matrice->Ldeb;
LNbTerm       = Matrice->LNbTerm;
Elm           = Matrice->Elm;
IndiceColonne = Matrice->LIndiceColonne;

CNbTerm = Matrice->CNbTerm;

for ( i = 0 ; i < Matrice->Rang ; i++ ) {
  LU_ClasserUneLigne( Matrice , i , LNbTerm[i] );
  if ( FaireDuPivotageDiagonal != OUI_LU ) {
    LU_ClasserUneColonne( Matrice , i , CNbTerm[i] );
  }
  /* Si l'on a choisi l'option du pivotage diagonal */
  if ( FaireDuPivotageDiagonal == OUI_LU ) {
		ilDeb = Ldeb[i];
    for ( il = ilDeb ; il < ilDeb + LNbTerm[i] ; il++ ) {
      if ( IndiceColonne[il] == i ) {
        X = fabs( Elm[il] ); 
	      if ( X < Matrice->PivotMin ) {	
	        /* Pivot non eligible, on declasse la ligne */
	        /* Attention ca peut poser des pb car on declasse les lignes dans Elimination des lignes */
          /* LU_DeClasserUneLigne( Matrice , i , ptLigne->NombreDeTermes ); */	  
	      }
	      else {
	        Matrice->AbsValeurDuTermeDiagonal[i] = X;
	      }
	      break;
      }
    }
  }    
}

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                 Classement d'une ligne en fonction de son nombre de termes                       */

void LU_ClasserUneLigne( MATRICE * Matrice , int Ligne , int NbTermes ) 
{
int SuivLigne;

if ( NbTermes <= 1 ) {
  if ( Matrice->LigneRejeteeTemporairementPourPivotage[Ligne] == OUI_LU ) {	
    goto FinClasserUneLigne;
  }
}

SuivLigne                    = Matrice->PremLigne[NbTermes];
Matrice->PremLigne[NbTermes] = Ligne;
Matrice->SuivLigne[Ligne]    = SuivLigne;

if ( SuivLigne >= 0 ) Matrice->PrecLigne[SuivLigne] = Ligne;

FinClasserUneLigne:
return;
}

/*--------------------------------------------------------------------------------------------------*/
/*      En enleve la ligne de son chainage en fonction de son nombre de termes 
        (elle sera reclassée ensuite). Remarque: une ligne doit etre dechainee avant 
        d'etre chainee a nouveau                                                                    */

void LU_DeClasserUneLigne( MATRICE * Matrice , int Ligne , int NbTermes ) 
{
int LPrec; int LSuiv;

if ( Matrice->PremLigne[NbTermes] == Ligne ) {
  /* C'est le premier element qu'on enleve */
  Matrice->PremLigne[NbTermes] = Matrice->SuivLigne[Ligne];  
  Matrice->PrecLigne[Ligne] = -1;
  goto FinDeClasserUneLigne;
}

/* On enleve un element qui n'est pas le premier */

LPrec = Matrice->PrecLigne[Ligne];
if ( LPrec < 0 ) return; /* La ligne n'est pas classee (pour left looking) */
LSuiv = Matrice->SuivLigne[Ligne];
Matrice->SuivLigne[LPrec] = LSuiv;
if ( LSuiv >= 0 ) Matrice->PrecLigne[LSuiv] = LPrec;
Matrice->PrecLigne[Ligne] = -1;

FinDeClasserUneLigne:
return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                 Classement d'une colonne en fonction de son nombre de termes                     */

void LU_ClasserUneColonne( MATRICE * Matrice , int Colonne , int NbTermes ) 
{
int SuivColonne;

if ( NbTermes <= 1 ) {
  if ( Matrice->ColonneRejeteeTemporairementPourPivotage[Colonne] == OUI_LU ) {
    /*printf("Refus de la tentative de reintroduction de la colonne %d\n",Colonne);*/
    goto FinClasserUneColonne;
  }
}

SuivColonne                   = Matrice->PremColonne[NbTermes];
Matrice->PremColonne[NbTermes] = Colonne;
Matrice->SuivColonne[Colonne]  = SuivColonne;

if ( SuivColonne >= 0 ) Matrice->PrecColonne[SuivColonne] = Colonne;

FinClasserUneColonne:
return;
}

/*--------------------------------------------------------------------------------------------------*/
/*      En enleve la colonne de son chainage en fonction de son nombre de termes 
        (elle sera reclassée ensuite). Remarque: une colonne doit etre dechainee avant 
        d'etre chainee a nouveau                                                                    */

void LU_DeClasserUneColonne( MATRICE * Matrice , int Colonne , int NbTermes ) 
{
int CPrec; int CSuiv; 

if ( Matrice->PremColonne[NbTermes] == Colonne ) {
  /* C'est le premier element qu'on enleve */
  Matrice->PremColonne[NbTermes] = Matrice->SuivColonne[Colonne];
  Matrice->PrecColonne[Colonne] = -1;
  goto FinDeClasserUneColonne;
}

/* On enleve un element qui n'est pas le premier */

CPrec = Matrice->PrecColonne[Colonne];
if ( CPrec < 0 ) return; /* La colonne n'est pas classee */
CSuiv = Matrice->SuivColonne[Colonne];
Matrice->SuivColonne[CPrec] = CSuiv;
if ( CSuiv >= 0 ) Matrice->PrecColonne[CSuiv] = CPrec;
Matrice->PrecColonne[Colonne] = -1;

FinDeClasserUneColonne:
return;
}
















