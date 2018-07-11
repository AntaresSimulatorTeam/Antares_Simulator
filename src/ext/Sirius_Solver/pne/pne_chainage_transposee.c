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

   FONCTION: Chainage de la transposee. On stocke les contraintes
	           d'egalite avant les contraintes d'inegalite.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_ConstruireLeChainageDeLaTransposee( PROBLEME_PNE * Pne )
{
int i; int il; int ilMax; int ilk; int Ligne; int * CderTrav; int * CdebTrav;
int * MdebTrav; int * NbTermTrav; int * NuvarTrav; int * NumContrainteTrav;
int * CsuiTrav; char * SensContrainteTrav; int * CNbTermTrav;

CdebTrav = Pne->CdebTrav;
CNbTermTrav = Pne->CNbTermTrav;
MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
NuvarTrav = Pne->NuvarTrav;
NumContrainteTrav = Pne->NumContrainteTrav;
CsuiTrav = Pne->CsuiTrav;
SensContrainteTrav =  Pne->SensContrainteTrav;

CderTrav = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) ); 
if ( CderTrav == NULL ) {
  printf(" Pne, memoire insuffisante dans le sous programme PNE_ConstruireLeChainageDeLaTransposee \n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) CdebTrav[i] = -1;

/* Contraintes d'egalite en premier */
for ( i = 0 ; i < Pne->NombreDeContraintesTrav ; i++ ) {
  if ( SensContrainteTrav[i] != '=' ) continue;
  il    = MdebTrav[i];
  ilMax = il + NbTermTrav[i];
  while ( il < ilMax ) {
    Ligne = NuvarTrav[il];
    if ( CdebTrav[Ligne] < 0 ) {
      CdebTrav         [Ligne] = il;
      NumContrainteTrav[il]    = i;
      CsuiTrav         [il]    = -1;
      CderTrav         [Ligne] = il;
    }
    else {
      ilk                      = CderTrav[Ligne];
      CsuiTrav         [ilk]   = il;
      NumContrainteTrav[il]    = i;
      CsuiTrav         [il]    = -1;
      CderTrav         [Ligne] = il;
    }
    il++;
  }
}
/* Contraintes d'inegalite ensuite */
for ( i = 0 ; i < Pne->NombreDeContraintesTrav ; i++ ) {
  if ( SensContrainteTrav[i] == '=' ) continue;
  il    = MdebTrav[i];
  ilMax = il + NbTermTrav[i];
  while ( il < ilMax ) {
    Ligne = NuvarTrav[il];
    if ( CdebTrav[Ligne] < 0 ) {
      CdebTrav         [Ligne] = il;
      NumContrainteTrav[il]    = i;
      CsuiTrav         [il]    = -1;
      CderTrav         [Ligne] = il;
    }
    else {
      ilk                      = CderTrav[Ligne];
      CsuiTrav         [ilk]   = il;
      NumContrainteTrav[il]    = i;
      CsuiTrav         [il]    = -1;
      CderTrav         [Ligne] = il;
    }
    il++;
  }
}

free( CderTrav );

/* Decompte nombre de termes de chaque colonne */
for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  il = CdebTrav[i];
	ilk = 0;
	while ( il >= 0 ) {
    ilk++;
	  il = CsuiTrav[il];
	}
  CNbTermTrav[i] = ilk;
}

Pne->ChainageTransposeeExploitable = OUI_PNE;

return;
}



