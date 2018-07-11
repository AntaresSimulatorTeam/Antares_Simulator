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

   FONCTION: Creation de la matrice du point interieur.
        
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

/*-------------------------------------------------------------------------*/
/*     Allocation de la matrice, calcul des termes, stockage des termes    */
 
void PI_CrematSystemeAugmente( PROBLEME_PI * Pi )
{
int Var; int Cnt ; int Colonne; int il; int ilMax  ; int ilCourant; double X ;
char Type; double * Alpha; double * Alpha2; int NombreDeVariables; int NombreDeContraintes;
double * RegulVar; double * RegulContrainte; double * Qpar2; double * UnSurUkMoinsUmin;
double * UnSurUmaxMoinsUk; double * S1; double * S2; double * U0; double * Lambda0; double * U;
double * Lambda; char * TypeDeVariable; int NbIter;
int * Ideb; int * Nonu; int * Indl; int * Cdeb;int * CNbTerm; int * NumeroDeContrainte;
int * Mdeb; int * NbTerm; int * Indcol; double * Elm; double * ACol; double * A;
double * TermeDeRegularisation;

MATRICE_A_FACTORISER * Matrice;

Matrice = (MATRICE_A_FACTORISER *) Pi->Matrice;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;

TermeDeRegularisation = (double *) malloc( ( NombreDeVariables + NombreDeContraintes ) * sizeof( double ));
if ( TermeDeRegularisation == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_CrebisSystemeAugmente \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); 
}

RegulVar        = Pi->RegulVar;
RegulContrainte = Pi->RegulContrainte;

TypeDeVariable = Pi->TypeDeVariable;

Qpar2 = Pi->Qpar2;
UnSurUkMoinsUmin = Pi->UnSurUkMoinsUmin;
UnSurUmaxMoinsUk = Pi->UnSurUmaxMoinsUk;
S1 = Pi->S1;
S2 = Pi->S2;

Cdeb    = Pi->Cdeb;
CNbTerm = Pi->CNbTerm;
ACol    = Pi->ACol;
NumeroDeContrainte = Pi->NumeroDeContrainte;

Mdeb   = Pi->Mdeb;
NbTerm = Pi->NbTerm;
Indcol = Pi->Indcol;
A      = Pi->A;

U0 = Pi->U0;
U  = Pi->U;
Lambda0 = Pi->Lambda0;
Lambda = Pi->Lambda;
Alpha  = Pi->Alpha;
Alpha2 = Pi->Alpha2;

/* RAZ des termes de regularisation */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  RegulVar[Var] = 0.0;
	Alpha[Var]    = 1.0;
	Alpha2[Var]   = 1.0;
}
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ){
  RegulContrainte[Cnt] = 0.0;
}
for ( Colonne = 0 ; Colonne < NombreDeVariables + NombreDeContraintes ; Colonne++ ) {
  TermeDeRegularisation[Colonne] = 0.0;
}

NbIter = 0;

DebutCremat:

NbIter++;

PI_LibereMatrice( Pi );
PI_AllocMatrice( Pi );

Ideb = Pi->MatricePi->Ideb;
Nonu = Pi->MatricePi->Nonu;
Elm  = Pi->MatricePi->Elm;
Indl = Pi->MatricePi->Indl;

Pi->MatricePi->NombreDeColonnes = NombreDeVariables + NombreDeContraintes;

ilCourant = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  Colonne = Var;
  Ideb[Colonne] = ilCourant;
  Nonu[Colonne] = 0;
  /* Terme diagonal ( hessien ) */
	Type = TypeDeVariable[Var];
  if ( Type == BORNEE ) {
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] ) + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( Type == BORNEE_INFERIEUREMENT ) {
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] );
  }
  else if ( Type == BORNEE_SUPERIEUREMENT ) {
    X = Qpar2[Var] + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else {	
    X = Qpar2[Var]; 
  }	
  Elm [ilCourant] = ( X + RegulVar[Var] ) * Alpha2[Var];	
  TermeDeRegularisation[Var] = RegulVar[Var] * Alpha2[Var];
  Indl[ilCourant] = Var;
  Nonu[Colonne]++;  
  ilCourant++;  
  /* Partie matrice des contraintes, vision transposee */
  il    = Cdeb[Var];
  ilMax = il + CNbTerm[Var];
  while ( il < ilMax ) {
    Elm [ilCourant] = ACol[il] * Alpha[Var];				
    Indl[ilCourant] = NombreDeVariables + NumeroDeContrainte[il];
    Nonu[Colonne]++;
    ilCourant++;
    il++;
  }    
}

/* Matrice des contraintes */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  Colonne = NombreDeVariables + Cnt;
  Ideb[Colonne] = ilCourant;
  Nonu[Colonne] = 0;
  
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Indcol[il];
    Elm [ilCourant] = A[il] * Alpha[Var];				
    Indl[ilCourant] = Var;
    Nonu[Colonne]++;
    ilCourant++;
    il++;
  }
  /* Terme de regularisation si necessaire */
  Elm [ilCourant] = RegulContrainte[Cnt];
  TermeDeRegularisation[Colonne] = RegulContrainte[Cnt]; 		
  Indl[ilCourant] = Colonne;
  Nonu[Colonne]++;
  ilCourant++;  
}

/* Elimination ordonnee et factorisation */
# if VERBOSE_PI
  printf("-> Factorisation du systeme augmente\n");
# endif

if ( Pi->MatriceFactorisee != NULL ) {
  LU_LibererMemoireLU( (MATRICE *) Pi->MatriceFactorisee );
  Pi->MatriceFactorisee = NULL;
}

Matrice->ContexteDeLaFactorisation  = LU_POINT_INTERIEUR;
/* S'il n'y a pas beaucoup de contraintes on n'utilise pas les super lignes */
if ( Pi->NombreDeContraintes > 1000 )  Matrice->UtiliserLesSuperLignes = OUI_LU;
else Matrice->UtiliserLesSuperLignes = NON_LU;
Matrice->ValeurDesTermesDeLaMatrice  = Elm;
Matrice->IndicesDeLigne		           = Indl;  
Matrice->IndexDebutDesColonnes	     = Ideb;
Matrice->NbTermesDesColonnes	       = Nonu;
Matrice->NombreDeColonnes	           = Pi->MatricePi->NombreDeColonnes;
Matrice->FaireScalingDeLaMatrice     = NON_LU; 
Matrice->UtiliserLesValeursDePivotNulParDefaut = NON_LU;
Matrice->ValeurDuPivotMin                      = VALEUR_DU_PIVOT_MIN;
Matrice->ValeurDuPivotMinExtreme               = VALEUR_DU_PIVOT_MIN_EXTREME;
Matrice->SeuilPivotMarkowitzParDefaut = NON_LU;
Matrice->ValeurDuPivotMarkowitz       = 1.e-3; /* Sans importance si valeur par defaut */ 
Matrice->FaireDuPivotageDiagonal      = OUI_LU;
Matrice->LaMatriceEstSymetrique       = OUI_LU; 
Matrice->LaMatriceEstSymetriqueEnStructure = NON_LU;

Matrice->ValeurDeRegularisation = VALEUR_DE_REGULARISATION;
Matrice->TermeDeRegularisation  = TermeDeRegularisation;

Pi->MatriceFactorisee = LU_Factorisation( Matrice );

if ( Pi->MatriceFactorisee == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_CrematSystemeAugmente \n"); 
  Pi->AnomalieDetectee = PI_ERREUR_INTERNE;
  longjmp( Pi->Env , Pi->AnomalieDetectee );
}
if ( Matrice->ProblemeDeFactorisation != NON_LU ) {
  printf(" Point interieur, factorisation impossible \n"); 
  Pi->AnomalieDetectee = PI_ERREUR_INTERNE;
  longjmp( Pi->Env , Pi->AnomalieDetectee );
}

if ( Matrice->OnARegularise  == OUI_LU ) {
  # if VERBOSE_PI
    printf("-> Regularisation de la matrice du systeme augmente\n");
  # endif
  for ( Colonne = 0 ; Colonne < Pi->MatricePi->NombreDeColonnes ; Colonne++ ) {	
    if ( TermeDeRegularisation[Colonne] == 0.0 ) continue;
    if ( Colonne < NombreDeVariables ) {
      RegulVar[Colonne] = TermeDeRegularisation[Colonne] / Alpha2[Colonne];
    }
    else {
      RegulContrainte[Colonne-NombreDeVariables] = TermeDeRegularisation[Colonne];
    }
  }
  if ( NbIter < 1 ) goto DebutCremat; 
}

free( TermeDeRegularisation );

/* En sortie on recupere le triangle dans lesquels on remet la matrice d'entree
   afin de la mettre a jour au cours des iterations suivantes */
/* Triangle U stocke par Ligne */
/* a revoir */
/*
printf(" Revoir la routine PI_InitMatriceLU\n");
*/
PI_InitMatriceLU( Pi ,
                  Matrice->IndexDebutDesLignesDeU , Matrice->NbTermesDesLignesDeU  ,
		              Matrice->ValeurDesTermesDeU     , Matrice->IndicesDeColonneDeU );
		
return;
}

/*-------------------------------------------------------------------------*/
/*  Calcul de termes de la matrice a chaque iteration puis factorisation   */

void PI_CrebisSystemeAugmente( PROBLEME_PI * Pi )
{
int Var; double X; int il; char Position; char Erreur; int Cnt; int Ordre;
int IndiceLigne; int Colonne; int NombreDeContraintes; int NombreDeColonnes;
double * TermeDeRegularisation; char OnARegularise; int i; int ilMax;
char * TypeDeVariable; double * Qpar2; double * UnSurUkMoinsUmin; double * UnSurUmaxMoinsUk; 
double * S1; double * S2; double * Lambda; int NombreDeVariables; 
int * Ideb; double * Elm; int * IndexDuTermeDiagonalDansU; double * RegulContrainte;
double * RegulVar; double * ValeurDesTermesDeU; int NbIter; double * Alpha; double * Alpha2; 
int Kp; int VarPiv; int * IndexDebutDesLignesDeU; int * NbTermesDesLignesDeU;   
int * IndicesDeColonneDeU; double * ValeurDesTermesDeU_SV; char Type;
 
TermeDeRegularisation = (double *) malloc( Pi->MatricePi->NombreDeColonnes * sizeof( double ));
if ( TermeDeRegularisation == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_CrebisSystemeAugmente \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); 
}

TypeDeVariable   = Pi->TypeDeVariable;
Qpar2            = Pi->Qpar2;
UnSurUkMoinsUmin = Pi->UnSurUkMoinsUmin;
UnSurUmaxMoinsUk = Pi->UnSurUmaxMoinsUk;
S1 = Pi->S1;
S2 = Pi->S2;
Lambda = Pi->Lambda;

Alpha  = Pi->Alpha;
Alpha2 = Pi->Alpha2;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;
NombreDeColonnes    = Pi->MatricePi->NombreDeColonnes;

RegulVar        = Pi->RegulVar;
RegulContrainte = Pi->RegulContrainte;

Elm  = Pi->MatricePi->Elm;
Ideb = Pi->MatricePi->Ideb;
IndexDuTermeDiagonalDansU = Pi->MatricePi->IndexDuTermeDiagonalDansU;
ValeurDesTermesDeU        = Pi->MatricePi->ValeurDesTermesDeU;
			
NbIter = 0;

/* Modification du terme diagonal de MatricePi */

Refactorisation:

# if VERBOSE_PI
  printf("-> Refactorisation du systeme augmente\n");
# endif

NbIter++;
for ( i = 0 ; i < NombreDeColonnes ; i++ ) TermeDeRegularisation[i] = 0.0;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ )   RegulVar[Var] = 0.0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) RegulContrainte[Cnt] = 0.0;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
	Type = TypeDeVariable[Var];
  if ( Type == BORNEE ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] ) + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( Type == BORNEE_INFERIEUREMENT ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] );
  }
  else if ( Type == BORNEE_SUPERIEUREMENT ) {			
    X = Qpar2[Var] + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else { 
    X = Qpar2[Var];				
  }
	
	X*= Alpha2[Var];
  TermeDeRegularisation[Var] = RegulVar[Var] * Alpha2[Var];
  ValeurDesTermesDeU[IndexDuTermeDiagonalDansU[Var]] = X + TermeDeRegularisation[Var];
	
}

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  Var = NombreDeVariables + Cnt;	
  X = RegulContrainte[Cnt];		
  TermeDeRegularisation[Var] = X; 	
  ValeurDesTermesDeU[IndexDuTermeDiagonalDansU[Var]] = X;	
}

/* On met a jour les termes extra diagonaux de U en fonction du changement de variables */
IndexDebutDesLignesDeU = Pi->MatricePi->IndexDebutDesLignesDeU;
NbTermesDesLignesDeU   = Pi->MatricePi->NbTermesDesLignesDeU;
IndicesDeColonneDeU    = Pi->MatricePi->IndicesDeColonneDeU;
ValeurDesTermesDeU_SV  = Pi->MatricePi->ValeurDesTermesDeU_SV;

Kp = 0;
ilMax = 0;
while ( Kp < NombreDeColonnes ) {
  il    = ilMax;
	ilMax+= NbTermesDesLignesDeU[Kp];
	VarPiv = IndicesDeColonneDeU[il];
	il++; /* On saute le terme diagonal: a verifier que c'est aussi le cas dans cette vue i.e. terme diagonal en premier */
  while ( il < ilMax ) {
	  Colonne = IndicesDeColonneDeU[il];
		if ( Colonne < NombreDeVariables ) Var = Colonne;	 
		else Var = VarPiv;
	  /* Il faut que Var et VarPiv soient dans les variables (pas dans les contraintes) */
		if ( Var < NombreDeVariables ) {
      ValeurDesTermesDeU[il] = ValeurDesTermesDeU_SV[il] * Alpha[Var];
		}
    il++;
	}
	Kp++;
}

LU_Refactorisation( (MATRICE *) Pi->MatriceFactorisee,
		                ValeurDesTermesDeU, Pi->MatricePi->NbElementsDeU,
		                TermeDeRegularisation            , VALEUR_DE_REGULARISATION    , 
		                &OnARegularise                   , &Erreur );
										
if ( Erreur == OUI_LU ) {
  free( TermeDeRegularisation );
  /* On tente une factorisation complete car peut-être que les permutations ne sont
     plus bonnes */ 
  PI_CrematSystemeAugmente( Pi );
	OnARegularise = NON_LU;
} 

if ( OnARegularise == OUI_LU ) {
  for ( i = 0 ; i < NombreDeColonnes ; i++ ) {	
    if ( TermeDeRegularisation[i] == 0.0 ) continue;
    if ( i < NombreDeVariables ) {
      RegulVar[i] = TermeDeRegularisation[i] / Alpha2[i];
			/*printf("Regularisation sur la variable %d RegulVar %e\n",i,RegulVar[i]);*/
    }
    else {
      RegulContrainte[i-NombreDeVariables] = TermeDeRegularisation[i];
			/*printf("Regularisation sur la contrainte %d\n",i-NombreDeVariables);*/
    }
  }
	
  # if VERBOSE_PI
    printf("-> Regularisation de la matrice du systeme augmente\n");
  # endif
	if ( NbIter < 1 ) goto Refactorisation; 
}  

free( TermeDeRegularisation );

return;
}


