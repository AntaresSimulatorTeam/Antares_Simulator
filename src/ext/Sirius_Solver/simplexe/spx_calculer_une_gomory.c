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

   FONCTION: Calcul d'une coupe de Gomory sur le probleme en cours.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_define.h"
# include "pne_fonctions.h"

# include "lu_define.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

/*----------------------------------------------------------------------------*/
/*                       Retourne 0 si ca s'est mal passe                     */

int SPX_PreparerLeCalculDesGomory( PROBLEME_SPX * Spx ,
                                   int NombreDeVariables , int * TypeDeVariable )							        
{
int VarSpx; int Cnt; int VarE; double S; int il; int ilMax; char Save;
char SecondMembreCreux; char TypeDEntree; char TypeDeSortie; char CalculEnHyperCreux; 
DONNEES_POUR_COUPES_DE_GOMORY * DonneesPourCoupesDeGomory; 

Spx->DonneesPourCoupesDeGomory = (DONNEES_POUR_COUPES_DE_GOMORY *) malloc( sizeof( DONNEES_POUR_COUPES_DE_GOMORY ) );
if ( Spx->DonneesPourCoupesDeGomory == NULL) return( 0 );

DonneesPourCoupesDeGomory = Spx->DonneesPourCoupesDeGomory;

DonneesPourCoupesDeGomory->T                       = (char *)   malloc( Spx->NombreDeVariables   * sizeof( char   ) );
DonneesPourCoupesDeGomory->Coeff                   = (double *) malloc( Spx->NombreDeVariables   * sizeof( double ) );      
DonneesPourCoupesDeGomory->B                       = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );      
DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere = (char *)   malloc( Spx->NombreDeVariables   * sizeof( char   ) );            
DonneesPourCoupesDeGomory->XmaxSv                  = (double *) malloc( Spx->NombreDeVariables   * sizeof( double ) );  
if ( DonneesPourCoupesDeGomory->T      == NULL || DonneesPourCoupesDeGomory->Coeff                   == NULL ||
     DonneesPourCoupesDeGomory->B      == NULL || DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere == NULL ||
     DonneesPourCoupesDeGomory->XmaxSv == NULL ) { 
  free( DonneesPourCoupesDeGomory->T ); free( DonneesPourCoupesDeGomory->Coeff ); free( DonneesPourCoupesDeGomory->B );
  free( DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere ); free( DonneesPourCoupesDeGomory->XmaxSv );
  return( 0 );
}

/* Recuperation des resultats du simplexe */
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  Spx->X                           [VarSpx] = Spx->XSV[VarSpx];	    
  Spx->PositionDeLaVariable        [VarSpx] = Spx->PositionDeLaVariableSV[VarSpx]; 
  Spx->ContrainteDeLaVariableEnBase[VarSpx] = Spx->ContrainteDeLaVariableEnBaseSV[VarSpx];   

  DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere[VarSpx] = NON_SPX;              
  VarE = Spx->CorrespondanceVarSimplexeVarEntree[VarSpx];
  if ( VarE >= 0 && VarE < NombreDeVariables ) {
    if ( TypeDeVariable[VarE] == ENTIER ) DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere[VarSpx] = OUI_SPX;
  }
 
  DonneesPourCoupesDeGomory->XmaxSv[VarSpx] = Spx->Xmax[VarSpx];

  /* Le calcul de BBarre fait intervenir Xmax, il faut dont le corriger pour simuler l'absence d'instanciation */ 
  if ( DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere[VarSpx] == NON_SPX ) continue; 
  /* C'est une variable entiere */
  /* La valeur 1. indique bien que ca ne marche que pour des variables binaires */
  Spx->Xmax[VarSpx] = 1. / Spx->ScaleX[VarSpx];	
  if ( Spx->XminEntree[VarSpx] == 1. ) {	
    /* C'est qu'on a instancie la variable a 1 donc comme */
    Spx->X[VarSpx] = Spx->Xmax[VarSpx];
  }
	    
} 

SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );

/*Spx->NombreDeChangementsDeBase = 0;*/

/* Calcul de B^-1 b */

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  Spx->BBarre[Cnt] = Spx->BAvantTranslationEtApresScaling[Cnt];
  il    = Spx->Mdeb[Cnt];
  ilMax = il + Spx->NbTerm[Cnt];
  S     = 0.;
  while ( il < ilMax ) {
    VarSpx = Spx->Indcol[il];
    if ( Spx->TypeDeVariable[VarSpx] != NON_BORNEE ) { 
      /* Pour les variable entieres on simule l'absence d'instanciation */
      if ( DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere[VarSpx] == NON_SPX ) {			
        S+= Spx->A[il] * Spx->XminEntree[VarSpx] / Spx->ScaleX[VarSpx]; 
      }
    }
    il++;
  }
  Spx->BBarre                 [Cnt]-=S;
  DonneesPourCoupesDeGomory->B[Cnt] = Spx->BBarre[Cnt];
} 

TypeDEntree  = VECTEUR_LU;
TypeDeSortie = VECTEUR_LU;
CalculEnHyperCreux = NON_SPX;
Save = NON_LU;
SecondMembreCreux = NON_LU;

SPX_ResoudreBYegalA( Spx, TypeDEntree, Spx->BBarre, NULL, NULL, &TypeDeSortie,													
                     CalculEnHyperCreux, Save, SecondMembreCreux );

/*printf("pas d'appel a SPX_InitCoupesDIntersection \n");*/

SPX_InitCoupesDIntersection( Spx , DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere ,
                             DonneesPourCoupesDeGomory->B );

 
return( 1 );
}

/*----------------------------------------------------------------------------*/

void SPX_TerminerLeCalculDesGomory( PROBLEME_SPX * Spx )
{
int VarSpx;
DONNEES_POUR_COUPES_DE_GOMORY * DonneesPourCoupesDeGomory;

DonneesPourCoupesDeGomory = Spx->DonneesPourCoupesDeGomory;

if ( DonneesPourCoupesDeGomory->XmaxSv != NULL ) {
  for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
    Spx->Xmax[VarSpx] = DonneesPourCoupesDeGomory->XmaxSv[VarSpx];
  }
}

free( DonneesPourCoupesDeGomory->T );
free( DonneesPourCoupesDeGomory->Coeff );
free( DonneesPourCoupesDeGomory->B );
free( DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere );
free( DonneesPourCoupesDeGomory->XmaxSv );
free( DonneesPourCoupesDeGomory );

/* Reactiver potentiellement l'hyper creux pour le strong branching */
SPX_InitialiserLesIndicateursHyperCreux( Spx );

return;   
}
 
/*----------------------------------------------------------------------------*/
/*  
     En argument, le sous-programme recoit le numero de la variable sur 
     laquelle il faut calculer la coupe. 
*/  

void SPX_CalculerUneCoupeDeGomory( 
       PROBLEME_SPX * Spx,
       int     VariableFractionnaire, /* Numero de la variable sur laquelle il 
                                         faut calculer la coupe */
			    
       double RapportMaxDesCoeffs,
			 double ZeroPourCoeffVariablesDEcart,
			 double ZeroPourCoeffVariablesNatives,
			 double RelaxRhsAbs,
			 double RelaxRhsRel,			  
			 
       /* En retour, la coupe */
       int   * NombreDeTermes       , /* Nombre de coefficients non nuls dans la coupe */
       double * Coefficient          , /* Coefficients de la contrainte */
       int   * IndiceDeLaVariable   , /* Indices des variables qui interviennent dans la coupe */
       double * SecondMembre,         /* Remarque: la coupe est toujours dans le sens <= SecondMembre */
       char * OnAEcrete )
{
int VarSpx ; double NBarreR; double AlphaI0; int Cnt; int il ; int ilMax; double NormeL1; 
int VariableFractionnaireSpx; double Scale; double S; double ScaleXFoisSupDesXmax;
DONNEES_POUR_COUPES_DE_GOMORY * DonneesPourCoupesDeGomory; double ValeurDuZero; 
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;
char * T; double * Coeff; double * B; char * LaVariableSpxEstEntiere; int * VariableEnBaseDeLaContrainte;
double * ErBMoinsUn; int * Cdeb; double * ACol; int * CNbTerm; int * NumeroDeContrainte;
char * OrigineDeLaVariable; char * PositionDeLaVariable; double * ScaleX; double * NBarreRVecteur;
double * XminEntree; double * XmaxEntree; double * Xmax; double * Bs; int NbColonnesDeTest; int NbFois;
int * IndexTermesNonNulsDeErBMoinsUn; int * NumVarNBarreRNonNul; char ControlerAdmissibiliteDuale;

*OnAEcrete = NON_SPX;
*NombreDeTermes = 0;

ValeurDuZero = ZERO_TERMES_DU_TABLEAU_POUR_GOMORY;

DonneesPourCoupesDeGomory = Spx->DonneesPourCoupesDeGomory;
T                         = DonneesPourCoupesDeGomory->T;
Coeff                     = DonneesPourCoupesDeGomory->Coeff;
B                         = DonneesPourCoupesDeGomory->B;
LaVariableSpxEstEntiere   = DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere;

/* Le numero fourni ne peut pas etre dans la numerotation interne au simplexe car l'appelant n'y a
   pas acces */
VariableFractionnaireSpx = Spx->CorrespondanceVarEntreeVarSimplexe[VariableFractionnaire];

Scale   = Spx->ScaleX[VariableFractionnaireSpx];
AlphaI0 = Spx->BBarre[Spx->ContrainteDeLaVariableEnBase[VariableFractionnaireSpx]] * Scale;

/* Calcul de la ligne de B^{-1} qui correspond a la variable de base fractionnaire. On utilise pour cela
   le module de l'algorithme dual */
/* Il est preferable de ne pas faire le calcul des gomory en hyper creux. De toutes façons une gomory est
   rarement hyper creuse. */
Spx->CalculErBMoinsUnEnHyperCreux       = NON_SPX;
Spx->CalculErBMoinsEnHyperCreuxPossible = NON_SPX;
Spx->CalculABarreSEnHyperCreux         = NON_SPX;
Spx->CalculABarreSEnHyperCreuxPossible = NON_SPX;	

Spx->VariableSortante = VariableFractionnaireSpx; /* Info utilisee dans le calcul de la ligne de B^{-1} */

/* Attention, DualCalculerNBarreR calcule ErBmoins1 */
SPX_DualCalculerNBarreR( Spx, NON_SPX, &ControlerAdmissibiliteDuale ); /* En sortie on recupere la ligne dans le vecteur Spx->NBarreR, les 
                                                                          emplacements utiles etant ceux qui correspondent aux variables hors base */
																						 
/* Verification */

VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
ErBMoinsUn                   = Spx->ErBMoinsUn;

ACol               = Spx->ACol; 
Cdeb               = Spx->Cdeb;
CNbTerm            = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;

/* Si le stockage de ErBMoinsUn est COMPACT_SPX on en fait un VECTEUR_SPX */
if ( Spx->TypeDeStockageDeErBMoinsUn == COMPACT_SPX ) {
  Bs = Spx->Bs;
	memset( (char *) Bs, 0, Spx->NombreDeContraintes * sizeof( double ) );
	IndexTermesNonNulsDeErBMoinsUn = Spx->IndexTermesNonNulsDeErBMoinsUn;
	for ( il = 0 ; il < Spx->NbTermesNonNulsDeErBMoinsUn ; il++ ) Bs[IndexTermesNonNulsDeErBMoinsUn[il]] = ErBMoinsUn[il];	
	memcpy( (char *) ErBMoinsUn, (char *) Bs, Spx->NombreDeContraintes * sizeof( double ) );	
  Spx->TypeDeStockageDeErBMoinsUn = VECTEUR_SPX;	
}
else if ( Spx->TypeDeStockageDeErBMoinsUn != VECTEUR_SPX ) {
  printf("Calcul des gomory, attention le mode de stockage de ErBMoinsUn est incorrect\n");
}

goto AAA;
NormeL1 = 0.0;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  VarSpx = VariableEnBaseDeLaContrainte[Cnt];	
  /* Verification */
  S = 0.;
  if ( VarSpx == Spx->VariableSortante ) S = -1.;
  il    = Cdeb[VarSpx];
  ilMax = il + CNbTerm[VarSpx];
  while ( il < ilMax ) {
    S+= ACol[il] * ErBMoinsUn[NumeroDeContrainte[il]];
    il++;
  }
  NormeL1+= fabs( S );
}

if ( NormeL1 > SEUIL_DE_VERIFICATION_DE_NBarreR_GOMORY ) {
  #if VERBOSE_SPX
    printf("Erreur de resolution sur ErBMoinsUn: %e, Gomory refusee\n",fabs( NormeL1 )); 
  #endif	
  Spx->FactoriserLaBase = NON_SPX;	
  return;			      
}
goto BBB;
AAA:
/* Calcul moins consommateur. De plus un test sur la norme L1 peut amener a supprimer trop de coupes */
/* Test sur la colonne de la variable sortante */
NormeL1 = 0.0;
VarSpx = Spx->VariableSortante;
S = -1.;
il    = Cdeb[VarSpx];
ilMax = il + CNbTerm[VarSpx];
while ( il < ilMax ) {
  S+= ACol[il] * ErBMoinsUn[NumeroDeContrainte[il]];
  il++;
}
NormeL1 += fabs( S );
if ( NormeL1 > SEUIL_DE_VERIFICATION_DE_NBarreR_GOMORY ) {
  #if VERBOSE_SPX
    printf("Erreur de resolution sur ErBMoinsUn: %e, Gomory refusee\n",fabs( NormeL1 )); 
  #endif	
  Spx->FactoriserLaBase = NON_SPX;	
  return;			      
}
/* Test sur quelques colonnes autres que celle de la variable sortante */
NbColonnesDeTest = (int) ceil( 0.05 * Spx->NombreDeContraintes );
if ( NbColonnesDeTest < 10 ) NbColonnesDeTest = 10;
NbFois = 0;
while ( NbFois < NbColonnesDeTest ) {
break;
  # if UTILISER_PNE_RAND == OUI_SPX
    Spx->A1 = PNE_Rand( Spx->A1 );
    S = Spx->A1 * (Spx->NombreDeContraintes - 1);		
  # else		
    S = rand() * Spx->UnSurRAND_MAX * (Spx->NombreDeContraintes - 1);		
  # endif	
  Cnt = (int) S;	
  if ( Cnt >= Spx->NombreDeContraintes ) Cnt = Spx->NombreDeContraintes - 1; 
  NormeL1 = 0.0;
  VarSpx = VariableEnBaseDeLaContrainte[Cnt];	
  /* Verification */
  S = 0.;
  if ( VarSpx == Spx->VariableSortante ) S = -1.;
  il    = Cdeb[VarSpx];
  ilMax = il + CNbTerm[VarSpx];
  while ( il < ilMax ) {
    S+= ACol[il] * ErBMoinsUn[NumeroDeContrainte[il]];
    il++;
  }
  NormeL1 += fabs( S );
  if ( NormeL1 > SEUIL_DE_VERIFICATION_DE_NBarreR_GOMORY ) {
    #if VERBOSE_SPX
      printf("Erreur de resolution sur ErBMoinsUn: %e, Gomory refusee\n",fabs( NormeL1 )); 
    #endif	
    Spx->FactoriserLaBase = NON_SPX;	
    return;			      
  }
	NbFois++;
}
BBB:

/* Constitution de la contrainte avant application des regles d'arrondi */
/* On ne veut pas de variables artificielles dans les coupes. Comme elles valent 0 on peut s'en passer */
OrigineDeLaVariable  = Spx->OrigineDeLaVariable;
PositionDeLaVariable = Spx->PositionDeLaVariable;
ScaleX         = Spx->ScaleX;
NBarreRVecteur = Spx->NBarreR;
XminEntree     = Spx->XminEntree;
XmaxEntree     = Spx->XmaxEntree;
Xmax           = Spx->Xmax;

/* Si le stockage de NBarreR est ADRESSAGE_INDIRECT_SPX on en fait un VECTEUR_SPX */
if ( Spx->TypeDeStockageDeNBarreR == ADRESSAGE_INDIRECT_SPX ) {	
	memset( (char *) T, 0, Spx->NombreDeVariables * sizeof( char ) );
	NumVarNBarreRNonNul = Spx->NumVarNBarreRNonNul;
  for ( il = 0 ; il < Spx->NombreDeValeursNonNullesDeNBarreR ; il++ ) T[NumVarNBarreRNonNul[il]] = 1;
  for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
	  if ( T[VarSpx] == 0 ) NBarreRVecteur[VarSpx] = 0.0;
		else T[VarSpx] = 0;
  }			
  Spx->TypeDeStockageDeNBarreR = VECTEUR_SPX;
}
else if ( Spx->TypeDeStockageDeNBarreR != VECTEUR_SPX ) {
  printf("Calcul des gomory, le mode de stockage de NBarreR est incorrect\n");
}

/* Constitution du vecteur sur lequel on fera la MIR */
/* Remarque: les variables non bornees x = x+ - x- avec x+ >= 0 et x- >= 0 sont hors base a 0 
   c'est à dire x+ et x- sont nuls. Comme il y a une difference la variable n'intervient pas. */
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {

  T    [VarSpx] = 0;
  Coeff[VarSpx] = 0.;

  if ( OrigineDeLaVariable[VarSpx] == BASIQUE_ARTIFICIELLE ) continue;
	
  if ( PositionDeLaVariable[VarSpx] != HORS_BASE_SUR_BORNE_INF &&
       PositionDeLaVariable[VarSpx] != HORS_BASE_SUR_BORNE_SUP ) continue;			 

  /* Nettoyage des tres petites valeurs car le vecteur va passer dans les coupes d'intersection.
	   Si on conserve trop de termes, le calcul des coupes d'intersection sera long. */	
  if ( fabs( NBarreRVecteur[VarSpx] ) < ValeurDuZero ) {
	  NBarreRVecteur[VarSpx] = 0.0;	
    continue;
	}
	
  ScaleXFoisSupDesXmax = ScaleX[VarSpx];	
  NBarreR = NBarreRVecteur[VarSpx] * Scale / ScaleXFoisSupDesXmax;	
	
  if ( LaVariableSpxEstEntiere[VarSpx] == OUI_SPX ) {
    /* La variable est entiere. Si elle a ete instanciee a 0, l'information HORS_BASE_SUR_BORNE_INF ou 
       HORS_BASE_SUR_BORNE_SUP ne joue pas. Si elle a ete instanciee a 1 alors c'est comme si elle 
       etait HORS_BASE_SUR_BORNE_SUP */
    if ( XminEntree[VarSpx] == XmaxEntree[VarSpx] ) {			
      /* C'est une variable instanciee */
      if ( XminEntree[VarSpx] == 0. ) goto MajT;			
      AlphaI0-= NBarreR * Xmax[VarSpx] * ScaleXFoisSupDesXmax; 	 
      NBarreR = -NBarreR;   
      goto MajT;
    }      
  }

  if ( PositionDeLaVariable[VarSpx] == HORS_BASE_SUR_BORNE_SUP ) {
    /* On fait le changement de variable X = Xmax - Xtilde => Xtilde est hors base et vaut 0 */
    AlphaI0-= NBarreR * Xmax[VarSpx] * ScaleXFoisSupDesXmax; 	
    NBarreR = -NBarreR;   
  }  
        
  MajT:
  T    [VarSpx] = 1;
  Coeff[VarSpx] = NBarreR;
}

/* On differe le stockage car si la gomory est rejetee plus tard alors on veut aussi rejeter la
   coupe d'intersection */
/* SPX_MatriceCoupesDIntersection( Spx, VariableFractionnaireSpx, T, Coeff, AlphaI0 ); */	 
DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;
DonneesPourCoupesDIntersection->AlphaI0 = AlphaI0;
memcpy( (char *) DonneesPourCoupesDIntersection->TSpx    , (char *) T    , Spx->NombreDeVariables * sizeof( char  ) );
memcpy( (char *) DonneesPourCoupesDIntersection->CoeffSpx, (char *) Coeff, Spx->NombreDeVariables * sizeof( double) );

/* Calcul de la MIR */
SPX_CalculMIRPourCoupeDeGomoryOuIntersection( Spx, RapportMaxDesCoeffs, ZeroPourCoeffVariablesDEcart,
                                              ZeroPourCoeffVariablesNatives, RelaxRhsAbs, RelaxRhsRel,
			                                        AlphaI0, B, T, Coeff, LaVariableSpxEstEntiere,			 
                                              NombreDeTermes, Coefficient, IndiceDeLaVariable, SecondMembre,
                                              OnAEcrete );

return;
}
     

