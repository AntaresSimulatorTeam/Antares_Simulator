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

   FONCTION: Epuration de la base dans le cas du noeud racine du branch
             and bound: on essai de faire sortir de la base les variables
	           artificelles des contraintes d'egalite et ainsi de diminuer
	           le nombre de variables.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "bb_define.h"		        
# include "bb_fonctions.h"		         

# define NOMBRE_MAX_DE_CYCLAGE_APRES_OPTIMALITE  10

/*----------------------------------------------------------------------------*/

void SPX_DualEpurerLaBaseSimplexe( PROBLEME_SPX * Spx ,
                                   int * NombreDeCyclagesApresOptimalite,
                                   char * EpurerLaBase ,
                                   char * Echec )
{
int Var; int Var1; double Amx; int Cnt; int il  ; int ilMax; double S;
int ic ;int NombreDeVariables; char EpurationFaite; 
int * ContrainteDeLaVariableEnBase; int * Mdeb; int * NbTerm; int * Indcol;
int * NumeroDeContrainte; int * Cdeb; char * StatutBorneSupCourante;
int * CNbTermSansCoupes;	int * VariableEnBaseDeLaContrainte; int *	CNbTerm;
char * OrigineDeLaVariable; char * PositionDeLaVariable; char * TypeDeVariable;
char * InDualFramework; char ControlerAdmissibiliteDuale; 
double * NBarreR; double * A; double * C; double * X; double * XminEntree;
double * Xmin;	double * XmaxEntree; double * Xmax; double * SeuilDeViolationDeBorne;
double * CBarre; double * ACol;	double * Csv; 
int i; int icMax; int iLimite; char Echange; int * NumerosDesVariables;
int * CdebBase; int * NbTermesDesColonnesDeLaBase;

/* Epuration de la base: on regarde s'il y a des variables artificielles dans la base.
   Si c'est le cas on essai de les en faire sortir */  
*Echec = NON_SPX;

OrigineDeLaVariable = Spx->OrigineDeLaVariable;
PositionDeLaVariable = Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
Mdeb = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A = Spx->A;
NBarreR = Spx->NBarreR;

C = Spx->C;
Csv = Spx->Csv;
StatutBorneSupCourante = Spx->StatutBorneSupCourante;
NumeroDeContrainte = Spx->NumeroDeContrainte;
Cdeb = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;

TypeDeVariable = Spx->TypeDeVariable;
X = Spx->X;
XminEntree = Spx->XminEntree;
Xmin = Spx->Xmin;
XmaxEntree = Spx->XmaxEntree;
Xmax = Spx->Xmax;
SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;
CBarre = Spx->CBarre;
InDualFramework = Spx->InDualFramework;
CNbTermSansCoupes = Spx->CNbTermSansCoupes;
ACol = Spx->ACol;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

CdebBase = Spx->CdebBase;
NbTermesDesColonnesDeLaBase = Spx->NbTermesDesColonnesDeLaBase;

if ( Spx->PremierSimplexe == OUI_SPX && *EpurerLaBase == OUI_SPX && 0 ) {
  /* L'algorithme de sortie de la base pour les variables artificielles est faux. En effet, il ne se base que sur
	   la conservation de l'adimissibilite primale. Or les couts reduits changent par le fait qu'une colonne de
		 la base change lorsqu'on fait sortir une variable artificielle sans prendre de precaution. En fait il
		 faut verifier que le cout reduit de la variable entrante est nul */
	/* Sinon ce qu'il faut faire c'est refaire des iterations de simplexe pour retrouver la base optimale. Comme les
	   variables artificielles ont ete sorties, elle n'entreront plus en base */
  #if VERBOSE_SPX
    printf("Recherche des variables basiques artificielles qui sont encore dans la base pour epuration \n");
  #endif	
  EpurationFaite = NON_SPX;
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
	
    if ( OrigineDeLaVariable[Var]  != BASIQUE_ARTIFICIELLE ) continue;  
    if ( PositionDeLaVariable[Var] != EN_BASE_LIBRE ) continue;      
    #if VERBOSE_SPX
      printf("Variable basique artificielle %d encore en base, on tente de la faire sortir \n",Var);
    #endif		
    /* La variable sortante est Var, seule une variable de la meme contrainte peut prendre sa place
       dans la base */
    Spx->VariableSortante = Var;
    Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMIN;
    Spx->VariableEntrante = -1;
    /* On calcule seulement NBarreR pour pouvoir selectionner une variable sortante */
    SPX_DualCalculerNBarreR( Spx, NON_SPX, &ControlerAdmissibiliteDuale ); 
    Spx->FactoriserLaBase = NON_SPX;
    SPX_DualVerifierErBMoinsUn( Spx );      
    if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
      if ( Spx->NombreDeChangementsDeBase == Spx->CycleDeRefactorisation ) Spx->FactoriserLaBase = OUI_SPX;
    }      
    if ( Spx->FactoriserLaBase == OUI_SPX ) {
      SPX_FactoriserLaBase( Spx );
      /* Si on est tombe sur un pivot nul, alors ça sent le roussi et on en reste la et on repart sur un simplexe */
			/* On detecte le fait qu'on soit tombe sur un pivot nul en testant la valeur de ChoixDeVariableSortanteAuHasard */
      if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) {
        *EpurerLaBase = NON_SPX;
        *Echec        = OUI_SPX; /* Si Echec = OUI on repart sur le simplexe */
	      return;
      }           	
      SPX_DualCalculerNBarreR( Spx, NON_SPX, &ControlerAdmissibiliteDuale ); 
    }

		/* Erreur  c'est pas la ContrainteDeLaVariableEnBase qu'il faut prendre mais la contrainte de la variable */
    /* Cnt = ContrainteDeLaVariableEnBase[Var]; */
    Cnt = NumeroDeContrainte[Cdeb[Var]];
		
    /* On ne peut pas supposer que le dernier terme est toujours celui de la variable basique artificielle */
		/* Oui maintenant on peut */
    /*
	 	il = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
    Amx = VALEUR_DE_PIVOT_ACCEPTABLE - 1.e-12;
    while ( il < ilMax ) {
      Var1 = Indcol[il];
      if ( PositionDeLaVariable[Var1] != EN_BASE_LIBRE && Var1 != Var ) {
        S = fabs( NBarreR[Var1] );
        if ( S > Amx ) {
  	      Spx->VariableEntrante = Var1;
          Amx = S;
        }
      }
      il++;
    }
    */
		/* Il faut tenir compte du type de stockage de NBarreR car si le stockage n'est pas VECTEUR_SPX,
		   alors il n'y a pas eu de RAZ de NBarreR */
    if ( Spx->TypeDeStockageDeNBarreR == VECTEUR_SPX ) {
      iLimite = Spx->NombreDeVariablesHorsBase;
	    NumerosDesVariables = Spx->NumerosDesVariablesHorsBase;
    }
    else {
      iLimite = Spx->NombreDeValeursNonNullesDeNBarreR;
	    NumerosDesVariables = Spx->NumVarNBarreRNonNul;
    }
    Amx = VALEUR_DE_PIVOT_ACCEPTABLE - 1.e-12;		
    for ( i = 0 ; i < iLimite ; i++ ) {   
      Var1 = NumerosDesVariables[i];
			S = fabs( NBarreR[Var1] );
      if ( S > Amx ) {
			  /* On verifie que la variable se trouve dans la contrainte */
				ic = Cdeb[Var1];
				icMax = ic + CNbTerm[Var1];
				Echange = NON_SPX;
				while ( ic < icMax ) {
          if ( NumeroDeContrainte[ic] == Cnt ) {
					  Echange = OUI_SPX;
						break;
					}
					ic++;
				}
				if ( Echange == OUI_SPX ) {				
 	        Spx->VariableEntrante = Var1;
          Amx = S;				
				}
      }
    }
		
    #if VERBOSE_SPX      
      printf("Variable sortante %d variable entrante %d Amx %e\n",Spx->VariableSortante,Spx->VariableEntrante,Amx);
    #endif
					
    if ( Spx->VariableEntrante >= 0 ) {
      EpurationFaite = OUI_SPX;	
      SPX_CalculerABarreS( Spx ); /* C'est pour calculer le spike et la maj du steepest edge */ 	
      Spx->FactoriserLaBase = NON_SPX;
      SPX_MajPoidsDualSteepestEdge( Spx ); /* Car utilise par le Strong Branching */	
      SPX_FaireLeChangementDeBase( Spx );
    }
				
  }
  /* Maintenant il faut recalculer les couts reduits */
  if ( EpurationFaite == OUI_SPX ) {
    SPX_CalculerPi( Spx );              /* Calcul de Pi = c_B * B^{-1} */
    SPX_CalculerLesCoutsReduits( Spx ); /* Calcul de CBarre = c_N - < Pi , N > */   
    SPX_CalculerBBarre( Spx );
  }
	
}

/* Si on ne vient pas immediatement de factoriser la base, on le fait afin de 
   l'utiliser dans le strong branching */
 
if ( Spx->NombreDeChangementsDeBase > 0 && Spx->ExplorationRapideEnCours == NON_SPX ) { 
  #if VERBOSE_SPX
    printf("Factorisation necessaire avant sauvegardes pour le strong branching ou les coupes de Gomory\n");
  #endif
  /* Remarque: il faudrait aussi verifier qu'on va effectivement faire du strong branching
               car il se peut que non et qu'on calcule des coupes a la place */
  SPX_FactoriserLaBase( Spx );	
  if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) { 
    *NombreDeCyclagesApresOptimalite = *NombreDeCyclagesApresOptimalite + 1; 
    if ( *NombreDeCyclagesApresOptimalite < NOMBRE_MAX_DE_CYCLAGE_APRES_OPTIMALITE ) {			
      *Echec = OUI_SPX; /* Si Echec = OUI on repart sur le simplexe */
      return;
    }
    else {
		  Spx->YaUneSolution = NON_SPX;
	    /* Il faut remettre toute les bornes natives car en cas de branch and bound cela peut faire
	       faire planter les simplexes suivants puisqu'on ne reinitialise pas le statut des bornes
				 sup courantes */
      for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
	      if ( StatutBorneSupCourante[Var] == BORNE_NATIVE ) continue;		
        SPX_SupprimerUneBorneAuxiliaire( Spx, Var );		
      }
			return;
		}
  }
}

/* On peut maintenant supprimer toutes les variables BASIQUE_ARTIFICIELLE qui sont hors base */
if ( Spx->PremierSimplexe == OUI_SPX ) {     
  #if VERBOSE_SPX      
    printf("Suppression des variables basiques artificielles qui ne sont plus dans la base \n");
  #endif	
  Var1 = Spx->NombreDeVariables;
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( OrigineDeLaVariable[Var] == BASIQUE_ARTIFICIELLE ) {
      Var1 = Var;
      break;
    }
  }
  /* On suppose que l'on va pouvoir enlever toutes les variables basiques artificielles */
  NombreDeVariables = Var1;
  for ( Var = Var1 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) continue;      
    #if VERBOSE_SPX      
      printf("Variable basique artificielle %d supprimee\n",Var);
    #endif		
		/* La variable artificielle est hors base: on l'enleve des donnees. Pour cela on l'enleve simplement des contraintes.
		   Les variables seront retassees ensuite */			
    C[Var] = 0.0;
    Cnt = NumeroDeContrainte[Cdeb[Var]];
    /* On ne peut pas supposer que le dernier terme est toujours celui de la variable basique artificielle */
		/* Attention: la correspondance ligne vers colonne et colonne vers ligne ne sera plus exploitable, pour la
		   reconstruire il faut appeler InitMatriceDeStockageDesContraintesParLigne */
    /* Maintenant que InitMatriceDeStockageDesContraintesParLigne n'existe plus pon peut supposer que le dernier
		   terme correspond a la variable d'ecart */
    il = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt] - 1;
    while ( il < ilMax ) {
	    if ( Indcol[il] == Var ) {
				A     [il] = A[ilMax];
			  Indcol[il] = Indcol[ilMax];
				break;
			}		  
      il++;
		}
	 	NbTerm[Cnt]--;	 			 
  }
  /* On remet dans la bonne numerotation les variables BASIQUE_ARTIFICIELLE qui sont
     restees dans la base */	
  for ( Var = Var1 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( OrigineDeLaVariable[Var]  != BASIQUE_ARTIFICIELLE ) continue;
    if ( PositionDeLaVariable[Var] != EN_BASE_LIBRE ) continue;      
    #if VERBOSE_SPX      
      printf("La variable basique artificielle %d reste dans les donnees du probleme avec le numero %d\n",
			        Var,NombreDeVariables);
    #endif     
		
    TypeDeVariable[NombreDeVariables] = BORNEE;
    C[NombreDeVariables] = 1.0;
    X[NombreDeVariables] = 0.; 
    XminEntree[NombreDeVariables] = 0.; 
    Xmin      [NombreDeVariables] = 0.; 
    XmaxEntree[NombreDeVariables] = 0.; 
    Xmax      [NombreDeVariables] = 0.;

		/* Pas besoin de toucher OrigineDeLaVariable, StatutBorneSupCourante, StatutBorneSupAuxiliaire */

		Csv[NombreDeVariables] = C[NombreDeVariables];
		
    SeuilDeViolationDeBorne[NombreDeVariables] = SeuilDeViolationDeBorne[Var];
    PositionDeLaVariable   [NombreDeVariables] = PositionDeLaVariable[Var];
    CBarre                 [NombreDeVariables] = 0.;

    InDualFramework             [NombreDeVariables] = InDualFramework[Var];		
    ContrainteDeLaVariableEnBase[NombreDeVariables] = ContrainteDeLaVariableEnBase[Var];
		VariableEnBaseDeLaContrainte[ContrainteDeLaVariableEnBase[NombreDeVariables]] = NombreDeVariables;
		
    ic = Cdeb[Var];                
    /* Matrice des contraintes par ligne */
    Cnt = NumeroDeContrainte[ic];		
		/* Il faut rechercher la variable artificielle: normalement c'est la derniere */
    il = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
    while ( il < ilMax ) {
	    if ( Indcol[il] == Var ) {
			  Indcol[il] = NombreDeVariables;
				break;
			}		  
      il++;
		}		
				
    /* Matrice des contraintes par colonne */
    Cdeb              [NombreDeVariables] = ic;      
    CNbTerm           [NombreDeVariables] = 1;
    CNbTermSansCoupes [NombreDeVariables] = 1;
    ACol              [ic]                = 1.;
    NumeroDeContrainte[ic]                = Cnt;

		/* Comme la variable reste en base, il faut changer la base */
    CdebBase                   [ContrainteDeLaVariableEnBase[NombreDeVariables]] = Cdeb[NombreDeVariables];
    NbTermesDesColonnesDeLaBase[ContrainteDeLaVariableEnBase[NombreDeVariables]] = CNbTerm[NombreDeVariables];
				 
    NombreDeVariables++;      
  }
  Spx->NombreDeVariables = NombreDeVariables;
  Spx->NombreDeVariablesDuProblemeSansCoupes = Spx->NombreDeVariables;
			
}

return;

}
