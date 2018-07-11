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

   FONCTION: On calcule les valeurs de X en fonction de la position 
             hors base ou en base.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_FixerXEnFonctionDeSaPosition( PROBLEME_SPX * Spx )
{
int Var; char PositionDeLaVariable; double X; double * ArrayX; char * TypeDeVariable; 
double SommeDesInfaisabilitesPrimales; double * ArrayXmax; double * BBarre;
char * ArrayPositionDeLaVariable; int * ContrainteDeLaVariableEnBase;
char * StatutBorneSupCourante; int Index; char UtiliserLaBaseReduite;
int * OrdreColonneDeLaBaseFactorisee; int RangDeLaMatriceFactorisee;

ArrayX         = Spx->X;
ArrayXmax      = Spx->Xmax;
BBarre         = Spx->BBarre;
TypeDeVariable = Spx->TypeDeVariable;
ArrayPositionDeLaVariable    = Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
StatutBorneSupCourante       = Spx->StatutBorneSupCourante;
SommeDesInfaisabilitesPrimales = 0.;

UtiliserLaBaseReduite = Spx->UtiliserLaBaseReduite;
RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;
OrdreColonneDeLaBaseFactorisee = Spx->OrdreColonneDeLaBaseFactorisee;

/* Attention l'utilisation de Index est fausse mais comme le cout des variables d'ecart est nul
   on calcule un cout correcten mettant 0 pour la valeur de X */

if ( Spx->NombreDeBornesAuxiliairesUtilisees == 0 ) {
  for ( Var = 0 ; Var < Spx->NombreDeVariables ;  Var++ ) {
    PositionDeLaVariable = ArrayPositionDeLaVariable[Var];	
    if ( PositionDeLaVariable == EN_BASE_LIBRE ) { 
      /* La variable est donc en base */
			
			if ( UtiliserLaBaseReduite == OUI_SPX ) {
			  Index = OrdreColonneDeLaBaseFactorisee[ContrainteDeLaVariableEnBase[Var]];
				if ( Index < RangDeLaMatriceFactorisee ) {
          X = BBarre[Index]; 
				}
				else X = 0;
			}
			else {
        X = BBarre[ContrainteDeLaVariableEnBase[Var]]; 
			}
			
      ArrayX[Var] = X;			
      /* Attention ici on utilise le fait que par translation on a toujours Xmin = 0. */
      if ( X < 0. && TypeDeVariable[Var] != NON_BORNEE ) SommeDesInfaisabilitesPrimales += -X;
      else {
        if ( X > ArrayXmax[Var] ) SommeDesInfaisabilitesPrimales += X - ArrayXmax[Var];			
      }
    }
    else if ( PositionDeLaVariable == HORS_BASE_SUR_BORNE_SUP ) ArrayX[Var] = ArrayXmax[Var];
	  else ArrayX[Var] = 0.0;		
  }
}
else {
  for ( Var = 0 ; Var < Spx->NombreDeVariables ;  Var++ ) {
    PositionDeLaVariable = ArrayPositionDeLaVariable[Var];	
    if ( PositionDeLaVariable == EN_BASE_LIBRE ) { 
      /* La variable est donc en base */
		
			if ( UtiliserLaBaseReduite == OUI_SPX ) {
			  Index = OrdreColonneDeLaBaseFactorisee[ContrainteDeLaVariableEnBase[Var]];
				if ( Index < RangDeLaMatriceFactorisee ) {
          X = BBarre[Index]; 
				}
				else X = 0;
			}
			else {
        X = BBarre[ContrainteDeLaVariableEnBase[Var]]; 
			}			
						
      ArrayX[Var] = X; 
      /* Attention ici on utilise le fait que par translation on a toujours Xmin = 0. */
      if ( X < 0. && TypeDeVariable[Var] != NON_BORNEE ) SommeDesInfaisabilitesPrimales += -X;
      else {
        if ( X > ArrayXmax[Var] ) SommeDesInfaisabilitesPrimales += X - ArrayXmax[Var];			
      }
    }
    else if ( PositionDeLaVariable == HORS_BASE_SUR_BORNE_SUP ) ArrayX[Var] = ArrayXmax[Var];
	  else {
	    /* La variable est HORS_BASE_A_ZERO ou HORS_BASE_SUR_BORNE_INF */
	    ArrayX[Var] = 0.0;
      if ( PositionDeLaVariable == HORS_BASE_SUR_BORNE_INF ) {
		    if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
	        /* La variable est hors base sur borne inf mais qu'elle a une borne avec un StatutBorneSupCourante egal a
				     BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE alors elle est a -Xmax[Var] */
	        ArrayX[Var] = -ArrayXmax[Var];				
	      }
		  }
	  }
  }
}

Spx->SommeDesInfaisabilitesPrimales = SommeDesInfaisabilitesPrimales;
	
return;
}

