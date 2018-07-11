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

   FONCTION: Phase 1 dans le cas ou la base de depart n'est pas fournie. 
             On positionne correctement les variables hors base compte 
             tenu de la crash base que l'on a determinee ou bien des 
             valeurs de couts reduits a la fin d'une iteration de phase 1.  
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define CYCLE_DAFFICHAGE 100
 
/*----------------------------------------------------------------------------*/

void SPX_DualPhase1PositionnerLesVariablesHorsBase( PROBLEME_SPX * Spx )  
{
int Var; double SommeDesInfaisabilites; char * PositionDeLaVariable; int i;
int * NumerosDesVariablesHorsBase; char * FaisabiliteDeLaVariable; char * TypeDeVariable;
double * CBarre; int NbInfaisabilitesDuales; double * SeuilDAmissibiliteDuale;
# if VERBOSE_SPX
 int NN;
# endif

NbInfaisabilitesDuales = 0;
SommeDesInfaisabilites = 0.0;		

if ( Spx->LaBaseDeDepartEstFournie == NON_SPX ) SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale1;
else SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale2;

PositionDeLaVariable        = Spx->PositionDeLaVariable;
NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
FaisabiliteDeLaVariable     = Spx->FaisabiliteDeLaVariable;
TypeDeVariable              = Spx->TypeDeVariable;
CBarre                      = Spx->CBarre;

/* Boucle sur les variables hors base */ 
for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  Var = NumerosDesVariablesHorsBase[i];
  
  FaisabiliteDeLaVariable[Var] = DUALE_FAISABLE;
	
  /* Examen des couts reduits pour chaque type de variable */
  if ( TypeDeVariable[Var] == BORNEE ) {
    if ( CBarre[Var] > SeuilDAmissibiliteDuale[Var] ) {		     
      PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
    }
    else if ( CBarre[Var] < -SeuilDAmissibiliteDuale[Var] ) {
      PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_SUP;
    }
    /* Remarque importante: lorsque la base de depart n'est pas fournie, on met initialement toutes les variables hors base 
       a HORS_BASE_SUR_BORNE_INF donc si les 2 tests ci-dessus ne positionne pas la variable c'est qu'elle est bien 
       positionnee */		
    continue;
  }

  if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
    PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF; 
    if ( CBarre[Var] < -SeuilDAmissibiliteDuale[Var] ) {
      SommeDesInfaisabilites+= CBarre[Var];
      PositionDeLaVariable   [Var] = HORS_BASE_SUR_BORNE_SUP; 
      FaisabiliteDeLaVariable[Var] = DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF;
      NbInfaisabilitesDuales++;
    }
    continue;
  }  	

  if ( TypeDeVariable[Var] == NON_BORNEE ) {  
    PositionDeLaVariable[Var] = HORS_BASE_A_ZERO; 
    /* La variable est duale realisable si son cout reduit est nul */		
    if ( CBarre[Var] < -SeuilDAmissibiliteDuale[Var] ) {
      SommeDesInfaisabilites+= CBarre[Var];
      PositionDeLaVariable   [Var] = HORS_BASE_SUR_BORNE_SUP; /* Donc infaisable */
      FaisabiliteDeLaVariable[Var] = DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF;
      NbInfaisabilitesDuales++;      
      continue;
    }
    if ( CBarre[Var] > SeuilDAmissibiliteDuale[Var] ) {
      SommeDesInfaisabilites-= CBarre[Var];
      PositionDeLaVariable   [Var] = HORS_BASE_SUR_BORNE_INF; /* Donc infaisable */
      FaisabiliteDeLaVariable[Var] = DUALE_INFAISABLE_PAR_COUT_REDUIT_POSITIF;
      NbInfaisabilitesDuales++;
      continue;
    }
  }
 
}

Spx->SommeDesInfaisabilitesDuales = -SommeDesInfaisabilites;
Spx->NbInfaisabilitesDuales       = NbInfaisabilitesDuales;

if ( Spx->Iteration == 1 ) Spx->NbInfaisabilitesDualesALaPremiereIteration = NbInfaisabilitesDuales;

/* Traces */
if ( Spx->NbCycles == 0 || Spx->NbCycles >= CYCLE_DAFFICHAGE ) { 
  #if VERBOSE_SPX
    if ( Spx->LaBaseDeDepartEstFournie == OUI_SPX ) {
      printf("Base de depart fournie, nombre d'infaisabilites %6d , somme des infaisabilites duales %15.8lf\n",Spx->NbInfaisabilitesDuales,-SommeDesInfaisabilites);     
    }
    else {
      printf("Iteration %6d nombre d'infaisabilites %6d , somme des infaisabilites duales %15.8lf\n",Spx->Iteration,Spx->NbInfaisabilitesDuales,
            -SommeDesInfaisabilites);     
    }
  #else 
  /* Cas non verbose */		 
  if ( Spx->LaBaseDeDepartEstFournie == NON_SPX && Spx->AffichageDesTraces == OUI_SPX ) { /* Premier simplexe */
    if ( Spx->EcrireLegendePhase1 == OUI_SPX ) {
      Spx->EcrireLegendePhase1 = NON_SPX;
      Spx->EcrireLegendePhase2 = OUI_SPX;      
      printf(" ");
      printf(" | Phase |");
      printf(" Iteration |");
      printf("    Dual infeas. count    |");
      printf("          Dual infeas.         |");
      printf("\n");               
    }   
    printf(" ");
    printf(" |    I  |");
    printf("   %6d  |",Spx->Iteration);
    printf("         %7d          |",Spx->NbInfaisabilitesDuales);
    printf("        %15.8e        |",-SommeDesInfaisabilites);
    printf("\n");    
  }						   
  #endif
  Spx->NbCycles = 0;

  # if VERBOSE_SPX
    if ( Spx->NbInfaisabilitesDuales == 1 ) {
		  NN = 0;
      for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
		    if (Spx->PositionDeLaVariable[Var] == EN_BASE_LIBRE ) continue;
        if ( FaisabiliteDeLaVariable[Var] != DUALE_FAISABLE ) {
			    NN++;
			    printf("positionner variables Iteration %d Var %d CNbTerm %d CBarre %e Xmin %e Xmax %e\n",
				          Spx->Iteration, Var,Spx->CNbTerm[Var],CBarre[Var],Spx->Xmin[Var],Spx->Xmax[Var]);
	  	  }
      }
    }
  # endif
	
}
Spx->NbCycles++;

/* Fin traces */

return;
}

# ifdef UTILISER_BORNES_AUXILIAIRES

/*----------------------------------------------------------------------------*/
/* Utilisation de bornes auxilaires pour forcer l'admissibilite duale */
void SPX_DualPhase1UtiliserLesBornesAuxiliaires( PROBLEME_SPX * Spx )	
{
int Var; char * PositionDeLaVariable; int i; double * CBarre;
int * NumerosDesVariablesHorsBase; char * FaisabiliteDeLaVariable; double * Xmax;
char * TypeDeVariable; char * StatutBorneSupCourante; double * Xmin;
double * SeuilDAmissibiliteDuale;

if ( Spx->LaBaseDeDepartEstFournie == NON_SPX ) SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale1;
else SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale2;

PositionDeLaVariable        = Spx->PositionDeLaVariable;
NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
FaisabiliteDeLaVariable     = Spx->FaisabiliteDeLaVariable;
TypeDeVariable              = Spx->TypeDeVariable;
CBarre                      = Spx->CBarre;

StatutBorneSupCourante   = Spx->StatutBorneSupCourante;
Xmax                     = Spx->Xmax;
Xmin                     = Spx->Xmin;

/* Boucle sur les variables hors base */ 
for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  Var = NumerosDesVariablesHorsBase[i];
  
  FaisabiliteDeLaVariable[Var] = DUALE_FAISABLE;
	
  /* Examen des couts reduits pour chaque type de variable */
  if ( TypeDeVariable[Var] == BORNEE ) {
    if ( CBarre[Var] > SeuilDAmissibiliteDuale[Var] ) {		     
      PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
    }
    else if ( CBarre[Var] < -SeuilDAmissibiliteDuale[Var] ) {
      PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_SUP;
    }
    /* Remarque importante: lorsque la base de part n'est pas fournie, on met initialement toutes les variables hors base 
       a HORS_BASE_SUR_BORNE_INF donc si les 2 tests ci-dessus ne positionne pas la variable c'est qu'elle est bien 
       positionnee */			 
    continue;
  }

  if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {		
    PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF; 
    /* La variable est duale realisable si son cout reduit est positif */		
    if ( CBarre[Var] < -SeuilDAmissibiliteDuale[Var] ) {
      PositionDeLaVariable  [Var] = HORS_BASE_SUR_BORNE_SUP; 
		  TypeDeVariable        [Var] = BORNEE;
      StatutBorneSupCourante[Var] = BORNE_AUXILIAIRE_DE_VARIABLE_BORNEE_INFERIEUREMENT;				
			Xmax                  [Var] = SPX_CalculerUneBorneAuxiliaire( Spx, Var );				
      Spx->NombreDeBornesAuxiliairesUtilisees++;				
    }
    continue;
  }

  if ( TypeDeVariable[Var] == NON_BORNEE ) {		
    PositionDeLaVariable[Var] = HORS_BASE_A_ZERO; 
    /* La variable est duale realisable si son cout reduit est nul */
    if ( CBarre[Var] < -SeuilDAmissibiliteDuale[Var] ) {			
      PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_SUP;			
			TypeDeVariable        [Var] = BORNEE;
      StatutBorneSupCourante[Var] = BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE;			
			Xmax [Var] = SPX_CalculerUneBorneAuxiliaire( Spx, Var );							
		 	Xmin[Var] = 0.;
      Spx->NombreDeBornesAuxiliairesUtilisees++;			
      continue;
    }
    if ( CBarre[Var] > SeuilDAmissibiliteDuale[Var] ) {			
      PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;			
		  TypeDeVariable        [Var] = BORNEE;
      StatutBorneSupCourante[Var] = BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE;						
			Xmax[Var] = SPX_CalculerUneBorneAuxiliaire( Spx, Var );					
			Xmin[Var] = 0.;						
      Spx->NombreDeBornesAuxiliairesUtilisees++;			
      continue;
    }				
  }
}

Spx->SommeDesInfaisabilitesDuales = 0.0;
Spx->NbInfaisabilitesDuales       = 0;

#if VERBOSE_SPX
  printf("Nombre de bornes auxiliaires utilisees %d\n",Spx->NombreDeBornesAuxiliairesUtilisees); 
#endif

return;
}

# endif





