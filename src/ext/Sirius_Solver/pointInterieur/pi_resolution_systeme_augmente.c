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

   FONCTION: Resolution du systeme a chaque iteration de point interieur 
       
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# include "lu_constantes_externes.h"
# include "lu_definition_arguments.h"

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif

/*--------------------------------------------------------------------------------*/

void PI_ResolutionSystemeAugmente( PROBLEME_PI * Pi )
{
int CodeRetour; int ItRaff; char FaireDuRaffinement;
int NombreDeVariables; int NombreDeContraintes; int NombreDeColonnes;
int Cnt; int Var; char * TypeDeVariable; double * Qpar2; double * UnSurUkMoinsUmin;
double * S1; double * UnSurUmaxMoinsUk; double * S2; int itDebutRaffinement;
double * SecondMembre; double * DeltaU; double * DeltaLambda;
int * Cdeb; int * CNbTerm; int * NumeroDeContrainte; double * ACol;
int * Mdeb; int * NbTerm; int * Indcol; double * A; double * Alpha;
double * SecondMembreSV; double * Sec; int i; int il; int ilMax;	double Xmx; int Lig;
char FaireDuGradientConjugue; int N; double X;
int * IndexDebutDesColonnes; int * NbTermesDesColonnes;
int * IndicesDeLigne; double * ValeurDesTermesDeLaMatrice;
		
MATRICE_A_FACTORISER * Matrice;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;
NombreDeColonnes    = NombreDeVariables + NombreDeContraintes;
SecondMembre        = Pi->SecondMembre;
DeltaU              = Pi->DeltaU;
DeltaLambda         = Pi->DeltaLambda;
TypeDeVariable      = Pi->TypeDeVariable;
Qpar2               = Pi->Qpar2;
UnSurUkMoinsUmin    = Pi->UnSurUkMoinsUmin;
S1                  = Pi->S1;
UnSurUmaxMoinsUk    = Pi->UnSurUmaxMoinsUk;
S2                  = Pi->S2;

Cdeb               = Pi->Cdeb;
CNbTerm            = Pi->CNbTerm;
ACol               = Pi->ACol;
NumeroDeContrainte = Pi->NumeroDeContrainte;
Mdeb   = Pi->Mdeb;
NbTerm = Pi->NbTerm;
A		   = Pi->A;
Indcol = Pi->Indcol;

Alpha = Pi->Alpha;

FaireDuGradientConjugue = NON_PI;
/* if ( Pi->NumeroDIteration > 50 ) FaireDuGradientConjugue = OUI_PI; */

if ( FaireDuGradientConjugue == OUI_PI ) {
  /* Sauvegarde du second membre qui sera utilise par le gradient conjugue */
  memcpy( (char *) Pi->SecondMembreAffine, (char *) SecondMembre, ( NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ )   DeltaU[Var] = 0.0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) DeltaLambda[Cnt] = 0.0;

Matrice = NULL;
ItRaff = 0;
itDebutRaffinement = ITERATION_DEBUT_RAFINEMENT_ITERATIF;

N = Pi->NumeroDIteration / 2;

if ( N * 2 == Pi->NumeroDIteration ) FaireDuRaffinement = OUI_PI;
else FaireDuRaffinement = NON_PI;

if ( Pi->TypeDIteration == AFFINE ) FaireDuRaffinement = NON_PI;

if ( FaireDuRaffinement == OUI_PI ) {
  if ( Pi->NumeroDIteration >= itDebutRaffinement ) {
    PI_ReconstruireLaMatriceDuSystemeAResoudre( Pi );
	  ItRaff = 1;
    Matrice = (MATRICE_A_FACTORISER *) Pi->Matrice;
    Matrice->ValeurDesTermesDeLaMatrice = Pi->MatricePi->Elm;
    Matrice->IndicesDeLigne		          = Pi->MatricePi->Indl;  
    Matrice->IndexDebutDesColonnes	    = Pi->MatricePi->Ideb;
    Matrice->NbTermesDesColonnes	      = Pi->MatricePi->Nonu;
    Matrice->NombreDeColonnes	          = Pi->MatricePi->NombreDeColonnes;
    SecondMembreSV = (double *) malloc( Matrice->NombreDeColonnes * sizeof( double ) );
    Sec            = (double *) malloc( Matrice->NombreDeColonnes * sizeof( double ) );
		for ( i = 0 ; i < Matrice->NombreDeColonnes ; i++ ) {
		  SecondMembreSV[i] = SecondMembre[i];
		  Sec[i]            = SecondMembre[i];
		}		
  }
}

LU_LuSolv( (MATRICE *) Pi->MatriceFactorisee,
           SecondMembre,   /* Le vecteur du second membre et la solution */
           &CodeRetour,    /* Le code retour ( 0 si tout s'est bien passe ) */
	         Matrice,
	         ItRaff,
	         SEUIL_PARFAIT
         );

/* Verification de la resolution */
if ( FaireDuRaffinement == OUI_PI ) {
  if ( Pi->NumeroDIteration >= itDebutRaffinement ) {
			
	  IndexDebutDesColonnes      = Matrice->IndexDebutDesColonnes;
		NbTermesDesColonnes        = Matrice->NbTermesDesColonnes;
		IndicesDeLigne             = Matrice->IndicesDeLigne;
		ValeurDesTermesDeLaMatrice = Matrice->ValeurDesTermesDeLaMatrice;
		
    for ( i = 0 ; i < NombreDeColonnes ; i++ ) {
      il = IndexDebutDesColonnes[i];
      ilMax = il + NbTermesDesColonnes[i];
			X = SecondMembre[i];
      while ( il < ilMax ) {
        SecondMembreSV[IndicesDeLigne[il]]-= ValeurDesTermesDeLaMatrice[il] * X;
        il++;
      }
    }
  	Xmx = -1;
    for ( i = 0 ; i < NombreDeColonnes ; i++ ) {
		  X = fabs( SecondMembreSV[i] );
	    if ( X > Xmx ) {
		    Xmx = X;
		    #if VERBOSE_PI
		  	  Lig = i;
        #endif		
		  }
	  }
		#if VERBOSE_PI
  	  if ( Lig < NombreDeVariables ) {
	      printf("-----> Plus gros ecart de resolution %e sur la variable %d RegulVar %e U %e S1 %e S2 %e\n",
			          Xmx,Lig,Pi->RegulVar[Lig],Pi->U[Lig],Pi->S1[Lig],Pi->S2[Lig]);
  	  }
	    else {  
	      printf("-----> Plus gros ecart de resolution %e sur la contrainte %d RegulContrainte %e\n",Xmx,
			                 Lig-NombreDeVariables,Pi->RegulContrainte[Lig-NombreDeVariables]);
	    }
	    printf(" Xmx %e Second membre %e\n",Xmx,Sec[Lig]);	 
    #endif		
    Pi->Resolution = BON;
		if ( Xmx > SEUIL_APPROX )  Pi->Resolution = APPROX;
		if ( Xmx < SEUIL_PARFAIT ) Pi->Resolution = PARFAIT;
    		
	  free ( SecondMembreSV );
	  free ( Sec );
	}
}
				 
if ( CodeRetour == PRECISION_DE_RESOLUTION_NON_ATTEINTE ) {
  /* Tant pis on continue quand-meme */
  CodeRetour = 0;     
}
				 
if ( CodeRetour != 0 ) {
  printf(" Erreur dans la resolution du systeme, numero d'erreur %d ",CodeRetour);
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

/*-------------------------------------------------------*/
/* Transfert de la solution vers les tables d'increments */

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) DeltaU[Var]+= SecondMembre[Var];
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) DeltaLambda[Cnt]+= SecondMembre[NombreDeVariables + Cnt];

if ( FaireDuGradientConjugue == OUI_PI ) {
  PI_GradientConjugue( Pi );
}

/*----------------------------------------------------------*/

/* Calcule de delta S1 et de delta S2 */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) DeltaU[Var]*= Alpha[Var];

PI_Sos1s2( Pi );

return;
}
