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

   FONCTION: Recuperation d'une ligne du tableau pour les 2 step MIR cuts
                
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

void SPX_GetTableauRow( 
       PROBLEME_SPX * Spx,
       int     VariableFractionnaire, /* Numero de la variable pour laquelle on veut la ligne */
       double  ValeurDuZero         , /* Utilise pour detecter les coefficients nuls de la coupe */
       /* En retour, la ligne */
       int   * NombreDeTermes       , 
       double * Coefficient          , 
       int   * IndiceDeLaVariable   , 
       double * SecondMembre,         
       char * CodeRet )
{
int   VarSpx ; double NBarreR; double AlphaI0 ; int   Cnt   ; int   il ; int   ilMax; double NormeL1; 
int   VariableFractionnaireSpx; double Scale ; double S; double ScaleXFoisSupDesXmax; int NbTermes;    
char * T;  char * LaVariableSpxEstEntiere; int * VariableEnBaseDeLaContrainte; double * ErBMoinsUn;
int * Cdeb; double * ACol; int * CNbTerm; int * NumeroDeContrainte; char * OrigineDeLaVariable;
char * PositionDeLaVariable; double * ScaleX; double * NBarreRVecteur; double * XminEntree;
double * XmaxEntree; double * Xmax; double * Bs;
int * IndexTermesNonNulsDeErBMoinsUn; int *NumVarNBarreRNonNul; char ControlerAdmissibiliteDuale;
DONNEES_POUR_COUPES_DE_GOMORY * DonneesPourCoupesDeGomory;

*CodeRet = OUI_SPX;

ValeurDuZero = ZERO_TERMES_DU_TABLEAU_POUR_GOMORY;

DonneesPourCoupesDeGomory = Spx->DonneesPourCoupesDeGomory;
LaVariableSpxEstEntiere = DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere;

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
  *CodeRet = NON_SPX;
  return;		
}

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
T = (char *) IndiceDeLaVariable;
if ( Spx->TypeDeStockageDeNBarreR == ADRESSAGE_INDIRECT_SPX ) {	
	memset( (char *) T, 0, Spx->NombreDeVariables * sizeof( char ) );
	NumVarNBarreRNonNul = Spx->NumVarNBarreRNonNul;
  for ( il = 0 ; il < Spx->NombreDeValeursNonNullesDeNBarreR ; il++ ) T[NumVarNBarreRNonNul[il]] = 1;
  for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
	  if ( T[VarSpx] == 0 ) NBarreRVecteur[VarSpx] = 0.0;
  }			
}

NbTermes = 0;

for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {

  if ( OrigineDeLaVariable[VarSpx] == BASIQUE_ARTIFICIELLE ) continue;

  /* Nettoyage des valeurs qui relevent plus du bruit numerique que des calculs */
	/* On nettoie les petites valeurs */
  if ( fabs( NBarreRVecteur[VarSpx] ) < ZERO_TERMES_DU_TABLEAU_POUR_GOMORY ) continue; 
	
  if ( PositionDeLaVariable[VarSpx] != HORS_BASE_SUR_BORNE_INF &&
       PositionDeLaVariable[VarSpx] != HORS_BASE_SUR_BORNE_SUP ) {
	  /* On a une variable non bornee hors base => on ne fait rien */
    *CodeRet = NON_SPX;
    return;		
	}

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
    if ( XminEntree[VarSpx] == XmaxEntree[VarSpx] && 0 ) { /* Car uniquement au noeud racine */			
      /* C'est une variable instanciee */
      if ( XminEntree[VarSpx] != 0. ) {			  
        AlphaI0-= NBarreR * Xmax[VarSpx] * ScaleXFoisSupDesXmax; 	 
        NBarreR = -NBarreR;
			}
    }      
  }

	/* On ne tient pas compte de HORS_BASE_SUR_BORNE_SUP car il va y avoir la substitution ensuite */
	if ( PositionDeLaVariable[VarSpx] == HORS_BASE_SUR_BORNE_SUP && 0 ) {
    /* On fait le changement de variable X = Xmax - Xtilde => Xtilde est hors base et vaut 0 */		
    AlphaI0-= NBarreR * Xmax[VarSpx] * ScaleXFoisSupDesXmax; 	 
    NBarreR = -NBarreR;		
  } 
       	
	IndiceDeLaVariable[NbTermes] = VarSpx;
  Coefficient[NbTermes] = NBarreR;
	NbTermes++;
}

*SecondMembre = AlphaI0;
*NombreDeTermes = NbTermes;
	
return;
}
     

