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
   FONCTION: Modification du probleme dans un contexte de branch and 
             cut. On ajoute les coupes (ce sont des contraintes d'inegalite) 
             La variable d'ecart est hors base (i.e. contraintes saturees)
             ou en base. 
             En ce qui concerne le scaling, on ne fait pas participer la 
             coupe au scaling. En consequence ScaleB vaut 1 pour toutes 
             ces contraintes. 
             Remarque: a ce stade le probleme initial (avant coupes) est 
                       deja sous la forme standard.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# define TERME_NUL 0.0

/*----------------------------------------------------------------------------*/

void SPX_AjouterLesCoupes(
         PROBLEME_SPX * Spx,
      /* La matrice des contraintes rangee par lignes */
         int     NbContr_E,  /* Nombre de contraintes (coupes a ajouter) */
         char   * PositionDeLaVariableDEcart, /* HORS_BASE_SUR_BORNE_INF si la contrainte 
                                                 est saturee, EN_BASE dans le cas contraire */                                               
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
         double * B_E         /* Le second membre */
                              ) 
{
int Cnt_E  ; int il; int il_E ; int ilMax_E; int ilMax; int Var_E;
int Var; int VarSimplexe; double S; int NombreDeContraintes;
int NombreDeVariables; int * Mdeb; int * NbTerm; int * CorrespondanceVarEntreeVarSimplexe;
int * Indcol; char * TypeDeVariable; int * CorrespondanceVarSimplexeVarEntree;
int * VariableEnBaseDeLaContrainte; char * PositionDeLaVariable; int * ContrainteDeLaVariableEnBase;
char * OrigineDeLaVariable; int * CntVarEcartOuArtif;  double * A; double * ScaleX;
double * ScaleB; double * B; double * BAvantTranslationEtApresScaling; double * XminEntree;
double * XmaxEntree; double * X; double * C; double * Xmin; double * Xmax;
double * SeuilDeViolationDeBorne; double * Csv; int * CNbTermesDeCoupes;
char * StatutBorneSupCourante; char * StatutBorneSupAuxiliaire;
int * CorrespondanceCntSimplexeCntEntree; int NombreDeContraintesDuProblemeSansCoupes;

/*
printf("SPX_AjouterLesCoupes \n"); fflush(stdout);
*/

if ( Spx->NombreDeVariables   >= Spx->NombreDeVariablesAllouees   ) SPX_AugmenterLeNombreDeVariables( Spx );
if ( Spx->NombreDeContraintes >= Spx->NombreDeContraintesAllouees ) SPX_AugmenterLeNombreDeContraintes( Spx );

NombreDeContraintesDuProblemeSansCoupes = Spx->NombreDeContraintesDuProblemeSansCoupes;
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
CorrespondanceCntSimplexeCntEntree = Spx->CorrespondanceCntSimplexeCntEntree;
CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;
A      = Spx->A;
Indcol = Spx->Indcol;
ScaleX = Spx->ScaleX;
ScaleB = Spx->ScaleB;
B      = Spx->B;
BAvantTranslationEtApresScaling = Spx->BAvantTranslationEtApresScaling;
TypeDeVariable = Spx->TypeDeVariable;
XminEntree     = Spx->XminEntree;
XmaxEntree     = Spx->XmaxEntree;
X    = Spx->X;
C    = Spx->C;
Xmin = Spx->Xmin; 
Xmax = Spx->Xmax;
SeuilDeViolationDeBorne            = Spx->SeuilDeViolationDeBorne;
CorrespondanceVarSimplexeVarEntree = Spx->CorrespondanceVarSimplexeVarEntree;
VariableEnBaseDeLaContrainte       = Spx->VariableEnBaseDeLaContrainte;
PositionDeLaVariable               = Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBase       = Spx->ContrainteDeLaVariableEnBase;
Csv                      = Spx->Csv;	  
OrigineDeLaVariable      = Spx->OrigineDeLaVariable;
StatutBorneSupCourante   = Spx->StatutBorneSupCourante;
StatutBorneSupAuxiliaire = Spx->StatutBorneSupAuxiliaire;

CntVarEcartOuArtif  = Spx->CntVarEcartOuArtif;
  
NombreDeContraintes = Spx->NombreDeContraintes;  
NombreDeVariables   = Spx->NombreDeVariables;

/* Seules nous interessent les variables natives car pour les autres on sait que ça vaut 1 */
CNbTermesDeCoupes = Spx->CNbTermesDeCoupes;
memset ( (char *) CNbTermesDeCoupes, 0 , Spx->NombreDeVariables * sizeof( int ) );  

for ( Cnt_E = 0 ; Cnt_E < NbContr_E ; Cnt_E++ ) {

  if ( NbTerm_E[Cnt_E] <= 0 ) continue;
	
  il = Mdeb[NombreDeContraintes - 1] + NbTerm[NombreDeContraintes - 1]; 
  if ( il >= Spx->NbTermesAlloues ) {
    Spx->NombreDeContraintes = NombreDeContraintes;
    Spx->NombreDeVariables   = NombreDeVariables;          
    SPX_AugmenterLaTailleDeLaMatriceDesContraintes( Spx );
    NombreDeContraintes = Spx->NombreDeContraintes;
    NombreDeVariables   = Spx->NombreDeVariables;     
    A      = Spx->A;
    Indcol = Spx->Indcol;
  }

  Mdeb  [NombreDeContraintes] = il;	 
  NbTerm[NombreDeContraintes] = 0;
	
  il_E    = Mdeb_E[Cnt_E];
  ilMax_E = il_E + NbTerm_E[Cnt_E];
  while ( il_E < ilMax_E) {
    Var_E = Indcol_E[il_E];
    /* Ces variables ont toutes une correspondance avec les variables simplexe (sauf bug).
       Au pire, si ce sont des variables qui on ete instanciees leurs bornes sont identiques 
       mais elles ne sont pas de type VARIABLE_FIXE */
    VarSimplexe = CorrespondanceVarEntreeVarSimplexe[Var_E];				
    Indcol[il] = VarSimplexe;    
    A     [il] = A_E[il_E];    
    /* Scale en ScaleX de la contrainte */    
    A[il] *= ScaleX[VarSimplexe];		

		if ( fabs( A[il] ) > TERME_NUL ) {
	    /* On ne prend en compte le terme que s'il n'est pas "infiniment petit" */

      NbTerm[NombreDeContraintes]++; 
      CNbTermesDeCoupes[VarSimplexe]++;
			
      il++;		
      if ( il >= Spx->NbTermesAlloues ) {
        Spx->NombreDeContraintes = NombreDeContraintes;
        Spx->NombreDeVariables   = NombreDeVariables;      
        SPX_AugmenterLaTailleDeLaMatriceDesContraintes( Spx );
        NombreDeContraintes = Spx->NombreDeContraintes;  
        NombreDeVariables   = Spx->NombreDeVariables;  
        A      = Spx->A;
        Indcol = Spx->Indcol;
      }
		}

    il_E++;
  } 
	
  ScaleB[NombreDeContraintes] = 1.;
  B[NombreDeContraintes] = B_E[Cnt_E] * ScaleB[NombreDeContraintes];

  BAvantTranslationEtApresScaling[NombreDeContraintes] = B[NombreDeContraintes];

  /* Pour le second membre il faut tenir compte de la translation des bornes aussi */
  il    = Mdeb[NombreDeContraintes];
  ilMax = il + NbTerm[NombreDeContraintes];
  S     = 0.;
  while ( il < ilMax ) {
    Var = Indcol[il];
    if ( TypeDeVariable[Var] != NON_BORNEE ) { 
      S+= A[il] * XminEntree[Var] / ScaleX[Var];			
    }
    il++;
  }
  Spx->B[NombreDeContraintes]-=S;

  CorrespondanceCntSimplexeCntEntree[NombreDeContraintes] = NombreDeContraintesDuProblemeSansCoupes + Cnt_E;
	
  /* On ajoute la variable d'ecart dans la contrainte  */
  /* Creation de la variable d'ecart */

  XminEntree    [NombreDeVariables] = 0.;
  XmaxEntree    [NombreDeVariables] = LINFINI_POUR_X;
  X             [NombreDeVariables] = 0.;
  C             [NombreDeVariables] = 0.;
  Xmin          [NombreDeVariables] = 0.;
  Xmax          [NombreDeVariables] = LINFINI_POUR_X;
  TypeDeVariable[NombreDeVariables] = BORNEE_INFERIEUREMENT;

  SeuilDeViolationDeBorne[NombreDeVariables] = SEUIL_DE_VIOLATION_DE_BORNE_VARIABLES_ECART_COUPES;

  Spx->SeuilDAmissibiliteDuale1[NombreDeVariables] = SEUIL_ADMISSIBILITE_DUALE_1;
  Spx->SeuilDAmissibiliteDuale2[NombreDeVariables] = SEUIL_ADMISSIBILITE_DUALE_2;	
	
  CorrespondanceVarSimplexeVarEntree[NombreDeVariables] = -1;

  VariableEnBaseDeLaContrainte[NombreDeContraintes] = -1;
  if ( PositionDeLaVariableDEcart[Cnt_E] == EN_BASE ) {
    PositionDeLaVariable        [NombreDeVariables]   = EN_BASE_LIBRE;
    ContrainteDeLaVariableEnBase[NombreDeVariables]   = NombreDeContraintes; 
    VariableEnBaseDeLaContrainte[NombreDeContraintes] = NombreDeVariables;    
  }
  else if ( PositionDeLaVariableDEcart[Cnt_E] == HORS_BASE_SUR_BORNE_INF ) {
    PositionDeLaVariable[NombreDeVariables] = HORS_BASE_SUR_BORNE_INF;
  }
  else {  
    printf("Bug dans le sous programme SPX_AjouterLesCoupes. Arret d'urgence\n");
    exit(0);
  }
  /* On la met dans l'equation de la contrainte */
  il = Mdeb[NombreDeContraintes] + NbTerm[NombreDeContraintes];  

  if ( il >= Spx->NbTermesAlloues ) {
    Spx->NombreDeContraintes = NombreDeContraintes;        
    Spx->NombreDeVariables   = NombreDeVariables;         
    SPX_AugmenterLaTailleDeLaMatriceDesContraintes( Spx );
    NombreDeContraintes = Spx->NombreDeContraintes;  
    NombreDeVariables   = Spx->NombreDeVariables;
    A      = Spx->A;
    Indcol = Spx->Indcol;    
  }

  NbTerm[NombreDeContraintes]++; 
  Indcol[il] = NombreDeVariables;
  A     [il] = 1.;
 
  Csv                     [NombreDeVariables] = 0.;	  
  ScaleX                  [NombreDeVariables] = 1.;
  OrigineDeLaVariable     [NombreDeVariables] = ECART;
  StatutBorneSupCourante  [NombreDeVariables] = BORNE_NATIVE;
  StatutBorneSupAuxiliaire[NombreDeVariables] = BORNE_AUXILIAIRE_INVALIDE;
	
  CntVarEcartOuArtif [NombreDeVariables] = NombreDeContraintes;

  /* Incrementation du nombre de variables */
  NombreDeVariables++;
  if ( NombreDeVariables >= Spx->NombreDeVariablesAllouees ) {  
    Spx->NombreDeContraintes = NombreDeContraintes;        
    Spx->NombreDeVariables   = NombreDeVariables;          
    SPX_AugmenterLeNombreDeVariables( Spx );
    NombreDeContraintes = Spx->NombreDeContraintes;  
    NombreDeVariables   = Spx->NombreDeVariables;    
    C    = Spx->C;      
    Csv  = Spx->Csv; 
    X    = Spx->X;       
    Xmin = Spx->Xmin;   
    Xmax = Spx->Xmax;   
    TypeDeVariable      = Spx->TypeDeVariable;   
    OrigineDeLaVariable = Spx->OrigineDeLaVariable;
    StatutBorneSupCourante   = Spx->StatutBorneSupCourante;
    StatutBorneSupAuxiliaire = Spx->StatutBorneSupAuxiliaire;		
    CntVarEcartOuArtif  = Spx->CntVarEcartOuArtif;
    CNbTermesDeCoupes   = Spx->CNbTermesDeCoupes; 
    XminEntree = Spx->XminEntree;                         
    XmaxEntree = Spx->XmaxEntree;       
    SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;   
    ScaleX = Spx->ScaleX ;		
    CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;  
    CorrespondanceVarSimplexeVarEntree = Spx->CorrespondanceVarSimplexeVarEntree;                          
    PositionDeLaVariable         = Spx->PositionDeLaVariable;  
    ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;    
  }

  /* Incrementation du nombre de contraintes */
  NombreDeContraintes++; 
  if ( NombreDeContraintes >= Spx->NombreDeContraintesAllouees ) {
    Spx->NombreDeContraintes = NombreDeContraintes;        
    Spx->NombreDeVariables   = NombreDeVariables;    
    SPX_AugmenterLeNombreDeContraintes( Spx );
    NombreDeContraintes = Spx->NombreDeContraintes;  
    NombreDeVariables   = Spx->NombreDeVariables;        
    B = Spx->B;
    CorrespondanceCntSimplexeCntEntree = Spx->CorrespondanceCntSimplexeCntEntree;		
    BAvantTranslationEtApresScaling = Spx->BAvantTranslationEtApresScaling;
    ScaleB = Spx->ScaleB;
    Mdeb   = Spx->Mdeb;
    NbTerm = Spx->NbTerm; 
    VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;    
  }
						       
}

Spx->NombreDeContraintes = NombreDeContraintes;        
Spx->NombreDeVariables   = NombreDeVariables;

return;
}                     

