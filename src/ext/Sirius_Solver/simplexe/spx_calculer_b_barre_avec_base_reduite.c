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

   FONCTION: Calcul de BBarre = B^{-1} * b 

                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"  

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

# define DEBUG NON_SPX

/*----------------------------------------------------------------------------*/

void SPX_CalculerBBarreAvecBaseReduite( PROBLEME_SPX * Spx )
{
int Var; int il; int ilMax; char Save; char SecondMembreCreux;  double * BBarre;
double * B; char * PositionDeLaVariable; int * IndicesDeLigneDesTermesDuProblemeReduit;
double * ValeurDesTermesDesColonnesDuProblemeReduit; int * CdebProblemeReduit;
int * CNbTermProblemeReduit; double * Xmax; double XmaxDeVar; int NombreDeBornesAuxiliairesUtilisees;
char * StatutBorneSupCourante; char TypeDEntree; char TypeDeSortie; char CalculEnHyperCreux;
int r; int RangDeLaMatriceFactorisee; int * LigneDeLaBaseFactorisee;

BBarre = Spx->BBarre;
B = Spx->B;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;
LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;   

CdebProblemeReduit = Spx->CdebProblemeReduit;
CNbTermProblemeReduit = Spx->CNbTermProblemeReduit;
IndicesDeLigneDesTermesDuProblemeReduit = Spx->IndicesDeLigneDesTermesDuProblemeReduit;
ValeurDesTermesDesColonnesDuProblemeReduit = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;

Xmax = Spx->Xmax;

PositionDeLaVariable = Spx->PositionDeLaVariable;

NombreDeBornesAuxiliairesUtilisees = Spx->NombreDeBornesAuxiliairesUtilisees;
StatutBorneSupCourante             = Spx->StatutBorneSupCourante;

for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) { 
  BBarre[r] = B[LigneDeLaBaseFactorisee[r]];
}	

/* On calcule B^{-1} * N * Xmax pour les variables hors base sur borne sup */
/* Ne pas utiliser la liste des variables hors base car elle evolue */
if ( NombreDeBornesAuxiliairesUtilisees == 0 ) {
  /* Seules les variables natives peuvent se trouver sur borne sup. Certes
	   les variables aditionnelles de contraintes d'egalite aussi mais leur
		 borne sup est nulle de toutes facons */
  for ( Var = 0 ; Var < Spx->NombreDeVariablesNatives ; Var++ ) { 
    if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) { 
	  if ( Xmax[Var] == 0.0 ) continue;
	    XmaxDeVar = Xmax[Var];
      il    = CdebProblemeReduit[Var];
      ilMax = il + CNbTermProblemeReduit[Var];
      while ( il < ilMax ) {
		    BBarre[IndicesDeLigneDesTermesDuProblemeReduit[il]] -= ValeurDesTermesDesColonnesDuProblemeReduit[il] * XmaxDeVar;								
        il++;
	    }
	    continue;
	  }
  }  
}
else { 
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) { 
	    if ( Xmax[Var] == 0.0 ) continue;
	    XmaxDeVar = Xmax[Var];
      il    = CdebProblemeReduit[Var];
      ilMax = il + CNbTermProblemeReduit[Var];
      while ( il < ilMax ) {				
		    BBarre[IndicesDeLigneDesTermesDuProblemeReduit[il]] -= ValeurDesTermesDesColonnesDuProblemeReduit[il] * XmaxDeVar;							    
        il++;
      }
	    continue;
	  }
	  else if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
	    /* Prise en compte des bornes inf auxiliaires sur les variables non bornees */
      if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
	      /* Car on simule le fait qu'on a cree une borne inf egale a -Xmax */
	      XmaxDeVar = -Xmax[Var];
        il    = CdebProblemeReduit[Var];
        ilMax = il + CNbTermProblemeReduit[Var];
        while ( il < ilMax ) {					
		      BBarre[IndicesDeLigneDesTermesDuProblemeReduit[il]] -= ValeurDesTermesDesColonnesDuProblemeReduit[il] * XmaxDeVar;							    								      
          il++;
        }
	    }
	  }
  }
}

/* Resoudre BBarre = B^{-1} * b */

TypeDEntree  = VECTEUR_LU;
TypeDeSortie = VECTEUR_LU;
CalculEnHyperCreux = NON_SPX;

Save = NON_LU;
SecondMembreCreux = NON_LU;

SPX_ResolutionDeSysteme( Spx, TypeDEntree, BBarre, NULL, NULL, &TypeDeSortie,
                         CalculEnHyperCreux, Save, SecondMembreCreux );									 

/* Eventuellement forme produit de l'inverse */
if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
  printf("CalculerBBarre AppliquerLesEtaVecteurs pas operationnel \n");
	exit(0);
  SPX_AppliquerLesEtaVecteurs( Spx, BBarre, NULL, NULL, CalculEnHyperCreux, TypeDeSortie );
}

# if VERIFICATION_BBARRE == OUI_SPX
printf("----------- CalculerBBarre Iteration %d ---------------- \n",Spx->Iteration);
{
double * Buff; int i; int Var; int ic; int icMx; double * Sortie; char Arret;
Buff = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
Sortie = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
memcpy( (char *) Buff, ( char *) Spx->B, Spx->NombreDeContraintes * sizeof( double ) );
if ( NombreDeBornesAuxiliairesUtilisees == 0 ) {
  /* Seules les variables natives peuvent se trouver sur borne sup. Certes
	   les variables aditionnelles de contraintes d'egalite aussi mais leur
		 borne sup est nulle de toutes facons */
  for ( Var = 0 ; Var < Spx->NombreDeVariablesNatives ; Var++ ) { 
    if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) { 
	    if ( Xmax[Var] == 0.0 ) continue;
	    XmaxDeVar = Xmax[Var];
      il    = Cdeb[Var];
      ilMax = il + CNbTerm[Var];
      while ( il < ilMax ) {
        Buff[NumeroDeContrainte[il]] -= ACol[il] * XmaxDeVar;
        il++;
      }
		  continue;
	  }
	}  
}
else { 
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) { 
	    if ( Xmax[Var] == 0.0 ) continue;
	    XmaxDeVar = Xmax[Var];
      il    = Cdeb[Var];
      ilMax = il + CNbTerm[Var];
      while ( il < ilMax ) {
        Buff[NumeroDeContrainte[il]] -= ACol[il] * XmaxDeVar;
        il++;
      }
		  continue;
	  }
	  else if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
		  /* Prise en compte des bornes inf auxiliaires sur les variables non bornees */
      if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
	      /* Car on simule le fait qu'on a cree une borne inf egale a -Xmax */
	      XmaxDeVar = -Xmax[Var];
        il    = Cdeb[Var];
        ilMax = il + CNbTerm[Var];
        while ( il < ilMax ) {
          Buff[NumeroDeContrainte[il]] -= ACol[il] * XmaxDeVar;
          il++;
        }
			}
		}
  }
}

for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Sortie[i] = BBarre[i];
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
  Var = Spx->VariableEnBaseDeLaContrainte[i];
	ic = Spx->Cdeb[Var];
	icMx = ic + Spx->CNbTerm[Var];
	while ( ic < icMx ) {
	  Buff[NumeroDeContrainte[ic]] -= ACol[ic] * Sortie[i];
	  ic++;
	}
}
Arret = NON_SPX;
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
	if ( fabs( Buff[i] ) > 1.e-7 ) {
	  printf("i = %d   ecart %e  VariableEnBaseDeLaContrainte %d\n",i,Buff[i],Spx->VariableEnBaseDeLaContrainte[i]);
		Var = Spx->VariableEnBaseDeLaContrainte[i];
		if ( Spx->OrigineDeLaVariable[Var] != NATIVE ) printf(" variable non native\n");
		else printf(" variable native\n");
		Arret = OUI_SPX;
	}
}
if ( Arret == OUI_SPX ) {
 printf("Verif Bbarre  not OK\n");
 exit(0);
}
printf("Fin verif Bbarre  OK\n");
free( Buff );
free( Sortie );

SPX_VerifierLesVecteursDeTravail( Spx );

}
# endif
													 										 
return;
}

