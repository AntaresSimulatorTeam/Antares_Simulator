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

   FONCTION: 
   Calcul du second membre incluant le gradient et le test de stationnarite.
   
                
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

/*-----------------------------------------------------------------------------------*/

void PI_Calstaf( PROBLEME_PI * Pi )
{
int i; int j; int il; int ilMax; double X; double Y; int iq; double Critere;
double Ecsta; int Icsta; double Eccnt; int Iccnt; int Var; double * Alpha;
double * RegulVar; double * U0; double * RegulContrainte; double * Lambda0;
double * Qpar2; double * L; double *S1 ; double * S2; int * Mdeb; int * NbTerm; int * Indcol;
double * A; double * U; double * SecondMembre; double * Lambda; double * B; 
double SeuilDAdmissibilite; int NombreDeVariables;int NombreDeContraintes;

Eccnt = 0.; Iccnt = -1; /* Raz */
Ecsta = 0.; Icsta = -1; /* Raz */

Mdeb   = Pi->Mdeb;
NbTerm = Pi->NbTerm;
Indcol = Pi->Indcol;
A      = Pi->A;
B      = Pi->B;

U = Pi->U;
SecondMembre = Pi->SecondMembre;
Lambda       = Pi->Lambda;

Alpha = Pi->Alpha;

SeuilDAdmissibilite = Pi->SeuilDAdmissibilite;
NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;

RegulVar = Pi->RegulVar;
U0       = Pi->U0;

RegulContrainte = Pi->RegulContrainte;
Lambda0         = Pi->Lambda0;

for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  X = 0.;
  il = Mdeb[i];
  ilMax = il + NbTerm[i];
  while ( il < ilMax ) {
    /* Partie contrainte */
    Var = Indcol[il];    
    X+= A[il] * U[Var];  
        
    /* Partie stationnarite */    
    SecondMembre[Var]+= A[il] * Alpha[Var] * Lambda[i];
		    
    il++;
  }
  
  SecondMembre[ NombreDeVariables + i ] = B[i] - X;
  
  X = SecondMembre[ NombreDeVariables + i ];
	/* On tient compte des termes de freinage dans la condition d'admissibilite */
  /*X-= RegulContrainte[i] * ( Lambda[i] - Lambda0[i] );*/
	
  X = fabs( X );
  if ( X > SeuilDAdmissibilite ) {
    if ( X > Eccnt ) { Eccnt = X; Iccnt = i; }
  }

}

/*printf("Attention on ajoute RegulContrainte[i] * ( Lambda[i] - Lambda0[i] dans le controle du second membre contraintes\n");*/

Pi->MaxContrainte[Pi->NumeroDIteration-1] = Eccnt;
if ( Eccnt > Pi->SeuilDAdmissibilite ) Pi->ArretCnt = NON_PI;

#if VERBOSE_PI 
  printf("Admissibilite: seuil %e ecart max %e ", Pi->SeuilDAdmissibilite, Eccnt);
  if ( Iccnt >= 0 ) {
    printf("contrainte %d regul %e", Iccnt,Pi->RegulContrainte[Iccnt]); 
  }
  printf("\n");
#endif

/* Test sur la stationnarite */

Qpar2 = Pi->Qpar2;
L     = Pi->L;
S1    = Pi->S1;
S2    = Pi->S2;
for ( i = 0 ; i < NombreDeVariables ; i++ ) {

  X = ( Qpar2[i] * U[i] ) + L[i];
	
  X*= Alpha[i];
	  
  SecondMembre[i]+= X;

  Critere = ( ( S1[i] - S2[i] ) * Alpha[i] ) - SecondMembre[i];
	/* On tient compte des termes de freinage dans la condition de stationnarite */
  Critere-= RegulVar[i] * ( U[i] - U0[i] ) * Alpha[i];
	
  SecondMembre[i] = -SecondMembre[i];

  Y = fabs( Critere );
	/* Il faudrait tenir compte de Alpha dans la valeur du seuil mais si Alpha devient tres petit le seuil
	   peut devenir vraiment trop petit et on ne l'atteint jamais */
  if ( Y > Pi->SeuilDeStationnarite ) {
    if ( Y > Ecsta ) { Ecsta = Y; Icsta = i; }   
  }
}

Pi->MaxStationnarite[Pi->NumeroDIteration-1] = Ecsta;
if ( Icsta >= 0 ) Pi->ArretSta = NON_PI;

#if VERBOSE_PI                                 
  printf("Stationnarite: seuil %e ecart max %e ", Pi->SeuilDeStationnarite, Ecsta);
  if ( Icsta >= 0 ) {
    printf("variable   %d regul %e valeur var. %e", Icsta, Pi->RegulVar[Icsta], Pi->U[Icsta]);
    if ( Pi->TypeDeVariable[Icsta] == BORNEE ) printf("  BORNEE ");
    else if ( Pi->TypeDeVariable[Icsta] == BORNEE_INFERIEUREMENT ) printf(" BORNEE_INFERIEUREMENT ");
    else if ( Pi->TypeDeVariable[Icsta] == BORNEE_SUPERIEUREMENT ) printf(" BORNEE_SUPERIEUREMENT "); 
    else if ( Pi->TypeDeVariable[Icsta] == NON_BORNEE ) printf(" NON_BORNEE ");
		printf(" S1 %e S2 %e", Pi->S1[Icsta],Pi->S2[Icsta]);
    if ( Pi->TypeDeVariable[Icsta] == BORNEE ) {
		}		
  }
  printf("\n");	
#endif

/* Sauvegarde */
memcpy( (char *) Pi->SecondMembreAffine, (char *) SecondMembre, ( NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );

return;
}

