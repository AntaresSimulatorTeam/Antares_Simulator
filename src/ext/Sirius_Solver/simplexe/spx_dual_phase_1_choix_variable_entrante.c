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

   FONCTION: Algorithme dual: choix de la variable qui entre en base.

   Rappel: dans l'alogrithme dual le test du ratio sert a determiner 
           la variable hors base qui entre en base alors que dans
           l'algorithme primal c'est l'inverse  
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1  SEUIL_ADMISSIBILITE_DUALE_1 /*SEUIL_ADMISSIBILITE_DUALE_2*/

# define NOMBRE_DE_FOIS_SCALING_SI_INVERSION_DE_TYPE_DE_SORTIE 0 /*5*/
    
/*----------------------------------------------------------------------------*/

void SPX_DualPhase1TestDuRatio( PROBLEME_SPX * Spx )
{
int Var   ; double T; double SeuilDePivot; double Tmin; double * CBarre;
double SeuilHarris   ; double MxNBarreR   ; double XnR; double Tmax;
int * NumerosDesVariablesHorsBase; char * TypeDeVariable; double * NBarreR;
int i; char * FaisabiliteDeLaVariable; int * NumerosDesVariables; int iLimite;

Tmin         = LINFINI_SPX;								      
SeuilDePivot = Spx->SeuilDePivotDual;

NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
TypeDeVariable              = Spx->TypeDeVariable;
NBarreR                     = Spx->NBarreR;
FaisabiliteDeLaVariable     = Spx->FaisabiliteDeLaVariable;
CBarre                      = Spx->CBarre;

if ( Spx->TypeDeStockageDeNBarreR == VECTEUR_SPX ) {
  iLimite = Spx->NombreDeVariablesHorsBase;
	NumerosDesVariables = Spx->NumerosDesVariablesHorsBase;	
}
else {
  iLimite = Spx->NombreDeValeursNonNullesDeNBarreR;
  NumerosDesVariables = Spx->NumVarNBarreRNonNul;
}

if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) {   
  /* La variable en base doit sortir de la base vers sa borne sup. pour etre duale realisable (T>=0) */
  for ( i = 0 ; i < iLimite ; i++ ) {	
    Var = NumerosDesVariables[i];
		
    if ( TypeDeVariable[Var] == BORNEE ) continue; /* Pas de point de cassure */ 

    /* Les variables entrantes doivent satisfaire la condition CBarre et NBarreR de meme signe */
		
    if ( NBarreR[Var] > SeuilDePivot ) {				
      /* La variable sortante fait diminuer CBarre */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        if ( TypeDeVariable[Var] == NON_BORNEE ) { 
          T = fabs( TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 / NBarreR[Var] );
          if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
          continue;
        }
				else {
				  /* La variable est faisable BORNEE_INFERIEUREMENT son cout reduit est >= - le seuil d'admissibilite duale */
					if ( CBarre[Var] < 0. ) { 
            T = fabs( TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 / NBarreR[Var] );
            if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
            continue;
          }										
				}
      }
      if ( CBarre[Var] >= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_POSITIF /* Donc non bornee */ || 
				     TypeDeVariable         [Var] == BORNEE_INFERIEUREMENT /* La variable est donc faisable */ ) {
          /* On a un point de cassure */
          T = fabs( ( CBarre[Var] + TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 ) / NBarreR[Var] );
          if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
          continue;
        }
      }
    }

    if ( NBarreR[Var] < -SeuilDePivot ) {		
      /* La variable sortante fait augmenter CBarre */
      if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) continue; 
      /* La variable est donc non bornee */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        T = fabs( TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 / NBarreR[Var] );
        if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
        continue;        
      }			
      if ( CBarre[Var] <= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF ) {
          /* La variable est donc non bornee infaisable */
          /* On a un point de cassure */
          T = fabs( ( CBarre[Var] - TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 ) / NBarreR[Var] );
          if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
          continue;
        }
      }
    }

  }
}

else if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {  
  /* La variable en base doit sortir de la base vers sa borne inf. pour etre duale realisable (T<=0) */
  for ( i = 0 ; i < iLimite ; i++ ) {	
    Var = NumerosDesVariables[i];
		
    if ( TypeDeVariable[Var] == BORNEE ) continue; /* Pas de point de cassure */ 

    /* Les variables entrantes doivent satisfaire la condition CBarre et NBarreR de signe contrainte */

    if ( NBarreR[Var] > SeuilDePivot ) {	
      /* La variable sortante fait augmenter CBarre */
      if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) continue; 
      /* La variable est donc non bornee */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        T = fabs( TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 / NBarreR[Var] );
        if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
        continue;       
      }
      if ( CBarre[Var] <= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF ) {
          /* La variable est donc non bornee infaisable */
          T = fabs( ( CBarre[Var] - TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 ) / NBarreR[Var] );
          if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
          continue;
        }
      }
    }

    if ( NBarreR[Var] < -SeuilDePivot ) {		
		  /* La variable sortante fait diminuer CBarre */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        if ( TypeDeVariable[Var] == NON_BORNEE ) { 
          T = fabs( TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 / NBarreR[Var] );
          if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
          continue;
        }
				else { /* La variable est faisable BORNEE_INFERIEUREMENT son cout reduit est >= -Seuil */
					if ( CBarre[Var] < 0. ) { 
            T = fabs( TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 / NBarreR[Var] );
            if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
            continue;
          }
				}
      }
      if ( CBarre[Var] >= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_POSITIF /* Donc non bornee */ ||
						 TypeDeVariable         [Var] == BORNEE_INFERIEUREMENT /* La variable est donc faisable */ ) {
          T = fabs( ( CBarre[Var] + TOLERANCE_POUR_LE_TEST_DE_HARRIS_PHASE_1 ) / NBarreR[Var] );
          if ( T < Tmin ) { Tmin = T; Spx->VariableEntrante = Var; }
          continue;
        }
      }
    }

  }
}

/* Deuxieme passe sans les tolerance Harris */
SeuilHarris = Tmin; 
MxNBarreR = -LINFINI_SPX;
 
if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) {   
  /* La variable en base doit sortir de la base vers sa borne sup. pour etre duale realisable (T>=0) */	
  for ( i = 0 ; i < iLimite ; i++ ) {	
    Var = NumerosDesVariables[i];
	
    if ( TypeDeVariable[Var] == BORNEE ) continue; /* Pas de point de cassure */ 

    /* Les variables entrantes doivent satisfaire la condition CBarre et NBarreR de meme signe */

    if ( NBarreR[Var] > SeuilDePivot ) {		
      XnR = NBarreR[Var];
      /* La variable sortante fait diminuer CBarre */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        if ( TypeDeVariable[Var] == NON_BORNEE ) { 
          T = 0.; 
          if ( T <= SeuilHarris /*&& XnR > MxNBarreR*/ ) {
					  Spx->VariableEntrante = Var; MxNBarreR = XnR;
						return; /* Priorite aux variables non bornees */
					}
          continue;
        }
				else { /* La variable est faisable BORNEE_INFERIEUREMENT son cout reduit est >= - le seuil d'admissibilite duale */
					if ( CBarre[Var] < 0. ) { 
            T = 0.; 
            if ( T <= SeuilHarris && XnR > MxNBarreR ) { Spx->VariableEntrante = Var; MxNBarreR = XnR; }
            continue;
          }
				}
      }
      if ( CBarre[Var] >= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_POSITIF /* Donc non bornee */ || 
						 TypeDeVariable         [Var] == BORNEE_INFERIEUREMENT /* La variable est donc faisable */ ) {
          /* On a un point de cassure */
          T = fabs( CBarre[Var] / NBarreR[Var] );
          if ( T <= SeuilHarris ) {
						if ( TypeDeVariable[Var] == NON_BORNEE ) {
					    Spx->VariableEntrante = Var; MxNBarreR = XnR;
						  return; /* Priorite aux variables non bornees */
            }
            if ( XnR > MxNBarreR ) {					
					    Spx->VariableEntrante = Var; MxNBarreR = XnR;
						}
					}
          continue;
        }
      }
    }

    if ( NBarreR[Var] < -SeuilDePivot ) {		
      XnR = -NBarreR[Var];
      /* La variable sortante fait augmenter CBarre */
      if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) continue; 
      /* La variable est donc non bornee */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        T = 0.; 
        if ( T <= SeuilHarris /*&& XnR > MxNBarreR*/ ) {
				  Spx->VariableEntrante = Var; MxNBarreR = XnR;
					return; /* Priorite aux variables non bornees */
				}
        continue;        
      }
      if ( CBarre[Var] <= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF ) {
          /* La variable est donc non bornee infaisable */
          /* On a un point de cassure */
          T = fabs( CBarre[Var] / NBarreR[Var] );
          if ( T <= SeuilHarris /*&& XnR > MxNBarreR*/ ) {
					  Spx->VariableEntrante = Var; MxNBarreR = XnR;
					  return; /* Priorite aux variables non bornees */
					}
          continue;
        }
      }
    }

  }
}

else if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {  
  /* La variable en base doit sortir de la base vers sa borne inf. pour etre duale realisable (T<=0) */
  for ( i = 0 ; i < iLimite ; i++ ) {	
    Var = NumerosDesVariables[i];
	 	
    if ( TypeDeVariable[Var] == BORNEE ) continue; /* Pas de point de cassure */ 

    /* Les variables entrantes doivent satisfaire la condition CBarre et NBarreR de signe contrainte */

    if ( NBarreR[Var] > SeuilDePivot ) {		
      XnR = NBarreR[Var];
      /* La variable sortante fait augmenter CBarre */
      if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) continue; 
      /* La variable est donc non bornee */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        T = 0.; 
        if ( T <= SeuilHarris /*&& XnR > MxNBarreR*/ ) {
				  Spx->VariableEntrante = Var; MxNBarreR = XnR;
				  return; /* Priorite aux variables non bornees */
				}
        continue;       
      }
      if ( CBarre[Var] <= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF ) {
          /* La variable est donc non bornee infaisable */
          T = fabs( CBarre[Var] / NBarreR[Var] );
          if ( T <= SeuilHarris /*&& XnR > MxNBarreR*/ ) {
					  Spx->VariableEntrante = Var; MxNBarreR = XnR;
				    return; /* Priorite aux variables non bornees */
					}
          continue;
        }
      }
    }

    if ( NBarreR[Var] < -SeuilDePivot ) {		
      XnR = -NBarreR[Var];
      /* La variable sortante fait diminuer CBarre */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_FAISABLE ) {
        if ( TypeDeVariable[Var] == NON_BORNEE ) { 
          T = 0.; 
          if ( T <= SeuilHarris /*&& XnR > MxNBarreR*/ ) {
					  Spx->VariableEntrante = Var; MxNBarreR = XnR;
				    return; /* Priorite aux variables non bornees */
					}
          continue;
        }
				else { /* La variable est faisable BORNEE_INFERIEUREMENT son cout reduit est >= -Seuil */
					if ( CBarre[Var] < 0. ) { 
            T = 0.; 
            if ( T <= SeuilHarris && XnR > MxNBarreR ) { Spx->VariableEntrante = Var; MxNBarreR = XnR; }
            continue;
          }
				}
      }
      if ( CBarre[Var] >= 0. ) { 
        if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_POSITIF /* Donc non bornee */ ||
						 TypeDeVariable         [Var] == BORNEE_INFERIEUREMENT /* La variable est donc faisable */ ) {
          T = fabs( CBarre[Var] / NBarreR[Var] );					
          if ( T <= SeuilHarris ) {
					  if ( TypeDeVariable[Var] == NON_BORNEE ) {
					    Spx->VariableEntrante = Var; MxNBarreR = XnR;
							return; /* Priorite aux variables non bornees */
						}
					  if ( XnR > MxNBarreR ) {
						  Spx->VariableEntrante = Var; MxNBarreR = XnR;
						}
					}
          continue;
        }
      }
    }

  }
}

if ( Spx->VariableEntrante >= 0 ) return;

/* Il se peut qu'en entrant en base, la variable ne crée pas de nouvelle infaisabilité duale. 
   Cela signifie que soit elle fait diminuer le nombre de variables infaisables, soit elle ne 
   le change pas, mais tout ceci sans créer de nouvelles infaisabilités. Ceci ne peut se produire
   que si toutes les infaisabilités sont de type DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF sur 
   des variables bornées inferieurement. Dans ce cas on choisit d'echanger avec la derniere 
   variable qui devient duale réalisable */

Tmax = -LINFINI_SPX;
								      
if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) {   
  /* La variable en base doit sortir de la base vers sa borne sup. pour etre duale realisable (T>=0) */
  for ( i = 0 ; i < iLimite ; i++ ) {			
    Var = NumerosDesVariables[i];			
    if ( NBarreR[Var] < -SeuilDePivot ) {		
      /* La variable sortante fait augmenter CBarre */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF ) {
        /* Remarque: la variable est donc bornée inférieurement */
        T = fabs( CBarre[Var] / NBarreR[Var] );
        if ( T > Tmax ) { Tmax = T; Spx->VariableEntrante = Var; }
      }
    }
  }
}

else if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {  
  /* La variable en base doit sortir de la base vers sa borne inf. pour etre duale realisable (T<=0) */
  for ( i = 0 ; i < iLimite ; i++ ) {					
    Var = NumerosDesVariables[i];			
    if ( NBarreR[Var] >= SeuilDePivot ) {
      /* La variable sortante fait augmenter CBarre */
      if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF ) {
        /* Remarque: la variable est donc bornée inférieurement */
        T = fabs( CBarre[Var] / NBarreR[Var] );
        if ( T > Tmax ) { Tmax = T; Spx->VariableEntrante = Var; }
      }
    }
  }
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_DualPhase1ChoixDeLaVariableEntrante( PROBLEME_SPX * Spx )
{
 
Spx->VariableEntrante = -1;
SPX_DualPhase1TestDuRatio( Spx );

/* Recherche de la variable entrante */
if ( Spx->VariableEntrante < 0 ) { 
  /* Problemes numerique: en désespoir de cause, si la variables sortante est bornee,
     on tente le changement de borne inverse */
  if ( Spx->TypeDeVariable[Spx->VariableSortante] == BORNEE ) {
    if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMAX; 
    else Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMIN;
    printf("Phase 1 iter %d: inversion du type de sortie de la variable sortante \n",Spx->Iteration);

    Spx->FaireScalingLU = NOMBRE_DE_FOIS_SCALING_SI_INVERSION_DE_TYPE_DE_SORTIE;

    SPX_DualPhase1TestDuRatio( Spx );
    if ( Spx->VariableEntrante >= 0 ) {
      printf("variable sortante %d poids de la variable sortante %lf contrainte %d\n",
              Spx->VariableSortante,Spx->DualPoids[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]],
              Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]);
    }
    else {
      printf("pas de variable entrante malgre l'inversion\n");
    }
  }
}

return;
}

