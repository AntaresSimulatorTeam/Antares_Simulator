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

   FONCTION: Switch vers le pivotage de Markowitz en cas d'instabilite
             de la factorisee
                 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/
void LU_SwitchVersMarkowitz( MATRICE * Matrice )
{
/* PB: pour l'instant je ne gere pas le fait de ne pas avoir cree de colonnes si la matrice est symetrique
   ou symetrique en structure */
   
LU_ClasserLesColonnesRestantes( Matrice );
return;
}

/*--------------------------------------------------------------------------------------------------*/
void LU_ClasserLesColonnesRestantes( MATRICE * Matrice )
{


# ifdef ON_COMPILE

int Kp; int Rang; int Colonne; int Ligne; int NbElements; int NombreDeTermes;
COLONNE_DE_LA_MATRICE ** ColonneDeLaMatrice;LIGNE_DE_LA_MATRICE **  LigneDeLaMatrice;
COLONNE_DE_LA_MATRICE * ptColonne; LIGNE_DE_LA_MATRICE * ptLigne;
int * IndiceColonne; int * IndiceLigne;

SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne; SUPER_LIGNE_DE_LA_MATRICE * ptSuperLigne;
char UtiliserLesSuperLignes;

UtiliserLesSuperLignes = Matrice->UtiliserLesSuperLignes;

/* On etait en pivotage diagonal : les lignes sont classees mais il faut classer toutes les colonnes */
Rang               = Matrice->Rang;
ColonneDeLaMatrice = Matrice->ColonneDeLaMatrice;

printf("Changement vers pivot total Matrice->Kp %d\n",Matrice->Kp);

/* Si la matrice etait symetrique, il faut construire un chainage par colonne */
if ( Matrice->LaMatriceEstSymetrique != OUI_LU ) goto ClassementDesColonnes;

LigneDeLaMatrice = Matrice->LigneDeLaMatrice;

SuperLigneDeLaLigne = Matrice->SuperLigneDeLaLigne;

for ( Kp = Matrice->Kp ; Kp < Rang ; Kp++ ) {
  Colonne = Matrice->OrdreColonne[Kp];
  ptColonne = ColonneDeLaMatrice[Colonne];
  /* La colonne aura le meme nombre de termes que la ligne */
  Ligne          = Colonne;

  if ( UtiliserLesSuperLignes == OUI_LU ) {
    if ( SuperLigneDeLaLigne[Ligne] == NULL ) {
      ptLigne        = LigneDeLaMatrice[Ligne];
      NombreDeTermes = ptLigne->NombreDeTermes;
      IndiceColonne  = ptLigne->IndiceColonne;  
    }
    printf("   super ligne \n");
    ptSuperLigne   = SuperLigneDeLaLigne[Ligne];
    NombreDeTermes = ptSuperLigne->NombreDeTermes;
    IndiceColonne  = ptSuperLigne->IndiceColonne;    
  }
  else {
    ptLigne        = LigneDeLaMatrice[Ligne];
    NombreDeTermes = ptLigne->NombreDeTermes;
    IndiceColonne  = ptLigne->IndiceColonne;  
  }
   
  ptColonne->NombreDeTermes = NombreDeTermes;
  
  NbElements = NombreDeTermes + Matrice->MargePourCreationDeTermesColonnes;
  ptColonne->DernierPossible = NbElements - 1;
  
  IndiceLigne = (int *) malloc( NbElements * sizeof( int ) );
  if ( IndiceLigne == NULL ) {          
    printf("Factorisation LU, sous-programme LU_ConstruireProbleme: \n");
    printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
    Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
    longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
  }

  memcpy( (char *) IndiceLigne, (char *) IndiceColonne, NombreDeTermes * sizeof( int ) );
  free( ptColonne->IndiceLigne );
  ptColonne->IndiceLigne = IndiceLigne;
}

ClassementDesColonnes:
for ( Kp = Matrice->Kp ; Kp < Rang ; Kp++ ) {
  Colonne = Matrice->OrdreColonne[Kp];
  printf("Classement colonne %d Nbtermes %d\n",Colonne,ColonneDeLaMatrice[Colonne]->NombreDeTermes);
  LU_ClasserUneColonne( Matrice, Colonne, ColonneDeLaMatrice[Colonne]->NombreDeTermes); 
}

Matrice->LaMatriceEstSymetrique  = NON_LU;
Matrice->FaireDuPivotageDiagonal = NON_LU;

# endif

return;
}
