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
   FONCTION: Construction du probleme
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"
 
# include "pne_fonctions.h"
 
/*----------------------------------------------------------------------------*/

void SPX_ConstruireLeProbleme(
         PROBLEME_SPX * Spx,
         double * C_E,        /* Couts lineaires */
         double * X_E,        /* Inconnues */
         double * Xmin_E,     /* Borne inf des variables */
         double * Xmax_E,     /* Borne sup des variables */
         int     NbVar_E,    /* Nombre de variables */
         int   * TypeVar_E,  /* Indicateur du type de variable, il ne doit prendre que les 
                                 suivantes (voir le fichier spx_define.h mais ne jamais utiliser 
                                 les valeurs explicites des constantes): 
                                  VARIABLE_FIXE                  , 
                                  VARIABLE_BORNEE_DES_DEUX_COTES , 
                                  VARIABLE_BORNEE_INFERIEUREMENT , 
                                  VARIABLE_BORNEE_SUPERIEUREMENT , 	
                                  VARIABLE_NON_BORNEE     	  
                              */
      /* La matrice des contraintes rangee par lignes */
         int     NbContr_E,  /* Nombre de contraintes  */  
         int   * Mdeb_E,     /* Indice debut (dans A) des lignes de la matrice des 
                                 contraintes */
         int   * NbTerm_E,   /* Nombre de termes de la ligne */
         int   * Indcol_E,   /* Indice colonne des termes de chaque ligne. 
                                 Attention, les termes de la ligne doivent etre 
                                 ranges dans l'ordre croissant des indices de 
                                 colonnes */
         double * A_E,        /* Les termes de la matrice de contraintes contenus 
                                 dans un grand vecteur */
      /* Le second membre */
         char   * TypeDeContrainte_E, /* Type de contrainte < ou > ou = */ 
         double * B_E,                /* Le second membre */
      /* La base */
	       int * PositionDeLaVariable_E,
         int   NbVarDeBaseComplementaires_E,  
         int * ComplementDeLaBase_E,  
      /* Pilotage de l'algorithme dual */    
	       double CoutMax,
         int   UtiliserCoutMax
                              ) 
{
int i; int Cnt_E; int il; int ilMax; int il_E; int ilMax_E;  int Var_E;
double Coeff; int Cnt;  double S; double Marge; double * Csv; double * C;
double * Xmax; double * Xmin; double * X; char * TypeDeVariable; char * OrigineDeLaVariable;
char * CorrectionDuale; char * PositionDeLaVariable; int * CorrespondanceVarEntreeVarSimplexe; 
int * CorrespondanceVarSimplexeVarEntree; int * CorrespondanceCntEntreeCntSimplexe;
int * CorrespondanceCntSimplexeCntEntree; double * ScaleX; double * ScaleB;
double * SeuilDeViolationDeBorne; double * B; int * Mdeb; int * NbTerm; int * Indcol; double * A;
double * BAvantTranslationEtApresScaling; int NombreDeVariables; int NombreDeContraintes;
int NombreDeTermes; char YaDesVariablesBorneesSuperieurement; int NombreDeVariablesNatives;
char * StatutBorneSupCourante; char * StatutBorneSupAuxiliaire; int * NumeroDesVariablesACoutNonNul;
double * SeuilDAmissibiliteDuale1; double * SeuilDAmissibiliteDuale2;

YaDesVariablesBorneesSuperieurement = NON_SPX;

# if RELAXATION_CONTRAINTES == OUI_SPX
  if ( NbVar_E < NbContr_E ) Spx->FaireDeLaRelaxationDeContrainte = OUI_SPX;
	else Spx->FaireDeLaRelaxationDeContrainte = NON_SPX;
# endif

Spx->FaireScalingLU     = 0; 
Spx->NombreDeVariables  = 0;
Spx->CoutMax            = LINFINI_POUR_LE_COUT; /* Attention, ce n'est qu'une securite de plus, il vaut 
                                                  mieux ne pas trop compter dessus */
Spx->UtiliserCoutMax    = NON_SPX;
if ( Spx->AlgorithmeChoisi == SPX_DUAL && UtiliserCoutMax == OUI_SPX ) { 
  Marge = 1.e-6 * fabs ( CoutMax );
  if ( Marge < 1.e-6 ) Marge = 1.e-6;
  if ( Marge > 1.e+1 ) Marge = 1.e+1;
  Spx->CoutMax         = CoutMax + Marge;
  Spx->UtiliserCoutMax = OUI_SPX;
}

Spx->ToleranceSurLesVariablesEntieresAjustees = NON_SPX;

Spx->NombreDeVariablesACoutNonNul = 0;

Spx->PremierSimplexe              = OUI_SPX;
Spx->StockageParColonneSauvegarde = NON_SPX;

Spx->BuffNbBoundFlip = 0;
Spx->NbItBoundFlip   = 0;
Spx->FaireTriRapide  = NON_SPX;

Spx->PartieFixeDuCout = 0.;

Spx->UnSurRAND_MAX    = 1. / RAND_MAX;	
/* Initialisation d'une sequence pseudo aleatoire reproductible */ 
Spx->A1 = PNE_SRand( 0.192837465 ); /* Fonction PNE */
srand( 1 );               /* Fonction C */

Spx->LeSteepestEdgeEstInitilise = NON_SPX;

Spx->PlusGrandTermeDeLaMatrice = 1.;

/* Decompte des variables et premier controle de la base dans le cas ou elle est fournie */

Csv  = Spx->Csv ;
C    = Spx->C;
X    = Spx->X;
Xmax = Spx->Xmax;
Xmin = Spx->Xmin;
CorrectionDuale      = Spx->CorrectionDuale;
PositionDeLaVariable = Spx->PositionDeLaVariable;
TypeDeVariable       = Spx->TypeDeVariable;
OrigineDeLaVariable  = Spx->OrigineDeLaVariable;

StatutBorneSupCourante   = Spx->StatutBorneSupCourante;
StatutBorneSupAuxiliaire = Spx->StatutBorneSupAuxiliaire;

CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;
CorrespondanceVarSimplexeVarEntree = Spx->CorrespondanceVarSimplexeVarEntree;
CorrespondanceCntEntreeCntSimplexe = Spx->CorrespondanceCntEntreeCntSimplexe;
CorrespondanceCntSimplexeCntEntree = Spx->CorrespondanceCntSimplexeCntEntree;

NombreDeVariables = Spx->NombreDeVariables;
for ( i = 0 ; i < NbVar_E ; i++ ) {

  if ( TypeVar_E[i] == VARIABLE_FIXE ) {
    /* La variable est fixe, on suppose que la valeur est donnee dans X_E */  
    CorrespondanceVarEntreeVarSimplexe[i] = -1;
	  PositionDeLaVariable_E[i] = HORS_BASE_SUR_BORNE_INF; /* Histoire d'initialiser la valeur */
    Spx->PartieFixeDuCout += C_E[i] * X_E[i];
    continue;
  }

  Csv            [NombreDeVariables] = C_E[i];
  C              [NombreDeVariables] = 0.; 
  Xmax           [NombreDeVariables] = Xmax_E[i];
  Xmin           [NombreDeVariables] = Xmin_E[i];
  CorrectionDuale[NombreDeVariables] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT; 
 
  if ( TypeVar_E[i] == VARIABLE_BORNEE_DES_DEUX_COTES || 
       TypeVar_E[i] == VARIABLE_BORNEE_INFERIEUREMENT ) {               
    Spx->PartieFixeDuCout+= C_E[i] * Xmin_E[i];
  }

  if ( Spx->LaBaseDeDepartEstFournie == OUI_SPX ) {
    /* Base de depart fournie */
    if ( PositionDeLaVariable_E[i] == EN_BASE ) {
      PositionDeLaVariable[NombreDeVariables] = EN_BASE_LIBRE;    
      /* X[NombreDeVariables] = Xmin_E[i] ; */
    }
    else if ( PositionDeLaVariable_E[i] == HORS_BASE_SUR_BORNE_INF ) {
      PositionDeLaVariable[NombreDeVariables] = HORS_BASE_SUR_BORNE_INF;
      /* X                [NombreDeVariables] = Xmin_E[i]; */
    }
    else if ( PositionDeLaVariable_E[i] == HORS_BASE_SUR_BORNE_SUP ) {
      PositionDeLaVariable[NombreDeVariables] = HORS_BASE_SUR_BORNE_SUP;
      /* X                   [NombreDeVariables] = Xmax_E[i]; */	     
    }
    else if ( PositionDeLaVariable_E[i] == HORS_BASE_A_ZERO ) {
      PositionDeLaVariable[NombreDeVariables] = HORS_BASE_A_ZERO;
      /* X                   [NombreDeVariables] = 0.; */	     
    }
    else {
      printf(" Bug dans la fourniture de la base de depart, la variable %d est mal positionnee\n",i);
      printf(" son positionnement donne est %d \n",(int) PositionDeLaVariable_E[i]);
      exit(0);
    }
  }
  else {
    /* Base de depart non fournie */
    PositionDeLaVariable[NombreDeVariables] = HORS_BASE_SUR_BORNE_INF;
    /* X                   [NombreDeVariables] = Xmin_E[i]; */
  }

  CorrespondanceVarEntreeVarSimplexe[i] = NombreDeVariables;
  CorrespondanceVarSimplexeVarEntree[NombreDeVariables] = i;      

  if ( TypeVar_E[i] == VARIABLE_BORNEE_DES_DEUX_COTES ) TypeDeVariable[NombreDeVariables] = BORNEE;
  else if ( TypeVar_E[i] == VARIABLE_BORNEE_INFERIEUREMENT ) { 
    TypeDeVariable[NombreDeVariables] = BORNEE_INFERIEUREMENT;
    Xmax          [NombreDeVariables] = LINFINI_POUR_X;
  }
  else if ( TypeVar_E[i] == VARIABLE_BORNEE_SUPERIEUREMENT ) { 
    TypeDeVariable[NombreDeVariables] = BORNEE_SUPERIEUREMENT;
    Xmin          [NombreDeVariables] = -LINFINI_POUR_X;
    YaDesVariablesBorneesSuperieurement = OUI_SPX;
  }
  else if ( TypeVar_E[i] == VARIABLE_NON_BORNEE ) {
    TypeDeVariable[NombreDeVariables] = NON_BORNEE;
    Xmin          [NombreDeVariables] = -LINFINI_POUR_X;
    Xmax          [NombreDeVariables] = LINFINI_POUR_X;
  }
  else {
    printf("Erreur entree du solveur: le seul type de variables reconnues est: \n");
    printf("    VARIABLE_FIXE                  -> valeur de constante: %d\n",(int) VARIABLE_FIXE);
    printf("    VARIABLE_BORNEE_DES_DEUX_COTES -> valeur de constante: %d\n",(int) VARIABLE_BORNEE_DES_DEUX_COTES);
    printf("    VARIABLE_BORNEE_INFERIEUREMENT -> valeur de constante: %d\n",(int) VARIABLE_BORNEE_INFERIEUREMENT);
    printf("    VARIABLE_BORNEE_SUPERIEUREMENT -> valeur de constante: %d\n",(int) VARIABLE_BORNEE_SUPERIEUREMENT);
    printf("    VARIABLE_NON_BORNEE            -> valeur de constante: %d\n",(int) VARIABLE_NON_BORNEE);
    printf("Or la variable %d est du type %d => exit volontaire car pb de mise au point\n",i,(int) TypeVar_E[i]); 
    exit(0);
  }

  OrigineDeLaVariable     [NombreDeVariables] = NATIVE;
  StatutBorneSupCourante  [NombreDeVariables] = BORNE_NATIVE;
  StatutBorneSupAuxiliaire[NombreDeVariables] = BORNE_AUXILIAIRE_INVALIDE;
	
  NombreDeVariables++;
  
}

Spx->NombreDeVariables        = NombreDeVariables;
Spx->NombreDeVariablesNatives = NombreDeVariables;
NombreDeVariablesNatives      = NombreDeVariables;

B      = Spx->B;
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A      = Spx->A;

Spx->NombreDeContraintes = 0;
NombreDeContraintes      = 0;
for ( il = 0 , Cnt_E = 0 ; Cnt_E < NbContr_E ; Cnt_E++ ) {

  CorrespondanceCntEntreeCntSimplexe[Cnt_E] = -1;

  Coeff = 1.;
  if ( TypeDeContrainte_E[Cnt_E] == '>' ) Coeff = -1.; /* Pour transformer en < */

  Mdeb[NombreDeContraintes] = il;
  NombreDeTermes = 0;
  S = 0.0;

  il_E    = Mdeb_E[Cnt_E];
  ilMax_E = il_E + NbTerm_E[Cnt_E];  
  while ( il_E < ilMax_E) {
    Var_E = Indcol_E[il_E];
    /* On verifie le numero de la variable car il se peut que la contrainte 
       contienne des variables d'ecart (exemple coupes de Gomory) dans ce cas
       la contrainte sera construite lorsque les variables d'ecart seront connues */
    if ( Var_E >= NbVar_E ) {
      NombreDeTermes = 0;    
      break;  
    }		
    if ( TypeVar_E[Var_E] == VARIABLE_FIXE ) {
      /* Si la variable est fixee, on ne la met pas dans la matrice des contraintes 
         mais on la comptabilise dans le second membre */	 	 
      /*S+= A_E[il_E] * X_E[Var_E];*/
			/* BUG 14/12/2015: il faut aussi multipilier par Coeff comme dans le cas ou
			   la variable n'est pas fixee */
      S += Coeff * A_E[il_E] * X_E[Var_E];			
    }
    else {
      A     [il] = Coeff * A_E[il_E];
      Indcol[il] = CorrespondanceVarEntreeVarSimplexe[Var_E];
      NombreDeTermes++;
      il++;
    }
    il_E++;
  }
  /* Il y a des variables non fixees dans la contrainte ? */
  if ( NombreDeTermes == 0 ) { /* Pas de nouvelle contrainte => fin de boucle */
    /* printf("PAS NOUVELLE CONTRAINTE\n"); */
		if ( TypeDeContrainte_E[Cnt_E] == '=' ) {
		  if ( fabs( S - B_E[Cnt_E] ) > SEUIL_DADMISSIBILITE ) {
        Spx->YaUneSolution = NON_SPX; return;  			
			}
		}
		else if ( TypeDeContrainte_E[Cnt_E] == '<' ) {
		  if ( S > B_E[Cnt_E] + SEUIL_DADMISSIBILITE ) {
        Spx->YaUneSolution = NON_SPX; return;  			
			}		
		}
		else if ( TypeDeContrainte_E[Cnt_E] == '>' ) {
		  if ( S < B_E[Cnt_E] - SEUIL_DADMISSIBILITE ) {
        Spx->YaUneSolution = NON_SPX; return;  			
			}		
		}		
    continue;
  } 

  /* Les variables additionnelles */
  
  if ( TypeDeContrainte_E[Cnt_E] == '=' ) {	
    /* Dans le cas des contraintes d'egalite il ne peut y avoir au plus qu'une seule 
       variable additionnelle laquelle sera affectée a la base initiale */
       il++; /* On laisse de la place pour l'eventuelle variable de base */
  }
  else {	
    /* Dans le cas des contraintes d'inegalite il ne peut y avoir au plus que 2  
       variables additionnelles une variable d'ecart et si necessaire une variable 
       affectée a la base initiale */
       il++; /* On laisse de la place pour la variable d'ecart */
       il++; /* On laisse de la place pour l'eventuelle variable de base */
  }
  
  NbTerm[NombreDeContraintes] = NombreDeTermes;
  B[NombreDeContraintes] = ( Coeff * B_E[Cnt_E] ) - S;  
  
  CorrespondanceCntSimplexeCntEntree[NombreDeContraintes] = Cnt_E;
  CorrespondanceCntEntreeCntSimplexe[Cnt_E]               = NombreDeContraintes;
  NombreDeContraintes++;

}
Spx->NombreDeContraintes = NombreDeContraintes;

# if SPX_ACTIVATION_SUPPRESSION_PETITS_TERMES == OUI_SPX
  /* Imperatif: a faire avant la mise sous forme standard des contraintes */
	/* C'est deja fait dans la partie pne mais comme le simplexe petit etre appele separement,
	   et qu'a ce niveau on ne le sait pas, on relance ce calcul.
		 Remarque: il se peut cependant (rarissime) qu'il y a une (legere) difference avec la
		 partie PNE. En effet, la suppression des tout petit termes y est faite avant le
		 presolve or le simplexe est appele apres le presolve */ 
  PNE_EnleverLesToutPetitsTermes( Mdeb, NbTerm, Indcol, A, Xmax, Xmin, NombreDeContraintes, Spx->AffichageDesTraces );
# endif

if ( YaDesVariablesBorneesSuperieurement == NON_SPX ) goto FinVariablesBorneesSuperieurement;

/* Transformation interne: les variables de type BORNEE_SUPERIEUREMENT sont transformees en BORNEE_INFERIEUREMENT 
   par un simple changement de variable */
/* 1- Partie cout et bornes */
for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
  if ( TypeDeVariable[i] != BORNEE_SUPERIEUREMENT ) continue;
  Csv [i] = -Csv[i];
  C   [i] = -C[i]; 
  Xmin[i] = -Xmax[i];
  Xmax[i] = LINFINI_POUR_X;
}

/* 2- Partie matrice des contraintes */
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    if ( TypeDeVariable[Indcol[il]] == BORNEE_SUPERIEUREMENT ) A[il] = -A[il];
    il++;
  }
} 
  
/* 3- Changement de variable */
for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
  if ( TypeDeVariable[i] == BORNEE_SUPERIEUREMENT ) TypeDeVariable[i] = BORNEE_INFERIEUREMENT;
}
/* Fin de la transformation interne BORNEE_SUPERIEUREMENT -> BORNEE_INFERIEUREMENT */

FinVariablesBorneesSuperieurement:

/* Scaling du probleme avant sa mise sous forme standard (on le fait ici pour eviter 
   de faire un scaling sur les variables additionnelles de toutes sortes) */

BAvantTranslationEtApresScaling = Spx->BAvantTranslationEtApresScaling;
memcpy( (char *) BAvantTranslationEtApresScaling, (char *) B, Spx->NombreDeContraintes * sizeof( double ) );

SPX_TranslaterLesBornes( Spx );

/* SPX_ChainageDeLaTransposee( Spx , CREUX ); */ /* Car le scaling ne l'utilise plus */

ScaleX = Spx->ScaleX;
ScaleB = Spx->ScaleB;
SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;

Spx->ScaleLigneDesCouts = 1.;
for ( i = 0 ; i < Spx->NombreDeVariables   ; i++ ) { 
  ScaleX                 [i] = 1.;
  SeuilDeViolationDeBorne[i] = SEUIL_DE_VIOLATION_DE_BORNE;
}
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) ScaleB[i] = 1.;

SeuilDAmissibiliteDuale1 = Spx->SeuilDAmissibiliteDuale1;
SeuilDAmissibiliteDuale2 = Spx->SeuilDAmissibiliteDuale2;
for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
  SeuilDAmissibiliteDuale1[i] = SEUIL_ADMISSIBILITE_DUALE_1;
  SeuilDAmissibiliteDuale2[i] = SEUIL_ADMISSIBILITE_DUALE_2;
}

/* Calcul des matrices de scaling */
SPX_CalculerLeScaling( Spx );
/* Application du scaling */
SPX_Scaling( Spx );

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  BAvantTranslationEtApresScaling[Cnt] *= ScaleB[Cnt];
}

/* Mise du probleme sous la forme standard */
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  Cnt_E = CorrespondanceCntSimplexeCntEntree[Cnt];
  if ( Cnt_E < 0 ) { 
    printf(" Bug dans SPX_ConstruireLeProbleme \n");
    Spx->AnomalieDetectee = OUI_SPX;
    longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }
  /* Mise sous forme standard */
  if ( TypeDeContrainte_E[Cnt_E] != '=' ) { 
    SPX_MettreLaContrainteSousFormeStandard( Spx , Cnt );
    if ( Spx->YaUneSolution == NON_SPX ) return;  
  }	
}

/* Construction de la base de depart */

/* SPX_ChainageDeLaTransposee( Spx , COMPACT ); */  

if ( Spx->LaBaseDeDepartEstFournie == NON_SPX ) SPX_DualConstruireUneCrashBase( Spx );
else {
  SPX_DualConstruireLaBaseInitiale( Spx, NbVar_E                , PositionDeLaVariable_E, 
                                    NbVarDeBaseComplementaires_E, ComplementDeLaBase_E  , 
                                    TypeDeContrainte_E ); 
}

/* Stockage de la matrice des contraintes dans l'ordre croissant des colonnes */
/* Apres plusieurs essais, le stockage dans l'ordre ne semble pas avoir d'impatc
   significatif sur les temps de calculs */
/*
SPX_OrdonnerMatriceDesContraintes( Spx->NombreDeContraintes, Spx->Mdeb, Spx->NbTerm, 
                                   Spx->Indcol, Spx->A );
*/

SPX_ChainageDeLaTransposee( Spx , COMPACT );

/* Et on complete le facteur de scaling pour les variables ajoutees */
for ( i = NombreDeVariablesNatives ; i < Spx->NombreDeVariables ; i++ ) { 
  ScaleX         [i] = 1.;
  CorrectionDuale[i] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT; 
}

#if VERBOSE_SPX
  printf("Nombre de variables dans le simplexe %d\n", Spx->NombreDeVariables);
  fflush( stdout );
#endif

memcpy( (char *) C, (char *) Csv, Spx->NombreDeVariables * sizeof( double ) );

il = 0;
NumeroDesVariablesACoutNonNul = Spx->NumeroDesVariablesACoutNonNul;
for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
  if ( C[i] != 0 ) {
	  NumeroDesVariablesACoutNonNul[il] = i;
		il++;
	}
}
Spx->NombreDeVariablesACoutNonNul = il;

/* Sauvegarde du nombre de variables et du nombre de contraintes simplexe du 
   probleme sans coupes */
Spx->NombreDeVariablesDuProblemeSansCoupes   = Spx->NombreDeVariables;
Spx->NombreDeContraintesDuProblemeSansCoupes = Spx->NombreDeContraintes;

return;
}

/*----------------------------------------------------------------------------*/
/*       A ce stade, les contraintes d'inegalite sont toujours de signe <  
         et la place pour la variable d'ecart a deja ete reservee             */

void SPX_MettreLaContrainteSousFormeStandard( PROBLEME_SPX * Spx , int Cnt ) 
{
int il; int ilMax; int Var; double Smin; double Seuil; int * Indcol; double * A;
double * Xmin; double * Xmax; char * TypeDeVariable; double * B; int * Mdeb;
int * NbTerm; double * XminEntree; double * X; double * C; double * ScaleB;
double * SeuilDeViolationDeBorne; double * XmaxEntree; int * CntVarEcartOuArtif;
int * CorrespondanceVarSimplexeVarEntree; char * PositionDeLaVariable;
double * Csv; char * OrigineDeLaVariable; char * StatutBorneSupCourante; char * StatutBorneSupAuxiliaire;

Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A      = Spx->A;
B      = Spx->B;
ScaleB = Spx->ScaleB;
TypeDeVariable = Spx->TypeDeVariable;
Xmin = Spx->Xmin;
Xmax = Spx->Xmax;
XminEntree = Spx->XminEntree;
XmaxEntree = Spx->XmaxEntree;
X   = Spx->X;
C   = Spx->C;
Csv = Spx->Csv;
SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;
PositionDeLaVariable    = Spx->PositionDeLaVariable;
OrigineDeLaVariable     = Spx->OrigineDeLaVariable;
CntVarEcartOuArtif      = Spx->CntVarEcartOuArtif;
CorrespondanceVarSimplexeVarEntree = Spx->CorrespondanceVarSimplexeVarEntree;

StatutBorneSupCourante   = Spx->StatutBorneSupCourante;
StatutBorneSupAuxiliaire = Spx->StatutBorneSupAuxiliaire;

il    = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
Smin  = 0.;
while ( il < ilMax) {
  Var    = Indcol[il];
  if ( A[il] <= 0. ) { 
    if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT || TypeDeVariable[Var] == NON_BORNEE ) goto CreationDeLaVariableDEcart;
    Smin+= A[il] * Xmax[Var];
  }
  else {
    if ( TypeDeVariable[Var] == NON_BORNEE ) goto CreationDeLaVariableDEcart;    
    Smin+= A[il] * Xmin[Var];
  }
  il++;
}

if ( Smin > ( B[Cnt] + SEUIL_DADMISSIBILITE ) ) { 
  #if VERBOSE_SPX 
    printf("Simplexe: impossible de satisfaire la contrainte d'inegalite %d car Smin = %lf B = %lf \n",Cnt,Smin,Spx->B[Cnt]);
  #endif
  Spx->YaUneSolution = NON_SPX;
  return;
}

CreationDeLaVariableDEcart:

/* Creation de la variable d'ecart */
XminEntree    [Spx->NombreDeVariables] = 0.;
X             [Spx->NombreDeVariables] = 0.;
C             [Spx->NombreDeVariables] = 0.;
Xmin          [Spx->NombreDeVariables] = 0.;
Xmax          [Spx->NombreDeVariables] = LINFINI_POUR_X;
TypeDeVariable[Spx->NombreDeVariables] = BORNEE_INFERIEUREMENT;

Seuil = SEUIL_DE_VIOLATION_DE_BORNE_NON_NATIVE * ScaleB[Cnt];

if ( Seuil < SEUIL_MIN_DE_VIOLATION_DE_BORNE_NON_NATIVE ) Seuil = SEUIL_MIN_DE_VIOLATION_DE_BORNE_NON_NATIVE;  
else if ( Seuil > SEUIL_MAX_DE_VIOLATION_DE_BORNE_NON_NATIVE ) Seuil = SEUIL_MAX_DE_VIOLATION_DE_BORNE_NON_NATIVE;

SeuilDeViolationDeBorne[Spx->NombreDeVariables] = Seuil;

Spx->SeuilDAmissibiliteDuale1[Spx->NombreDeVariables] = SEUIL_ADMISSIBILITE_DUALE_1;
Spx->SeuilDAmissibiliteDuale2[Spx->NombreDeVariables] = SEUIL_ADMISSIBILITE_DUALE_2;

XmaxEntree[Spx->NombreDeVariables] = Xmax[Spx->NombreDeVariables];

CorrespondanceVarSimplexeVarEntree[Spx->NombreDeVariables] = -1;

PositionDeLaVariable[Spx->NombreDeVariables] = HORS_BASE_SUR_BORNE_INF;

/* On la met dans l'equation de la contrainte */
il = Mdeb[Cnt] + NbTerm[Cnt]; /* On a deja reserve la place de cette variable a la creation du probleme */
NbTerm[Cnt]++; 
Indcol[il] = Spx->NombreDeVariables;
A     [il] = 1.; /* Important: ne pas mettre autre chose que 1 */

Csv                     [Spx->NombreDeVariables] = C[Spx->NombreDeVariables];
OrigineDeLaVariable     [Spx->NombreDeVariables] = ECART;
StatutBorneSupCourante  [Spx->NombreDeVariables] = BORNE_NATIVE;
StatutBorneSupAuxiliaire[Spx->NombreDeVariables] = BORNE_AUXILIAIRE_INVALIDE;

CntVarEcartOuArtif [Spx->NombreDeVariables] = Cnt;

/* Incrementation du nombre de variables */
Spx->NombreDeVariables++;

return;
}


