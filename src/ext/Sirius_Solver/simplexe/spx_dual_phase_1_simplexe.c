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

   FONCTION: Phase 1 de l'algorithme dual du simplexe: recherche d'une 
             solution duale admissible.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"  
  
/*----------------------------------------------------------------------------*/

void SPX_DualPhase1Simplexe( PROBLEME_SPX * Spx )

{
char ModifCoutsAutorisee; int CntBase; double MoyenneSommeDesInfaisabilites;
double BufferSommeDesInfaisabilites; int CycleDeCalculDeLaMoyenne; int NbCycles;	
char PositionDeLaVariable; char FactoriserLaBase; int Var;
char ControlerAdmissibiliteDuale;

#if VERBOSE_SPX
  printf("Entree dans la phase 1 de l algorithme dual du simplexe\n");  
#endif

Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;

Spx->PhaseEnCours = PHASE_1;

Spx->EcrireLegendePhase1 = OUI_SPX;
  
ModifCoutsAutorisee	      = Spx->ModifCoutsAutorisee;
/* Attention remettre ensuite la valeur d'origine */
Spx->ModifCoutsAutorisee       = NON_SPX; 

Spx->LaBaseEstDualeAdmissible  = NON_SPX;
Spx->Iteration                 = 0;
/*Spx->NombreDeChangementsDeBase = 0;*/
/*Spx->LastEta                   = -1;*/

Spx->CalculerCBarre            = OUI_SPX; 

Spx->ChoixDeVariableSortanteAuHasard = NON_SPX;
Spx->NombreMaxDeChoixAuHasard        = 5;
Spx->NombreDeChoixFaitsAuHasard      = 0;
Spx->FaireDuRaffinementIteratif      = 0;
MoyenneSommeDesInfaisabilites = -LINFINI_SPX;
BufferSommeDesInfaisabilites  = 0.;

NbCycles                 = 0;
CycleDeCalculDeLaMoyenne = 5/*6*/;

while ( 1 ) {

  Debut: 
  Spx->Iteration++;
	Spx->FaireChangementDeBase = OUI_SPX;
	Spx->NbBoundFlip = 0;

  if ( Spx->FaireDuRaffinementIteratif > 0 ) Spx->FaireDuRaffinementIteratif--;			

  /*
  printf("*** Iteration %d dans la phase 1 l'algorithme dual\n",Spx->Iteration); 
  */
	
  if ( Spx->DureeMaxDuCalcul > 0 ) SPX_ControleDuTempsEcoule( Spx );
  
  if ( Spx->Iteration > Spx->NombreMaxDIterations ) { /* Clause de sortie */
    Spx->LaBaseEstDualeAdmissible = NON_SPX;
    break;
  }

  if ( Spx->CalculerCBarre == OUI_SPX ) {
    SPX_CalculerPi( Spx );              /* Calcul de Pi = c_B * B^{-1} */
    SPX_CalculerLesCoutsReduits( Spx ); /* Calcul de CBarre = c_N - < Pi , N > */ 
    Spx->CalculerCBarre = NON_SPX; 
  }

  /* On positionne les variables bornees en fonction de leur cout reduit et on 
     on determine les infaisabilites sur les variables non bornees d'un seul 
     ou des deux cotes */		 
  SPX_DualPhase1PositionnerLesVariablesHorsBase( Spx );	
  if ( Spx->NbInfaisabilitesDuales == 0 ) {
    /* La solution est duale admissible */
    #if VERBOSE_SPX
      printf( "Simplexe dual phase 1: admissibilite duale atteint en %d iterations\n",Spx->Iteration-1);
    #endif		
    Spx->LaBaseEstDualeAdmissible = OUI_SPX;
    break;
  }

  # ifdef UTILISER_BORNES_AUXILIAIRES
    if ( Spx->Iteration > Spx->IterationPourBornesAuxiliaires ) {
	    /* On utilise les bornes auxiliaires pour obtenir l'admissibilite duale */
      SPX_DualPhase1UtiliserLesBornesAuxiliaires( Spx );
      if ( Spx->NbInfaisabilitesDuales == 0 ) {
        /* La solution est duale admissible */
        #if VERBOSE_SPX
          printf( "Simplexe dual phase 1: admissibilite duale atteint en %d iterations\n",Spx->Iteration-1);
        #endif
        Spx->LaBaseEstDualeAdmissible = OUI_SPX;
        break;
      }			
  	}
  # endif
	
  NbCycles++;
  BufferSommeDesInfaisabilites+= Spx->SommeDesInfaisabilitesDuales;
  if ( NbCycles == CycleDeCalculDeLaMoyenne ) {
    BufferSommeDesInfaisabilites/= (double) NbCycles;
    if ( fabs ( MoyenneSommeDesInfaisabilites - BufferSommeDesInfaisabilites ) < 1.e-6 && 0 ) {
      /* Cyclage */
      #if VERBOSE_SPX
        printf( "Simplexe dual phase 1: suspiscion de cyclage => on initialise une phase de tirages au hasard\n");
        printf( "MoyenneSommeDesInfaisabilites %e BufferSommeDesInfaisabilites %e\n",MoyenneSommeDesInfaisabilites,BufferSommeDesInfaisabilites);	
      #endif

      printf( "Simplexe dual phase 1: suspiscion de cyclage => on initialise une phase de tirages au hasard\n");
      printf( "MoyenneSommeDesInfaisabilites %e BufferSommeDesInfaisabilites %e\n",MoyenneSommeDesInfaisabilites,BufferSommeDesInfaisabilites);
			
      Spx->ChoixDeVariableSortanteAuHasard = OUI_SPX;
      Spx->NombreMaxDeChoixAuHasard        = 2;   
      Spx->NombreDeChoixFaitsAuHasard      = 0;            
    }  
    NbCycles = 0;
    MoyenneSommeDesInfaisabilites = BufferSommeDesInfaisabilites;
    BufferSommeDesInfaisabilites  = 0.;
  }

  /* Calcul du vecteur V servant a determiner la variable sortante */
  SPX_DualPhase1CalculerV( Spx );
  /* Choix de la variable sortante */
  if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) {
    SPX_DualPhase1ChoixDeLaVariableSortanteAuHasard( Spx );
    Spx->NombreDeChoixFaitsAuHasard++;
  }
  else {
    SPX_DualPhase1ChoixDeLaVariableSortante( Spx );
  }
  if ( Spx->VariableSortante < 0 ) {
    /* Le probleme n'a pas de solution duale admissible */
    #if VERBOSE_SPX
      printf("Somme des infaisabilites duales %e\n",Spx->SommeDesInfaisabilitesDuales);
    #endif

    # ifdef UTILISER_BORNES_AUXILIAIRES
		    printf("NbInfaisabilitesDuales %d NbInfaisabilitesDualesALaPremiereIteration %d\n",
		            Spx->NbInfaisabilitesDuales, Spx->NbInfaisabilitesDualesALaPremiereIteration);
        if ( Spx->NbInfaisabilitesDuales < Spx->NbInfaisabilitesDualesALaPremiereIteration ) {
	        /* On utilise les bornes auxiliaires pour obtenir l'admissibilite duale */
          SPX_DualPhase1UtiliserLesBornesAuxiliaires( Spx );
          if ( Spx->NbInfaisabilitesDuales == 0 ) {
            /* La solution est duale admissible */
            #if VERBOSE_SPX
              printf( "Simplexe dual phase 1: admissibilite duale atteint en %d iterations\n",Spx->Iteration-1);
            #endif		
            Spx->LaBaseEstDualeAdmissible = OUI_SPX;
		      }
		 	  }
		    else Spx->LaBaseEstDualeAdmissible = NON_SPX;
		# else
        Spx->LaBaseEstDualeAdmissible = NON_SPX;
		# endif
		
    break;
  }

  if ( Spx->NombreDeChoixFaitsAuHasard >= Spx->NombreMaxDeChoixAuHasard ) { 
    Spx->ChoixDeVariableSortanteAuHasard = NON_SPX;
    Spx->NombreDeChoixFaitsAuHasard      = 0;
  }

  /* Calcul de la ligne du tableau pour la variable sortante */
  SPX_DualCalculerNBarreR( Spx, NON_SPX, &ControlerAdmissibiliteDuale ); 
  /* On verifie s'il faut refactoriser la base */
  FactoriserLaBase      = Spx->FactoriserLaBase; /* Sauvegarde du verdict en cours */
  Spx->FactoriserLaBase = NON_SPX;	
  SPX_DualVerifierErBMoinsUn( Spx );  
  if ( Spx->FactoriserLaBase == OUI_SPX ) {
    SPX_FactoriserLaBase( Spx );
    Spx->CalculerCBarre = OUI_SPX;
    goto Debut;
  }
  Spx->FactoriserLaBase = FactoriserLaBase;	
 
  /* Choix de la variable entrante */
  SPX_DualPhase1ChoixDeLaVariableEntrante( Spx );
  if ( Spx->VariableEntrante < 0 && Spx->SommeDesInfaisabilitesDuales < SEUIL_ADMISSIBILITE_DUALE_2 ) {
    /* Mais il faut tout de meme positionner correctement les variables */     
    for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
      PositionDeLaVariable = Spx->PositionDeLaVariable[Var];
      if ( PositionDeLaVariable == HORS_BASE_SUR_BORNE_INF ||
           PositionDeLaVariable == HORS_BASE_SUR_BORNE_SUP ||
           PositionDeLaVariable == HORS_BASE_A_ZERO ) { 
        if ( Spx->TypeDeVariable[Var]      == NON_BORNEE            ) Spx->PositionDeLaVariable[Var] = HORS_BASE_A_ZERO;
        else if ( Spx->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) Spx->PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
      }
    }    
    /* On accepte quand meme la solution */
    Spx->LaBaseEstDualeAdmissible = OUI_SPX;
    break;    
  }
  else if ( Spx->VariableEntrante < 0 ) {
    #if VERBOSE_SPX
      printf("Probleme de precision dans la phase 1 iteration %d de l'algorithme dual: il doit toujours y avoir une variable entrante\n",Spx->Iteration);
    #endif

    # ifdef UTILISER_BORNES_AUXILIAIRES
		  /* Si on utilise les bornes auxilaires, on modifie le numero d'iteration et on repart au
				 debit d'une afin d'activer la creation de bornes */				 
	    SPX_FactoriserLaBase( Spx );			
      Spx->CalculerCBarre = OUI_SPX;  
      Spx->Iteration = Spx->IterationPourBornesAuxiliaires;
      goto Debut;
		# else		
     /* Attention, il faut controler la precision: on tente au hasard 2 fois */
     /* Le choix au hasard n'est pas la bonne solution, le mieux est de choisir une autre variable
		    sortante parmis celles qui le peuvent, et ceci dans l'ordre de préséance */
      SPX_FactoriserLaBase( Spx );
      Spx->ChoixDeVariableSortanteAuHasard = OUI_SPX;
      Spx->NombreMaxDeChoixAuHasard        = 2;   
      Spx->NombreDeChoixFaitsAuHasard      = 0; 
      Spx->CalculerCBarre = OUI_SPX; /* Pour affiner le calcul des couts reduits */ 
      goto Debut;
		# endif
		
  }

  if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
    if ( Spx->NombreDeChangementsDeBase == Spx->CycleDeRefactorisation ) Spx->FactoriserLaBase = OUI_SPX;
  }

  SPX_CalculerABarreS( Spx ); /* Car on en a aussi besoin pour faire le changement de base et pour le steepest edge */  
  /* On verifie s'il faut refactoriser la base */
  SPX_VerifierABarreS( Spx );
  if ( Spx->FactoriserLaBase == NON_SPX ) {
    /* Si la factoristion de la base n'est pas encore prévue, on fait ici la mise a jour 
       des couts reduits. En effet, si une factorisation de la base est effectuee lors 
       du changement de base effectif, l'indicateur Spx->CalculerCBarre est positionne 
       a OUI_SPX ce qui provoque un calcul de couts reduits au debut du while */

    SPX_DualComparerABarreSEtNBarreR( Spx );
    if ( Spx->FaireChangementDeBase == NON_SPX ) goto Debut;

    /* Mise a jour des couts reduits */
    CntBase = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
    SPX_MettreAJourLesCoutsReduits( Spx ); 
  }
  
  /* Mise a jour des poids de la methode projected steepest edge */
  SPX_MajPoidsDualSteepestEdge( Spx );  

  /* On effectue le changement de base et eventuellement un refactorisation de la base */
  SPX_FaireLeChangementDeBase( Spx ); 

	/* Apres chaque chagement de base reussi on essaie de revenir au seuil de pivotage initial */
	if ( Spx->SeuilDePivotDual > VALEUR_DE_PIVOT_ACCEPTABLE ) {
    Spx->SeuilDePivotDual /= DIVISEUR_VALEUR_DE_PIVOT_ACCEPTABLE;		
	  if ( Spx->SeuilDePivotDual < VALEUR_DE_PIVOT_ACCEPTABLE ) Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;	
	}
	
}

Spx->ModifCoutsAutorisee = ModifCoutsAutorisee; 

/* Remarque: si, a ce stade, on stocke la base courante pour en faire une base de redemarrage 
   en cas de pivot nul, on n'est pas certain qu'elle soit inversible. On prefere conserver 
   la derniere base inversible obtenue meme si elle n'est pas completement duale realisable. 
   De toute facon, il y a le controle cyclique d'admissibilite duale dans la phase 2 du 
   simplexe dual */
/* Mais attention, il faut quand meme mettre l'indicateur de PositionDeLaVariable d'aplomb */

/* En realite c'est surtout utile pour le cas des bornes auxiliaires */
	 
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  PositionDeLaVariable = Spx->PositionDeLaVariableSV[Var];
  if ( PositionDeLaVariable == HORS_BASE_SUR_BORNE_INF ||
       PositionDeLaVariable == HORS_BASE_SUR_BORNE_SUP ||
       PositionDeLaVariable == HORS_BASE_A_ZERO ) goto OnCalcule; 
  continue;
  /* La variable est hors base */
  OnCalcule:
  if ( Spx->TypeDeVariable[Var]      == NON_BORNEE            ) Spx->PositionDeLaVariableSV[Var] = HORS_BASE_A_ZERO;
  else if ( Spx->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) Spx->PositionDeLaVariableSV[Var] = HORS_BASE_SUR_BORNE_INF;
}

return;

}
