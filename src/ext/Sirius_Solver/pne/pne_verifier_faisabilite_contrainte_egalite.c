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

   FONCTION: Contraintes d'egalite avec variables entiere. On verifie
	           qu'il peut y avoir une solution.
	           On divise tout par le pgcd du membre de gauche. Si la
						 division du nombre a droite n'est pas entiere alors il n'y a
						 pas de solution.
                
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

/*# define AAA*/

/*----------------------------------------------------------------------------*/

void PNE_DetectionContraintesEntieresInfaisable( PROBLEME_PNE * Pne )
{

#ifdef AAA

int Cnt; int NombreDeContraintes; int Var; double Co; int NbBool; char PrendreEnCompte;
int * Mdeb; int * NbTerm; int * Nuvar; int * TypeDeBorne; double * A; double * B;
double * Coeff; double * Xmin; double * Xmax; double BCnt; char * SensContrainte;
int * TypeDeVariable; int Kref; int i; int a; int b; int c; double Min;
char TousLesCoeffSontEntiers; int il; int ilMax; double * X;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;

TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
X = Pne->UTrav;

if ( Pne->Coefficient_CG == NULL ) {
  Pne->Coefficient_CG         = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
  Pne->IndiceDeLaVariable_CG  = (int *)    malloc( Pne->NombreDeVariablesTrav * sizeof( int    ) );
  if ( Pne->Coefficient_CG == NULL || Pne->IndiceDeLaVariable_CG == NULL ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_DetectionContraintesEntieresInfaisable \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }	
}
Coeff = Pne->Coefficient_CG;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {	   
  if ( SensContrainte[Cnt] != '=' ) continue;
	PrendreEnCompte = OUI_PNE;
	NbBool = 0;
	BCnt = B[Cnt];
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Nuvar[il];
		if ( TypeDeVariable[Var] != ENTIER ) { PrendreEnCompte = NON_PNE; break; }
	  Co = A[il];
	  if ( Co == 0.0 ) goto NextElement;
		if ( TypeDeBorne[Var] == VARIABLE_FIXE ) b -= Co * X[Var];
		else if ( Xmin[Var] == Xmax[Var] ) b -= Co * Xmin[Var];
    else {
		  Coeff[NbBool] = Co;
		  NbBool++;
		}		
		NextElement:
		il++;
	}
	if ( PrendreEnCompte == NON_PNE ) continue;
	if ( NbBool < 2 ) continue;
	
	/* Si les coefficients ne sont pas entiers on essaie de les rendre entiers en deplacant
	   la virgule */
		 
  Min = ldexp( 2, 4 ); /*2^4=16 2^5=32 etc..*/

  /* Si les coefficients sont entiers au passe a l'etape du PGCD */
  TousLesCoeffSontEntiers = OUI_PNE;
  for ( i = 0 ; i < NbBool && TousLesCoeffSontEntiers == OUI_PNE ; i++ ) {
    if ( Coeff[i] - floor( Coeff[i] ) != 0.0 ) TousLesCoeffSontEntiers = NON_PNE;
  }
  if ( TousLesCoeffSontEntiers == NON_PNE ) {
    Kref = 3; /* Pour esperer se deplacer d'au moins un chiffre apres la virgule */
    for ( i = 0 ; i < NbBool ; i++ ) {
      while ( ldexp( Coeff[i], Kref ) < Min ) Kref++;
    }	
    for ( i = 0 ; i < NbBool ; i++ ) {
	    Coeff[i] = floor( ldexp( Coeff[i], Kref ) );
    }		
  }
		 
	/* On cherche le PGCD du membre de gauche */
  if ( Coeff[0] > Coeff[1] ) { a = (int) Coeff[0]; b = (int) Coeff[1]; }
  else                       { a = (int) Coeff[1]; b = (int) Coeff[0]; }

  i = 1;

  GCD:

	printf("a %d  b %d\n",a,b);
	
  if ( a == 0 && b == 0 ) c = 0;
  else if ( b == 0 ) c = a;
  else if ( a == 0 ) c = b;
  else {
    c = a % b;
    while ( c != 0 ) { 
      a = b;
      b = c;
      c = a % b;
    }
    c = b;
  }

  i++;
  if ( i < NbBool ) {
    if ( (int) Coeff[i] > c ) { a = (int) Coeff[i]; b = c; }
    else                      { a = c; b = (int) Coeff[i]; }
	  goto GCD;
  }
	
	/* c est le pgcd */
	printf("pgcd = %d  Cnt = %d\n",c,Cnt);
	
  if ( c != 0 ) {
    /* On divise le second membre par le pgcd et si le resultat n'est pas entier
	     on est certain que la contrainte ne peut pas etre satisfaite */
	  BCnt /= c;

		printf("BCnt %e\n",BCnt);
		
    if ( BCnt - floor( BCnt ) > 1.e-4 && ceil( BCnt ) - BCnt > 1.e-4) {
		  printf("La contrainte %d n'est jamais satisfaite\n",Cnt);
			exit(0);
    }		
  }
	exit(0);
 	
}
# endif
  
return;
}

 
