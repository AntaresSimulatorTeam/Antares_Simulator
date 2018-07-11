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

   FONCTION: Recherche des bornes variables.
	           Une borne variable est une contrainte de type
						 x < a * y avec x continue et y binaire.
						 Apres translation des bornes on a en realite
						 x < b + a * y 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif     

/*----------------------------------------------------------------------------*/
   
void PNE_AnalyseInitialeDesBornesVariables( PROBLEME_PNE * Pne )
{
int Cnt; int il; int ilMax; int Var; int NbBin; double X; 
int NbCont; int VarCont; int VarBin; double CoeffCont; double CoeffBin; int * Mdeb; int * NbTerm;
int * Nuvar; int * TypeDeVariable; double * A;  int * CntDeBorneSupVariable;
int * CntDeBorneInfVariable; char * SensContrainte; int * TypeDeBorne; int ilDeb;
char YaDesBornesVariables; double * Umin; double * Umax; double * B;  int Nb;
int NombreDeVariables; int NombreDeContraintes;

if ( Pne->CntDeBorneSupVariable != NULL && Pne->CntDeBorneInfVariable != NULL ) return;

if ( Pne->CntDeBorneSupVariable == NULL ) {   
  Pne->CntDeBorneSupVariable  = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
  if ( Pne->CntDeBorneSupVariable == NULL ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AnalyseInitialeDesBornesVariables \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }	
}
if ( Pne->CntDeBorneInfVariable == NULL ) {   
  Pne->CntDeBorneInfVariable  = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
  if ( Pne->CntDeBorneInfVariable == NULL ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AnalyseInitialeDesBornesVariables \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }	
}

Nb = 0;
YaDesBornesVariables = NON_PNE;
CntDeBorneSupVariable = Pne->CntDeBorneSupVariable;
CntDeBorneInfVariable = Pne->CntDeBorneInfVariable;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  CntDeBorneSupVariable[Var] = -1;
  CntDeBorneInfVariable[Var] = -1;
}

SensContrainte = Pne->SensContrainteTrav;
NbTerm = Pne->NbTermTrav;   
B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
Nuvar = Pne->NuvarTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;
A = Pne->ATrav;
CoeffBin = 0.0;
CoeffCont = 0.0;

/* Marquage des variables continues soumises a une borne sup ou inf variable */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( SensContrainte[Cnt] != '<' ) continue;
  if ( NbTerm[Cnt] != 2 ) continue;
	/* Rq: il faudrait ne pas tester la nullite de BTrav car il y a eu translation
	   des bornes mais ca marche pas dans les MIR de Marchand-Wolsey */		 
	if ( B[Cnt] != 0.0 ) continue;
	NbCont = 0;
	NbBin = 0;
	VarCont = -1;
	VarBin = -1;
  ilDeb = Mdeb[Cnt];
	il = ilDeb;
  ilMax = il + 2;	
  while ( il < ilMax ) { 
    Var = Nuvar[il];
    if ( TypeDeBorne[Var] != VARIABLE_FIXE ) {
      if ( TypeDeVariable[Var] == ENTIER ) {
			  NbBin++;
				VarBin = Var;
				CoeffBin = A[il];
				if ( CoeffBin == 0.0 ){
				  NbBin = 0;
					break;
				}
			}
      else {
	      NbCont++;
				VarCont = Var;
				CoeffCont = A[il];
				if ( CoeffCont == 0.0 ) {   
				  NbCont = 0;
					break;
				}
			}
		}
    il++;
  }

  /* Attention a la restriction: pour l'instant on a 1 seule variable bound par variable */
	
	if ( NbCont == 1 && NbBin == 1 && VarCont != -1 && VarBin != -1 ) {	
		Nb++;
	  YaDesBornesVariables = OUI_PNE;

    Pne->ChainageTransposeeExploitable = NON_PNE;
		
		/* C'est une contrainte de borne variable */
		/* Pour avoir la borne on a besoin du second membre et du coeff. de la variable entiere.
		   Pour cela on normalise par abs(CoeffCont). On range la variable entiere en premier
			 pour y acceder plus vite */
		X = fabs( CoeffCont );
		A[ilDeb]     = CoeffBin / X;
		Nuvar[ilDeb] = VarBin;
		A[ilDeb+1]     = CoeffCont / X;
		Nuvar[ilDeb+1] = VarCont;				
		B[Cnt] /= X;
		if ( CoeffCont > 0.0 ) {
		  CntDeBorneSupVariable[VarCont] = Cnt;
			if ( -A[ilDeb] >= 0.0 ) X = B[Cnt] - A[ilDeb];
			else X = B[Cnt];
		  if ( TypeDeBorne[VarCont] == VARIABLE_BORNEE_INFERIEUREMENT ) {
			  Umax[VarCont] = X;
				TypeDeBorne[VarCont] = VARIABLE_BORNEE_DES_DEUX_COTES;				
			}
			else if ( TypeDeBorne[VarCont] == VARIABLE_BORNEE_DES_DEUX_COTES ) { 
			  if ( Umax[VarCont] > X ) Umax[VarCont] = X;				
			}
		}  
		else {
		  CntDeBorneInfVariable[VarCont] = Cnt;
			if ( A[ilDeb] >= 0.0 ) X = -B[Cnt];
      else X = -B[Cnt] + A[ilDeb];
			/* Attention a la translation des bornes si on arrive sur Umin superieur a 0 donc on met 0.0 */
			/* Il faut le faire avant de translater les bornes */
			/*
      if ( TypeDeBorne[VarCont] == VARIABLE_NON_BORNEE ) { 
			  Umin[VarCont] = X;
				TypeDeBorne[VarCont] = VARIABLE_BORNEE_INFERIEUREMENT;								
      }
      else if ( TypeDeBorne[VarCont] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			  Umin[VarCont] = X;
				TypeDeBorne[VarCont] = VARIABLE_BORNEE_DES_DEUX_COTES;						
      }
			else {
			  if ( Umin[VarCont] < X ) Umin[VarCont] = X;
			}
			*/
		}				
	}
}

if ( YaDesBornesVariables == NON_PNE ) {
  /*printf(" ATTENTION PAS DE BORNES VARIABLES\n");*/
  free( Pne->CntDeBorneSupVariable );
  free( Pne->CntDeBorneInfVariable );
	Pne->CntDeBorneSupVariable = NULL;
	Pne->CntDeBorneInfVariable = NULL;
}

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( YaDesBornesVariables == OUI_PNE ) printf("Variable bounds detected in native constraints\n");
}

return;
}
