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

   FONCTION: 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

char PNE_IsInteger( double );

/*----------------------------------------------------------------------------*/

char PNE_IsInteger( double a )
{
char Entier;
Entier = OUI_PNE;
if ( ceil( a ) != floor( a ) ) Entier = NON_PNE;
return( Entier );
}

/*----------------------------------------------------------------------------*/

void PNE_ChangerLesTypesDeVariables( PROBLEME_PNE * Pne )
{
int Cnt; int il; int ilMax; int Var; int * Mdeb; int * NbTerm; int * Nuvar;
int * TypeDeVariable; double * A; double * X; char * SensContrainte;
int * TypeDeBorne; double a; double b; double * SecondMembre;
char ContrainteEntiere; int ic; int * Cdeb; int * Csui; int * NumContrainte;
int Cnt1; int il1; int il1Max; char KnapsackPossible; int Var1;
char TypeDeVariableChange; int NbVarEntieres; int NbVarCont; int VarCont;
double MinPartieEntiere; double MaxPartieEntiere; double * Xmin; double * Xmax;
double aCont; double mn; double mx;

X = Pne->UTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
SensContrainte = Pne->SensContrainteTrav;
SecondMembre = Pne->BTrav;
Mdeb   = Pne->MdebTrav;
NbTerm  = Pne->NbTermTrav;
Nuvar   = Pne->NuvarTrav;
A       = Pne->ATrav;
TypeDeVariable  = Pne->TypeDeVariableTrav;
TypeDeBorne  = Pne->TypeDeBorneTrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  if ( SensContrainte[Cnt] != '=' ) continue;
	NbVarEntieres = 0;
	NbVarCont = 0;
	VarCont = -1;
	aCont = 0.0001;
  ContrainteEntiere = OUI_PNE;
  MinPartieEntiere = 0;
	MaxPartieEntiere = 0;	
  b = SecondMembre[Cnt];	
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];	
  while ( il < ilMax ) {	
    Var = Nuvar[il];
		a = A[il];	
    if ( TypeDeBorne[Var] == VARIABLE_FIXE ) {
		  b -= a * X[Var];
		  goto NextIl_1;
		}
    if ( TypeDeVariable[Var] != ENTIER ) {
		  NbVarCont++;
			if ( NbVarCont > 1 ) {
			  ContrainteEntiere = NON_PNE;
				break;
			}
			VarCont = Var;
			aCont = a;
		}
		else {
		  NbVarEntieres++;
			if ( PNE_IsInteger( a ) != OUI_PNE ) {
			  ContrainteEntiere = NON_PNE;
				break;
			};
			if ( a > 0 ) MaxPartieEntiere += a * Xmax[Var];
			else MinPartieEntiere += a * Xmax[Var];
		}
		NextIl_1:
    il++;
  }
 
	if ( ContrainteEntiere == NON_PNE || VarCont < 0 ) continue;
	if ( aCont != 1 && aCont != -1 ) continue;
	/* Le second membre est entier ? */
 
	if ( PNE_IsInteger( b ) != OUI_PNE ) continue;

	mn = b-MaxPartieEntiere;
	mx = b-MinPartieEntiere;
	if ( aCont < 0 ) {
	  mn = (b-MinPartieEntiere)/aCont;	
		mx = (b-MaxPartieEntiere)/aCont;

	}

	printf("Contrainte entier avec 1 variable continue Cnt = %d  aCont = %e VarCont = %d   %e %e et min = %e max = %e\n",
	Cnt,aCont,VarCont,mn,mx,Xmin[VarCont],Xmax[VarCont]);


	if ( Xmin[VarCont] == 0.0 && Xmax[VarCont] == 1.0 ) {
    TypeDeVariable[VarCont] = ENTIER;
	}
	
	continue;
	
	
  /* On evite de decreter continue une variable entiere qui
	   intervient potentiellement dans une knapsack a coeffs entiers */
	TypeDeVariableChange = NON_PNE;
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];	
  while ( il < ilMax ) {	
    Var = Nuvar[il];
    if ( TypeDeBorne[Var] == VARIABLE_FIXE ) goto NextIl_2;
		/* On va essayer de changer le type de la variable */
		/* Balayage vertical */
		ic = Cdeb[Var];
		while ( ic >= 0 ) {		
      Cnt1 = NumContrainte[ic];
			if ( Cnt1 == Cnt ) goto NextIc;
			KnapsackPossible = NON_PNE;			
      if ( SensContrainte[Cnt1] != '=' ) {
			  /* Knapsack potentielle ? */
				KnapsackPossible = OUI_PNE;
	      il1    = Mdeb[Cnt1];
        il1Max = il1 + NbTerm[Cnt1];	
        while ( il1 < il1Max ) {
          Var1 = Nuvar[il1];
          if ( TypeDeBorne[Var1] == VARIABLE_FIXE ) goto NextElm;					
          if ( TypeDeVariable[Var1] != ENTIER ) {
            KnapsackPossible = NON_PNE;
						break;
					}					
          NextElm:
			    il1++;
			  }				
			}
      if ( KnapsackPossible == NON_PNE ) {
			  /* On peut changer le type */
				printf("On change le type de la variable %d\n",Var);
        TypeDeVariable[Var] = REEL;			
	      TypeDeVariableChange = OUI_PNE;
        break;
			}					
			NextIc:
      ic = Csui[ic];
		}
    if ( TypeDeVariableChange == OUI_PNE ) break;
		NextIl_2:
    il++;		 
  }		
	 			
}

return;
}
 
