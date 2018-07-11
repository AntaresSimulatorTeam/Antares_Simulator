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

   FONCTION: Split des colonnes trop denses 

                   
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif

# define SEUIL_TERMES_COLONNE 10

/*------------------------------------------------------------------------*/
/*                          Initialisation                                */ 

void PI_SplitColonnes( PROBLEME_PI * Pi )
{
int Var; int Var1; int ic; int icMax; int il; int ilMax; int ilLibre;
int Cnt; int Count; int CNbTerm; int CNbLim; int i; int NbTerm; int ilDeb;  
int * CntModifiees; int NbCntModifiees; char OnInverse; int il1; int Isv; int Xsv;
int NombreDeVariablesSV;

return;  

PI_InitATransposee( Pi , COMPACT );

NbCntModifiees = 0;
CntModifiees = (int *) malloc( Pi->NombreDeContraintes * sizeof( int   ) );
if ( CntModifiees == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_SplitColonnes \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee );
}

Cnt = Pi->NombreDeContraintes - 1;
ilLibre = Pi->Mdeb[Cnt] + Pi->NbTerm[Cnt];

NombreDeVariablesSV = Pi->NombreDeVariables;
for ( Var = 0 ; Var < NombreDeVariablesSV ; Var++ ) {
  CNbTerm = Pi->CNbTerm[Var];
  if ( CNbTerm >= SEUIL_TERMES_COLONNE ) {
    /* On splitte la colonne */
    /* 1- Creation d'une variable de dedoublement supplementaire */
    Var1 = Pi->NombreDeVariables;
    Pi->Q     [Var1] = Pi->Q[Var];
    Pi->L     [Var1] = Pi->L[Var];
    Pi->U     [Var1] = Pi->U[Var];
    Pi->Umin  [Var1] = Pi->Umin[Var]; 
    Pi->Umax  [Var1] = Pi->Umax[Var]; 
    Pi->ScaleU[Var1] = Pi->ScaleU[Var];
    Pi->S1    [Var1] = Pi->S1[Var];
    Pi->S2    [Var1] = Pi->S2[Var];
    Pi->TypeDeVariable[Var1] = Pi->TypeDeVariable[Var];
    /* Incrementation du nombre de variables */
    Pi->NombreDeVariables++;

    /* 2- Modification des contraintes intervenant dans la colonne */
    ic = Pi->Cdeb[Var];
    CNbLim  = (int) 0.5 * CNbTerm;
    icMax = ic + CNbTerm;
    Count = 0;
    while ( ic < icMax ) {
      Count++;
      if ( Count > CNbLim ) {
        Cnt = Pi->NumeroDeContrainte[ic];
        CntModifiees[NbCntModifiees] = Cnt;
        il = Pi->Mdeb[Cnt];
        ilMax = il + Pi->NbTerm[Cnt];
        while (il < ilMax ) {
          if ( Pi->Indcol[il] == Var ) {
            Pi->Indcol[il] = Var1;
            break;
          }
          il++;    
        }
      }
      ic++;
    }

    /* Creation de la contrainte Var - Var1 = 0 */
    Pi->Mdeb  [Pi->NombreDeContraintes] = ilLibre;
    Pi->NbTerm[Pi->NombreDeContraintes] = 2;
    Pi->B     [Pi->NombreDeContraintes] = 0.0;
    Pi->A     [ilLibre] = 1;
    Pi->Indcol[ilLibre] = Var;
    ilLibre++;
    Pi->A     [ilLibre] = -1;
    Pi->Indcol[ilLibre] = Var1;
    ilLibre++;
    Pi->NombreDeContraintes++;
    
  }
}

/* Pour chaque contrainte modifiee, reclassement des termes */
for ( i = 0 ; i < NbCntModifiees ; i++ ) {
  Cnt       = CntModifiees[i];
  OnInverse = OUI_PI;
  ilDeb     = Pi->Mdeb[Cnt];
  NbTerm    = Pi->NbTerm[Cnt] - 1;
  ilMax     = ilDeb + NbTerm; 
  while ( OnInverse == OUI_PI ) {
    OnInverse = NON_PI;
    il = ilDeb;
    while (il < ilMax ) {
      il1 = il + 1;
      if ( Pi->Indcol[il] > Pi->Indcol[il1] ) {
        OnInverse = OUI_PI;
	Isv = Pi->Indcol[il];
	Xsv = Pi->A[il];
        Pi->Indcol[il]  = Pi->Indcol[il1];
	Pi->A     [il]  = Pi->A[il1];
	Pi->Indcol[il1] = Isv;
	Pi->A     [il1] = Xsv;
      }
      il++;
    }    
  }
}

free( CntModifiees );
CntModifiees = NULL;

if ( NbCntModifiees > 0 ) PI_InitATransposee( Pi , COMPACT );


return;
}	       


   
