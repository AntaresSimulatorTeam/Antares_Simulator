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

   FONCTION: On fait des pivotages supplementaires pour essayer de
	           sortir de la base le plus grand nombre possible de
						 variables entieres. Ce n'est interessant que s'il y a
						 beaucoup de variables hors base avec un cout reduit nul.
						 Dans ce cas on peut esperer faire des pivotages sans faire
						 varier la fonction cout.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "bb_define.h"		        
# include "bb_fonctions.h"		        

# define SEUIL_VARIABLES_DEGENEREES 50.0
# define VALEUR_DE_ZERO             1.e-9

# ifdef PIVOTAGES_COMPLEMENTAIRES

/*----------------------------------------------------------------------------*/

void SPX_DualPivotagesComplementaires( PROBLEME_SPX * Spx )
{
int Var; int Var1; double Amx; int Cnt; int il  ; int ilMax; double S;
int ic ;int NombreDeVariables; int NbVarACoutReduitNul;
char * PositionDeLaVariable; double * CBarre; double Xs; int NombreDeContraintes;
int NombreDeVariablesEntieres; double * Xmin; double * Xmax; char * VariableEntiere;
double * ScaleX; int j; int i; int Count;
int * NumerosDesVariablesHorsBase; int * NumeroDesVariableATester;
double * NBarreRHorsBase; double * BBarre; double SupXmax;
int VariableSortante; char SortSurXmaxOuSurXmin; double Ec; double X;
int * VariableEnBaseDeLaContrainte; double Ecart; double SeuilHarris; int iChoisi;
char * TypeDeVariable; double NBarreRMx; double * ABarreS; 
double BBarreDeCntBase; int CntBase; int * ContrainteDeLaVariableEnBase;
char * SortieDeBasePossible; int NbPivotages; double NBarreR; char Position; int CountVarEnt;
double FractionnaliteMoyenne; int Nn; int * CntDeABarreSNonNuls;


printf("DualPivotagesComplementaires a revoir en fonction de base reduite ou complete\n");
return;


NombreDeVariables    = Spx->NombreDeVariables;
NombreDeContraintes  = Spx->NombreDeContraintes;
PositionDeLaVariable = Spx->PositionDeLaVariable;
CBarre               = Spx->CBarre;

/* Detection presence de variables entieres */
NbVarACoutReduitNul = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) continue;
  if ( fabs( CBarre[Var] ) < SEUIL_DE_DEGENERESCENCE_DUAL ) NbVarACoutReduitNul++;			
}

Xs = (double) NbVarACoutReduitNul * 100./ (double) (NombreDeVariables - NombreDeContraintes);
if ( Xs < SEUIL_VARIABLES_DEGENEREES) return;

printf("Pourcentage de variables hors base degenerees %f\n",Xs);

TypeDeVariable   = Spx->TypeDeVariable;
NBarreRHorsBase  = &Spx->NBarreRHorsBase[1];     
BBarre           = Spx->BBarre;
ABarreS          = Spx->ABarreS;
VariableEnBaseDeLaContrainte  = Spx->VariableEnBaseDeLaContrainte;
ContrainteDeLaVariableEnBase  = Spx->ContrainteDeLaVariableEnBase;

NumeroDesVariableATester    = Spx->NumeroDesVariableATester;
NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;

/* On repere les variables entieres */

VariableEntiere = (char *) malloc( NombreDeVariables * sizeof( char ) );
SortieDeBasePossible = (char *) malloc( NombreDeVariables * sizeof( char ) );
Xmin    = Spx->Xmin;
Xmax    = Spx->Xmax;
ScaleX  = Spx->ScaleX;
SupXmax = Spx->SupXmax;
NombreDeVariablesEntieres = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  VariableEntiere[Var] = NON_SPX;
  if ( Xmin[Var] != 0.0 ) continue;
	if ( fabs( (Xmax[Var] * ScaleX[Var] * SupXmax) - 1.0 ) < VALEUR_DE_ZERO ) {
	  VariableEntiere[Var] = OUI_SPX;
	  NombreDeVariablesEntieres++;
	}
}

printf(" NombreDeVariablesEntieres %d\n",NombreDeVariablesEntieres);

NbPivotages = 0;

RECHERCHE_VARIABLE_SORTANTE_1:
printf("************** Debut recherche Phase 1 *****************\n");

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) SortieDeBasePossible[Var] = OUI_SPX;

RECHERCHE_VARIABLE_SORTANTE_2:
printf("---> Debut recherche Phase 2\n");
/* On recherche une variable entiere basique eloignee de ses bornes */
VariableSortante = -1;
Ec = LINFINI_SPX;
FractionnaliteMoyenne = 0.0;
Nn = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  Var = VariableEnBaseDeLaContrainte[Cnt];
  if ( VariableEntiere[Var] == NON_SPX ) continue;		
	if ( SortieDeBasePossible[Var] == NON_SPX ) continue;
  X = BBarre[Cnt];
  if ( fabs( X - Xmin[Var] ) < VALEUR_DE_ZERO ) continue;
  if ( fabs( Xmax[Var] - X ) < VALEUR_DE_ZERO ) continue;


	if ( X < Xmin[Var] - 1.e-6 ) {printf(" X %e Xmin %e Xmax %e\n",X,Xmin[Var],Xmax[Var] ); exit(0);}
	if ( X > Xmax[Var] + 1.e-6 ) {printf(" X %e Xmin %e Xmax %e\n",X,Xmin[Var],Xmax[Var] ); exit(0);}

	
  Xs = 0.5 * ( Xmax[Var] + Xmin[Var] );


  Xs*= ScaleX[Var] * SupXmax;
	X*= ScaleX[Var] * SupXmax; 

	if ( X < Xs ) FractionnaliteMoyenne+= X - (Xmin[Var] * ScaleX[Var] * SupXmax);
	else FractionnaliteMoyenne+= (Xmax[Var] * ScaleX[Var] * SupXmax) - X;
  Nn++;
	
  Ecart = fabs( X - Xs );
  if ( Ecart < Ec ) {
	  VariableSortante = Var;
	  if ( X < Xs ) SortSurXmaxOuSurXmin = SORT_SUR_XMIN;
		else          SortSurXmaxOuSurXmin = SORT_SUR_XMAX;
	  Ec = Ecart;										
	}											
}
 
if ( VariableSortante < 0 ) {
  printf("                               Pas de variable sortante\n");
	goto FIN_PIVOTAGES;
  return;
}
Spx->VariableSortante     = VariableSortante;
Spx->SortSurXmaxOuSurXmin = SortSurXmaxOuSurXmin;
SortieDeBasePossible[VariableSortante] = NON_SPX;

CntBase         = ContrainteDeLaVariableEnBase[VariableSortante];
BBarreDeCntBase = BBarre[CntBase];
printf("Variable sortante %d valeur %e  min %e  max %e",
        VariableSortante,BBarreDeCntBase * ScaleX[VariableSortante] * SupXmax,
				Xmin[VariableSortante] * ScaleX[VariableSortante] * SupXmax,
				Xmax[VariableSortante] * ScaleX[VariableSortante] * SupXmax);
if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) printf("  SORT_SUR_XMIN\n");
else printf("  SORT_SUR_XMAX\n");

printf("Fractionnalite moyenne %e\n",FractionnaliteMoyenne/Nn);

/* On fait un test du ratio */

SPX_DualCalculerNBarreR( Spx , NON_SPX );

/* Construction des candidats */
j = 0;
for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  if ( fabs( NBarreRHorsBase[i] ) < VALEUR_DE_PIVOT_ACCEPTABLE ) continue;
  Var = NumerosDesVariablesHorsBase[i];
	/* On ne fait pas entrer de variables entieres dans la base */
	if ( VariableEntiere[Var] == OUI_SPX ) continue;
	/* On ne veut que des variables degenerees */
  if ( fabs( CBarre[Var] ) > SEUIL_DE_DEGENERESCENCE_DUAL ) continue;
  Position = PositionDeLaVariable[Var];
	NBarreR = NBarreRHorsBase[i];
	/* On ne recherche que les positions degenerees pour lesquelles le deplacement a des
	   chances de rester admissible */		 
  if ( SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
      if ( Position == HORS_BASE_SUR_BORNE_INF && NBarreR < 0.0 ) continue;
      if ( Position == HORS_BASE_SUR_BORNE_SUP && NBarreR > 0.0 ) continue;
  }
  else { /* La variable sort sur XMAX */
      if ( Position == HORS_BASE_SUR_BORNE_INF && NBarreR > 0.0 ) continue;
      if ( Position == HORS_BASE_SUR_BORNE_SUP && NBarreR < 0.0 ) continue;
  }	
  if ( Position == HORS_BASE_SUR_BORNE_INF || Position == HORS_BASE_SUR_BORNE_SUP ) {		
    NumeroDesVariableATester[j] = i;
    j++;      
  }
  else {
    /* On est dans la cas d'une variable non bornee */      
    NumeroDesVariableATester[0] = i;
    j = 1;
    break;	 
  }    
}

Spx->NombreDeVariablesATester = j;

if ( Spx->NombreDeVariablesATester <= 0 ) {
  printf("echec sur NombreDeVariablesATester\n");
  goto RECHERCHE_VARIABLE_SORTANTE_2;
  return; /* On risque de perdre du temps */
}

/* Choix de la variable entrante */
CountVarEnt = -1;

RECHERCHE_VARIABLE_ENTRANTE:
CountVarEnt++;
if ( CountVarEnt > 20 /* Spx->NombreDeVariablesATester */ ) Spx->NombreDeVariablesATester = 0;  

iChoisi   = -1;
NBarreRMx = -LINFINI_SPX;	
for ( j = 0 ; j < Spx->NombreDeVariablesATester ; j++ ) {	
  i = NumeroDesVariableATester[j];
  if ( i < 0 ) continue;		
  if ( fabs( NBarreRHorsBase[i] ) > NBarreRMx ) {
    NBarreRMx = fabs( NBarreRHorsBase[i] );
    iChoisi   = j;
    Spx->VariableEntrante = i;    
  }				
}	

if ( iChoisi < 0 ) {
  printf("echec pivot variable entrante instable\n");
  goto RECHERCHE_VARIABLE_SORTANTE_2;
}
NumeroDesVariableATester[iChoisi] = -1;

NBarreR = NBarreRHorsBase[Spx->VariableEntrante];
Spx->VariableEntrante = NumerosDesVariablesHorsBase[Spx->VariableEntrante];

printf("Variable entrante %d CBarre %e",Spx->VariableEntrante,Spx->CBarre[Spx->VariableEntrante]);
if ( PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_SUR_BORNE_INF ) printf(" position HORS_BASE_SUR_BORNE_INF\n");
if ( PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_SUR_BORNE_SUP ) printf(" position HORS_BASE_SUR_BORNE_SUP\n"); 
if ( PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_A_ZERO ) printf(" position HORS_BASE_A_ZERO\n");

/* Calcul du deplacement primal sur la variable entrante */
Spx->ChangementDeBase = OUI_SPX;

if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
  /* La variable en base diminue */
  Spx->DeltaXSurLaVariableHorsBase = BBarreDeCntBase / NBarreR;
}
else { /* SORT_SUR_XMAX */
  /* La variable en base augmente */
  Spx->DeltaXSurLaVariableHorsBase = ( BBarreDeCntBase - Xmax[Spx->VariableSortante] ) / NBarreR;
}

if ( PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_SUR_BORNE_INF ) {
  if ( Spx->DeltaXSurLaVariableHorsBase < 0.0 ) {
	  printf("echec variable entrante HORS_BASE_SUR_BORNE_INF devient infaisable DeltaXSurLaVariableHorsBase %e\n",
		        Spx->DeltaXSurLaVariableHorsBase);
    goto RECHERCHE_VARIABLE_SORTANTE_2;
	}
	else if ( TypeDeVariable[Spx->VariableEntrante] == BORNEE ) {
	  if ( Spx->DeltaXSurLaVariableHorsBase > Xmax[Spx->VariableEntrante] ) {
	    printf("echec bound flip\n");
      goto RECHERCHE_VARIABLE_SORTANTE_2;		
		}
	}
}
if ( PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_SUR_BORNE_SUP ) {
  if ( Spx->DeltaXSurLaVariableHorsBase > 0.0 ) {
	  printf("echec variable entrante HORS_BASE_SUR_BORNE_SUP devient infaisable DeltaXSurLaVariableHorsBase %e\n",
		        Spx->DeltaXSurLaVariableHorsBase);
    goto RECHERCHE_VARIABLE_SORTANTE_2;
	}
	else if ( -Spx->DeltaXSurLaVariableHorsBase > Xmax[Spx->VariableEntrante] ) {
	  printf("echec bound flip\n");
    goto RECHERCHE_VARIABLE_SORTANTE_2;		
	}
}

printf(" VariableEntrante  %d  pivot %e\n",Spx->VariableEntrante,NBarreRMx);

SPX_CalculerABarreS( Spx ); /* C'est pour calculer le spike et la maj du steepest edge */

/* On verifie si on ne cree pas de contraintes */

if ( Spx->TypeDeStockageDeABarreS == VECTEUR_SPX ) {
  for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
    if ( Cnt == CntBase ) continue;
	  X = BBarre[Cnt] - ( Spx->DeltaXSurLaVariableHorsBase * ABarreS[Cnt] );
	  Var = VariableEnBaseDeLaContrainte[Cnt];
	  if ( X < Xmin[Var] - 1.e-7 ) {
	    printf("Erreur: Spx->DeltaXSurLaVariableHorsBase = %e \n", Spx->DeltaXSurLaVariableHorsBase);
	    printf("  Var %d X = %e Xmin %e  valeur avant %e\n",Var,X,Xmin[Var],BBarre[Cnt]);
		  goto RECHERCHE_VARIABLE_ENTRANTE;	
	  }
	  if ( X > Xmax[Var] + 1.e-7 ) {
	    printf("Erreur: Spx->DeltaXSurLaVariableHorsBase = %e \n", Spx->DeltaXSurLaVariableHorsBase);
	    printf("  Var %d X = %e Xmax %e  valeur avant %e\n",Var,X,Xmax[Var],BBarre[Cnt]);
		  goto RECHERCHE_VARIABLE_ENTRANTE;			
	  }		
  }
}
else {
  CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	
  for ( i = 0 ; i < Spx->NbABarreSNonNuls ; i++ ) {
    Cnt = CntDeABarreSNonNuls[i];
    if ( Cnt == CntBase ) continue;
	  X = BBarre[Cnt] - ( Spx->DeltaXSurLaVariableHorsBase * ABarreS[i] );
	  Var = VariableEnBaseDeLaContrainte[Cnt];
	  if ( X < Xmin[Var] - 1.e-7 ) {
	    printf("Erreur: Spx->DeltaXSurLaVariableHorsBase = %e \n", Spx->DeltaXSurLaVariableHorsBase);
	    printf("  Var %d X = %e Xmin %e  valeur avant %e\n",Var,X,Xmin[Var],BBarre[Cnt]);
		  goto RECHERCHE_VARIABLE_ENTRANTE;	
	  }
	  if ( X > Xmax[Var] + 1.e-7 ) {
	    printf("Erreur: Spx->DeltaXSurLaVariableHorsBase = %e \n", Spx->DeltaXSurLaVariableHorsBase);
	    printf("  Var %d X = %e Xmax %e  valeur avant %e\n",Var,X,Xmax[Var],BBarre[Cnt]);
		  goto RECHERCHE_VARIABLE_ENTRANTE;			
	  }		
		
  }
}

Spx->FactoriserLaBase = NON_SPX;
SPX_MajPoidsDualSteepestEdge( Spx ); /* Car utilise par le Strong Branching */	
SPX_FaireLeChangementDeBase( Spx );
 
/* Maintenant il faut recalculer les couts reduits */
SPX_CalculerPi( Spx );              /* Calcul de Pi = c_B * B^{-1} */
SPX_CalculerLesCoutsReduits( Spx ); /* Calcul de CBarre = c_N - < Pi , N > */   
SPX_CalculerBBarre( Spx );

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  Var = VariableEnBaseDeLaContrainte[Cnt];
  X = BBarre[Cnt];
	if ( X < Xmin[Var] - 1.e-6 ) {
	  printf("Erreur: Spx->DeltaXSurLaVariableHorsBase = %e \n", Spx->DeltaXSurLaVariableHorsBase);
	  printf("  Var %d X = %e Xmin %e\n",Var,X,Xmin[Var]);
		exit(0);
	}
	if ( X > Xmax[Var] + 1.e-6 ) {
	  printf("Erreur: Spx->DeltaXSurLaVariableHorsBase = %e \n", Spx->DeltaXSurLaVariableHorsBase);
	  printf("  Var %d X = %e Xmax %e\n",Var,X,Xmax[Var]);
		exit(0);
	}
}


NbPivotages++;

goto RECHERCHE_VARIABLE_SORTANTE_1;

FIN_PIVOTAGES:
/* Si on ne vient pas immediatement de factoriser la base, on le fait afin de 
   l'utiliser dans le strong branching */
if ( Spx->NombreDeChangementsDeBase > 0 && Spx->ExplorationRapideEnCours == NON_SPX ) { 
  #if VERBOSE_SPX
    printf("Factorisation necessaire avant sauvegardes pour le strong branching ou les coupes de Gomory\n");
  #endif
  printf("Factorisation necessaire avant sauvegardes pour le strong branching ou les coupes de Gomory\n");
  /* Remarque: il faudrait aussi verifier qu'on va effectivement faire du strong branching
               car il se peut que non et qu'on calcule des coupes a la place */
  SPX_FactoriserLaBase( Spx );
  if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) { 
    printf("Attention probleme a la factorisation, prevoir qq chose \n");
  }
}

printf("NOMBRE DE PIVOTAGES REUSSIS: %d\n",NbPivotages);
exit(0);

return;
}

# endif
