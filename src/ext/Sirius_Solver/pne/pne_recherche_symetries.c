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

   FONCTION: Recherche de symetries.						 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
   
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE   	
  # include "pne_memoire.h"
# endif

# define TRACES 1

/*----------------------------------------------------------------------------*/

void PNE_RechercheSymetries( PROBLEME_PNE * Pne, int VariableInstanciee , int * BasesFilsDisponibles )                    

{
int NombreDeVariables; int NombreDeContraintes; int * Mdeb; int * NbTerm;
int * Nuvar; double * A; double * CoutLineaire; int Cnt; int il; int Nb;
int Var1; int Var2; int ic; int * Cdeb; int * Csui; int * NumContrainte; 
char * SensContrainte; double * B; double * ColonneDeVar1; int NbTermesVar1;
int ilMax; double * ContrainteCnt1;
double * Buffer; int * TypeDeVariable; double * ColonneDeVar2; int * TypeDeBorne;
char * ContrainteActivable; double * Xmin; double * Xmax; double Smax; int NbT;
char YaDesSymetries;

return;

printf("RechercheSymetries\n");

YaDesSymetries = NON_PNE;
/*Pne->NbVarGauche = 0;*/

NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
CoutLineaire = Pne->LTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

Cdeb = Pne->CdebTrav;   
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

Buffer = (double *) malloc( (NombreDeContraintes+NombreDeContraintes+NombreDeVariables+NombreDeContraintes)* sizeof( double ) );
if ( Buffer == NULL ) return;
memset( (char *) Buffer, 0, (NombreDeContraintes+NombreDeContraintes+NombreDeVariables+NombreDeContraintes)* sizeof( double ) );
ColonneDeVar1 = Buffer;
ColonneDeVar2 = &Buffer[NombreDeContraintes];
ContrainteCnt1 = &Buffer[NombreDeContraintes+NombreDeContraintes];
ContrainteActivable = (char *) &Buffer[NombreDeContraintes+NombreDeContraintes+NombreDeVariables];
/* Activite des contraintes */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) ContrainteActivable[Cnt] = OUI_PNE;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) ColonneDeVar1[Cnt] = 0.0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) ColonneDeVar2[Cnt] = 0.0;



for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( SensContrainte[Cnt] != '<' ) continue;
  Smax = 0;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
	  if ( A[il] > 0 ) {
		  if ( TypeDeBorne[Nuvar[il]] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorne[Nuvar[il]] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			  Smax = A[il] * Xmax[Nuvar[il]];
			}
			else {
			  Smax = LINFINI_PNE;
				break;
			}
		}
		else {
		  if ( TypeDeBorne[Nuvar[il]] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorne[Nuvar[il]] == VARIABLE_BORNEE_INFERIEUREMENT ) {	
		    Smax = A[il] * Xmin[Nuvar[il]];
			}
			else {
			  Smax = LINFINI_PNE;
				break;
			}			
		}
    il++;
  }
	if ( Smax < B[Cnt] ) {
	  ContrainteActivable[Cnt] = NON_PNE;
	}	
}

for ( Var1 = 0 ; Var1 < NombreDeVariables ; Var1++ ) {
  if ( Var1 != VariableInstanciee ) continue;
  if ( TypeDeVariable[Var1] != ENTIER ) continue;
  /* Expand de la colonne de Var1 */
  NbTermesVar1 = 0;
  ic = Cdeb[Var1];
  while ( ic >= 0 ) {
	  if ( ContrainteActivable[NumContrainte[ic]] == OUI_PNE ) {
      ColonneDeVar1[NumContrainte[ic]] = A[ic];
	    NbTermesVar1++;
		}
    ic = Csui[ic];
  }	
  for ( Var2 = 0/*Var1 + 1*/ ; Var2 < NombreDeVariables ; Var2++ ) {
	  if ( Var2 == Var1 ) continue;
    if ( TypeDeVariable[Var2] != ENTIER ) continue;
		if ( CoutLineaire[Var1] != CoutLineaire[Var2] ) continue;
		
    /* Comparaison avec la colonne de Var2: elle doit etre soit identique soit avec 2 termes differents au plus */
		Nb = NbTermesVar1;
		NbT = 0;
    ic = Cdeb[Var2];
    while ( ic >= 0 ) {	
	    if ( ContrainteActivable[NumContrainte[ic]] == OUI_PNE ) {
			  NbT++;
			  if ( NbT > NbTermesVar1 ) {
			    Nb = 1;
				  break;
			  }					
		    if ( ColonneDeVar1[NumContrainte[ic]] == A[ic] ) Nb--;
      }
			ic = Csui[ic];
    }
		
		
    if ( Nb	== 0 ) {
		  printf("Colonnes %d et %d egales\n",Var1,Var2);
      YaDesSymetries = OUI_PNE;
			/*
			Pne->ValeurAGauche = 0;
      Pne->PaquetDeGauche[Pne->NbVarGauche] = Var2;
      Pne->NbVarGauche++;			
      *BasesFilsDisponibles = NON_PNE;
			*/
		}
		
	}
	
  /* On remet tout en ordre pour la variable Var1 */
  ic = Cdeb[Var1];
  while ( ic >= 0 ) {
    ColonneDeVar1[NumContrainte[ic]] = 0.0;
    ic = Csui[ic];
  }
	
}

free( Buffer );

if ( YaDesSymetries == OUI_PNE ) {
  /*
  Pne->ValeurADroite = 0;
  Pne->PaquetDeDroite[Pne->NbVarDroite] = VariableInstanciee;
  Pne->NbVarDroite++;
	*/
}

return;
}
