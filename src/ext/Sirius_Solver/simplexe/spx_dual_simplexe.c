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

   FONCTION: Resolution de Min c x sous contrainte Ax = b par un  
             simplexe dual (forme revisee du simplexe) en matrices  
             creuses
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "bb_define.h"		        
# include "bb_fonctions.h"

# include "pne_define.h"		        

# define MX_DUAL_NON_BORNE 2 /*5*/

# define VERBOSE_SPX 0

/*----------------------------------------------------------------------------*/

void SPX_DualSimplexe( PROBLEME_SPX * Spx )
{
int Var; char ControleSolutionFait ; char BaseReconstruite; int AdmissibiliteRestauree;
int NbCyclesSansVerifDuale; char ConfirmationDualNonBorneEnCours; int FoisVerifDualNonBorne;
int NbControlesFinaux; int NombreDeCyclagesApresOptimalite; char EpurerLaBase; char Echec;
char OptimaliteRencontree; int NbIterDeFinition; char Reoptimiser; char VerificationCyclage;
int IterationStockageCout1; int IterationStockageCout2; int DeltaIterationDeStockage;
double StockageCout1; double StockageCout2; double DeltaStockageCout; int SeuilPourRedemarrageBaseRacine;
char ControlerAdmissibiliteDuale; PROBLEME_PNE * Pne; BB * Bb; int i; char PresenceSeuilPourRedemarrageBaseRacine;
int NeutralisationVerificationDuCyclage;

#if VERBOSE_SPX
  printf("Entree dans l algorithme dual du simplexe\n"); 
#endif

/* Ensuite Bs sera toujours remis a 0 des qu'on aura fini de s'en servir */
memset( (char *) Spx->Bs , 0 , Spx->NombreDeContraintes * sizeof( double ) );

SeuilPourRedemarrageBaseRacine = Spx->NombreMaxDIterations << 1;
PresenceSeuilPourRedemarrageBaseRacine = NON_SPX;
Pne = (PROBLEME_PNE *) Spx->ProblemePneDeSpx;
if ( Pne != NULL ) {
  Bb = (BB *) Pne->ProblemeBbDuSolveur;
  if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
    if ( Bb->BaseDisponibleAuNoeudRacine == OUI ) {
		  if ( Bb->NombreDeSimplexes != 0 ) {   
			  SeuilPourRedemarrageBaseRacine = (int) ceil( (double) Bb->SommeDuNombreDIterations / (double) Bb->NombreDeSimplexes );
		    SeuilPourRedemarrageBaseRacine *= 10;
        PresenceSeuilPourRedemarrageBaseRacine = OUI_SPX;				
			}
		}
	}	
}

SPX_InitialiserLesIndicateursHyperCreux( Spx );

Spx->FaireDuRaffinementIteratif = 0;

DeltaIterationDeStockage = 150 /*100*/;
NeutralisationVerificationDuCyclage = 1000;
DeltaStockageCout        = 1.e-6;
StockageCout1 = 0.0;
StockageCout1 = 0.0;
VerificationCyclage = NON_SPX;
IterationStockageCout1 = -1;
IterationStockageCout2 = -1;
BaseReconstruite = NON_SPX;

Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;
  
Spx->NbBoundFlipIterationPrecedente = 0;

Spx->TypeDeStockageDeABarreS = VECTEUR_SPX;
	
Spx->NombreDeBornesAuxiliairesUtilisees      = 0;
Spx->IterationPourBornesAuxiliaires          = ITERATION_POUR_BORNES_AUXILIAIRES;
Spx->CoeffMultiplicateurDesBornesAuxiliaires = 1;

Spx->LesCoutsOntEteModifies = NON_SPX;
Spx->ModifCoutsAutorisee = OUI_SPX;
Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori = 1.;

OptimaliteRencontree = NON_SPX;
Reoptimiser          = NON_SPX;

Spx->NbCyclesSansControleDeDegenerescence = (int) floor( 0.5 * Spx->CycleDeControleDeDegenerescence );
/* En mode branching il faut faire le controle rapidement */
Spx->NbCyclesSansControleDeDegenerescence = Spx->CycleDeControleDeDegenerescence - 1;

/* Initialisation des poids de la methode projected steepest edge */
if ( Spx->LeSteepestEdgeEstInitilise == NON_SPX ) SPX_InitDualPoids( Spx );  

/* Que la base soit fournie ou non, on appelle toujours ce sous-programme. S'il s'avere 
   que la base fournie etait duale realisable, il se contente de repositionner 
   les variables et se termine tout de suite */ 
SPX_DualPhase1Simplexe( Spx );
if ( Spx->LaBaseEstDualeAdmissible == NON_SPX ) { 
  #if VERBOSE_SPX
    printf("Pas de base duale admissible disponible\n");
  #endif	
  Spx->YaUneSolution = NON_SPX;
  return;
}

ControleSolutionFait  = NON_SPX;

/* Et c'est parti dans les iterations */
  
Spx->EcrireLegendePhase2 = OUI_SPX;
 
Spx->CalculerBBarre = OUI_SPX; /* OUI_SPX pour la premiere iteration */

Spx->PhaseEnCours      = PHASE_2;
Spx->Iteration         = 0;
NbCyclesSansVerifDuale = 0;

Spx->ChoixDeVariableSortanteAuHasard = NON_SPX;
Spx->NombreMaxDeChoixAuHasard = 1;
Spx->NombreDeChoixFaitsAuHasard = 0;

Spx->ProchaineIterationDeReinitDesCouts = 1000;

ConfirmationDualNonBorneEnCours = NON_SPX;
NombreDeCyclagesApresOptimalite = 0;

NbControlesFinaux = 0;

EpurerLaBase = OUI_SPX;

NbIterDeFinition = 0;

Spx->YaUneSolution = NON_SPX;

FoisVerifDualNonBorne = 0;

while ( 1 ) {

  Debut:
	
  Spx->Iteration++;	  		
	Spx->FaireChangementDeBase = OUI_SPX;
	Spx->NbBoundFlip = 0;
	
  if ( Spx->FaireDuRaffinementIteratif > 0 ) {
	  Spx->FaireDuRaffinementIteratif--;
		
		if ( Spx->FaireDuRaffinementIteratif > 0 ) {
      Spx->CalculErBMoinsUnEnHyperCreux       = NON_SPX;
      Spx->CountEchecsErBMoins                = 0;                
      Spx->CalculABarreSEnHyperCreux         = NON_SPX;
      Spx->CountEchecsABarreS                = 0;                
		}
		else {
      SPX_InitialiserLesIndicateursHyperCreux( Spx );
		}
		
	}
	
  #if VERBOSE_SPX
    if ( Spx->FaireDuRaffinementIteratif > 0 ) {
	    printf("RaffinementIteratif %d demande a l'iteration %d\n",Spx->FaireDuRaffinementIteratif,Spx->Iteration);
	  }
	# endif
		
  if ( Spx->DureeMaxDuCalcul > 0 ) SPX_ControleDuTempsEcoule( Spx );
		
  if ( Spx->Iteration > Spx->NombreMaxDIterations ) { /* Clause de sortie */			
	  if ( OptimaliteRencontree == OUI_SPX ) {
		  /* Si on a deja trouve un optimum mais avec bruitage, cela veut dire qu'on a du mal a
		     se sortir de la degenerescence mais qu'on a une solution realisable et presque optimale.
			   On verifie alors que la solution n'est pas trop mauvaise. Si c'est le cas, on dit qu'il
			   y a une solution */
      /* Calcul des couts reduits */
      SPX_CalculerPi( Spx );             
      SPX_CalculerLesCoutsReduits( Spx ); 
      /* Calcul de BBarre */  
      SPX_CalculerBBarre( Spx );
      /* Choix de la variable sortante */
      SPX_DualChoixDeLaVariableSortante( Spx );						
		  if ( Spx->VariableSortante < 0 ) Spx->YaUneSolution = OUI_SPX;
		  else if ( VerificationCyclage == NON_SPX ) {			
		    Var = Spx->VariableSortante;
        if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) {
			    if ( Spx->BBarre[Spx->ContrainteDeLaVariableEnBase[Var]] - Spx->Xmax[Var] < 100. * SEUIL_DE_VIOLATION_DE_BORNE ) Spx->YaUneSolution = OUI_SPX;
			  	else Spx->YaUneSolution = NON_SPX;
			  }
			  else if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
			    if ( Spx->Xmin[Var] - Spx->BBarre[Spx->ContrainteDeLaVariableEnBase[Var]] < 100. * SEUIL_DE_VIOLATION_DE_BORNE ) Spx->YaUneSolution = OUI_SPX;
				  else Spx->YaUneSolution = NON_SPX;			
        }
			  else Spx->YaUneSolution = NON_SPX;			
		  }
      break;			
    }
    else if ( Spx->Iteration > (Spx->NombreMaxDIterations << 3) && 0 ) {
		  /* Ailleurs on met Spx->Iteration = 10 * Spx->NombreMaxDIterations pour forcer l'arret */
      Spx->YaUneSolution = NON_SPX;
      break;
    }		
    else if ( VerificationCyclage == NON_SPX ) {	
      # if VERBOSE_SPX
		    printf("On positionne VerificationCyclage a OUI_SPX iteration %d  NombreMaxDIterations %d\n",Spx->Iteration,Spx->NombreMaxDIterations);
			# endif
	    /* On verifie qu'il n'y a aucun progres dans la fonction cout. S'il y en a, on continue les iterations.
		     Pour cela, on calcule le cout entre k iterations */
      VerificationCyclage = OUI_SPX;
			IterationStockageCout1 = Spx->Iteration;
			IterationStockageCout2 = IterationStockageCout1 + DeltaIterationDeStockage;
			/* Spx->ModifCoutsAutorisee = NON_SPX; */ /* Il vaut mieux ne pas le faire car alors on favorise le cyclage */
		}
  }

	/* Si on est dans les controles finaux et qu'on a du mal a converger, on reactive le bruitage
	   anti degenerescence */
  if ( NbControlesFinaux > 0 ) {
	  NbIterDeFinition++;		
	  i = 0.1 * Spx->NombreDeContraintes;
	  if ( i < 1000 ) i = 1000;
	  if ( NbIterDeFinition > i && Spx->ModifCoutsAutorisee == NON_SPX && 0 ) {
		  
		  printf("      Reactivation du bruitage apres convergence car trop d'iterations CoefficientPourLaValeurDePerturbationDeCoutAPosteriori %e\n",
			              Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori);
		  printf("      **************************************************************** \n");

		  NbIterDeFinition     = 0;
			/*NbControlesFinaux    = 0;*/ /* Non car risque de cyclage */
			ControleSolutionFait = NON_SPX;
			Spx->ModifCoutsAutorisee = OUI_SPX;
			/*
			Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori *= 0.1;
			if ( Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori < 1.e-3 ) {
			  Spx->ModifCoutsAutorisee = NON_SPX;
			}
			*/
		  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) Spx->CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT; 
    }
	}
	
  /* Calcul des couts reduits */
  if ( Spx->CalculerCBarre == OUI_SPX ) {			
    SPX_CalculerPi( Spx );              /* Calcul de Pi = c_B * B^{-1} */
    SPX_CalculerLesCoutsReduits( Spx ); /* Calcul de CBarre = c_N - < Pi , N > */ 
    Spx->CalculerCBarre = NON_SPX; 
  }
 
  /* On essaie cycliquement de restaurer les couts initiaux */
  SPX_ReinitialiserLesCoutsNatifSiCestPossible( Spx );
	
  /* S'il y a des bornes auxiliaires, on essaie cycliquement de remettre les bornes initiales tout
	   en restant dual realisable */
	# ifdef UTILISER_BORNES_AUXILIAIRES
	if ( Spx->NombreDeBornesAuxiliairesUtilisees > 0 ) {
    if ( Spx->Iteration % CYCLE_POUR_SUPPRESSION_DES_BORNES_AUXILIAIRES == 0 ) {
		  SPX_DualSupprimerLesBornesAuxiliaires( Spx );
		}
	}
	# endif
	
  /* Calcul de BBarre c'est a dire B^{-1} * b */  
  if ( Spx->CalculerBBarre == OUI_SPX ) {	  
    SPX_CalculerBBarre( Spx );
    Spx->CalculerBBarre = NON_SPX;		
  }
	
  NbCyclesSansVerifDuale++;
  if ( NbCyclesSansVerifDuale == CYCLE_DE_VERIF_ADMISSIBILITE_DUALE ) {
	  /* Ici l'utilite du calcul du cout et le calcul de X en fonction de la position est de permettre l'affichage
		   du deroulement du calcul */
    SPX_CalculDuCout( Spx ); 	
		/* On verifie l'admissibilite avec les couts courants i.e. perturbes */
    SPX_VerifierAdmissibiliteDuale( Spx , &AdmissibiliteRestauree );
    NbCyclesSansVerifDuale = 0;
    if ( AdmissibiliteRestauree == OUI_SPX ) {
			# if VERBOSE_SPX
        printf( "Controle d'admissibilite duale iteration %d : la base n'etait pas duale realisable\n",Spx->Iteration); 
      # endif
			Spx->CalculerBBarre = OUI_SPX;
      Spx->CalculerCBarre = OUI_SPX; 
      goto Debut;
    }		
  }
		
  /* Verification: si le cout courant depasse le cout max fourni, alors on
     arrete les calculs car le cout courant est un minorant du cout optimal */
  if ( Spx->UtiliserCoutMax == OUI_SPX && Spx->Iteration > 1 ) {
	      
	  /*SPX_CalculDuCout( Spx );*/
    /* Test d'une autre methode de calcul du cout */
    SPX_CalculDuCoutSimplifie( Spx );
    /* Fin test d'une autre methode de calcul du cout */
    if ( Spx->Cout > Spx->CoutMax ) {		
      if ( Spx->LesCoutsOntEteModifies == OUI_SPX ) {
        /* Si le flag a ete leve, on restaure les couts et on refait un calcul */
        memcpy( (char *) Spx->C, (char *) Spx->Csv, Spx->NombreDeVariables * sizeof( double ) );
				
        /* Il faut controler l'admissibilite duale de la solution. Si elle n'est pas duale admissible on repositionne
	         les variables hors base pour que ce soit le cas et on repart dans les iteration */
        Spx->ModifCoutsAutorisee    = NON_SPX;
        Spx->LesCoutsOntEteModifies = NON_SPX;   
        SPX_CalculerPi( Spx );              
        SPX_CalculerLesCoutsReduits( Spx );
        SPX_CalculerBBarre( Spx );				
        Spx->CalculerCBarre = NON_SPX;
      }

			/* Dans tous les cas on reverifie a nouveau le depassement de CoutMax */
	    /*SPX_CalculDuCout( Spx );*/
	    SPX_CalculDuCoutSimplifie( Spx );
      if ( Spx->Cout <= Spx->CoutMax ) goto FinTestDepassementCoutMax;				
			
      /* Controle d'admissibilite duale */
      if ( NbControlesFinaux < NOMBRE_MAX_DE_CONTROLES_FINAUX ) { /* C'est pour eviter un cyclages pour des questions d'epsilon */			
        NbControlesFinaux++;				
        SPX_VerifierAdmissibiliteDuale( Spx , &AdmissibiliteRestauree );
        NbCyclesSansVerifDuale = 0;
        if ( AdmissibiliteRestauree == OUI_SPX ) { 
          Spx->CalculerBBarre = OUI_SPX; 
          Spx->CalculerCBarre = OUI_SPX; 
          goto Debut; 
        }	
      }
        
      Spx->YaUneSolution = NON_SPX;

      Pne = (PROBLEME_PNE *) Spx->ProblemePneDeSpx;
      if ( Pne != NULL ) {
        Bb = (BB *) Pne->ProblemeBbDuSolveur;          
        BB_LeverLeFlagDeSortieDuSimplexeParDepassementDuCoutMax( Bb ); 
        #if VERBOSE_SPX 
          printf( "Arret du Simplexe dual par depassement du cout max, iterations %d  Cout %e CoutMax %e\n",Spx->Iteration-1,Spx->Cout,Spx->CoutMax);
        #endif		  
      } 
      break;
    } 
  }
	FinTestDepassementCoutMax:
  /* On a depasse le nombre max d'iteration et on est en train de verifier s'il y a cyclage */
  if ( VerificationCyclage == OUI_SPX ) {
	  if ( Spx->Iteration == IterationStockageCout1 )  {
		  /*SPX_CalculDuCout( Spx );*/
      SPX_CalculDuCoutSimplifie( Spx );		
			StockageCout1 = Spx->Cout;
		}
		else if ( Spx->Iteration == IterationStockageCout2 ) {
		  /*SPX_CalculDuCout( Spx );*/
      SPX_CalculDuCoutSimplifie( Spx );		
      StockageCout2 = Spx->Cout;
			if ( StockageCout2 < StockageCout1 + DeltaStockageCout ) {
			  /* Arret confirme */
        Spx->YaUneSolution = NON_SPX;		
        break;				
			}
			else {
			  /* L'arret est en sursis */
        VerificationCyclage = NON_SPX;
        Spx->NombreMaxDIterations += NeutralisationVerificationDuCyclage; /* Afin de neutraliser la verification du cycale pendant un certain temps */				
        # if VERBOSE_SPX
				  printf("On positionne VerificationCyclage a NON_SPX  iteration %d\n",Spx->Iteration);
        # endif				
			}                       
    }
	}

  /* Si on est en mode branch and bound et qu'on a du mal a trouver l'optimum, on repart
	   de la base optimale du noeud racine sans les coupes (les variables d'ecart des coupes
		 sont alors positionnees en base */
  if ( Spx->Iteration > SeuilPourRedemarrageBaseRacine && PresenceSeuilPourRedemarrageBaseRacine == OUI_SPX ) {
	  if ( BaseReconstruite == NON_SPX ) {		
      #if VERBOSE_SPX
        printf(" \n");
        printf("------> Convergence lente, on tente de repartir d'une BASE du noeud racine\n");
        printf(" \n");
      #endif			
      /* Attention il faut avoir resolu le noeud racine sans coupes */
			SPX_DualReconstruireUneBase( Spx , &Echec );
			if ( Echec == OUI_SPX ) {
				/* On continue avec la base courante car on ne peut pas faire mieux */
				goto Debut;
			}
			BaseReconstruite = OUI_SPX; 			
      SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );
			SPX_InitialiserLesIndicateursHyperCreux( Spx );

			printf("\n 1- On autorise a nouveau le bruitage des couts  !!!!! \n");
			
			Spx->ModifCoutsAutorisee = OUI_SPX;
      Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori = 1.;
			
      /* On remet les vrais couts */
			
      memcpy( (char *) Spx->C, (char *) Spx->Csv, Spx->NombreDeVariables * sizeof( double ) );
      Spx->LesCoutsOntEteModifies = NON_SPX;
			
      for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) Spx->CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;			
			
			Spx->YaUneSolution = OUI_SPX; /* Car c'est positionne a NON_SPX en cas de probleme */
      SPX_FactoriserLaBase( Spx );
      if ( Spx->YaUneSolution == NON_SPX ) {
        #if VERBOSE_SPX
			    printf("Base non inversible ou probleme pour trouver une base duale admissible\n");
        #endif
        break;				
      }
			Spx->YaUneSolution = NON_SPX; /* Car le positionnement a OUI_SPX n'etait que pour la factorisation */
      SPX_InitDualPoids( Spx );  			
      SPX_DualPhase1Simplexe( Spx );
      if ( Spx->LaBaseEstDualeAdmissible == NON_SPX ) { 
        #if VERBOSE_SPX
          printf("Pas de base duale admissible disponible\n");
        #endif
        Spx->YaUneSolution = NON_SPX;
        break;
      }			
      Spx->CalculerBBarre = OUI_SPX; 
      Spx->CalculerCBarre = OUI_SPX;
		  goto Debut;		
		}		
	}
	
  RechercheVariableSortante:
	
  /* Choix de la variable sortante */	
  if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) {
    SPX_DualChoixDeLaVariableSortanteAuHasard( Spx ); 
    Spx->NombreDeChoixFaitsAuHasard++;
    if ( Spx->VariableSortante < 0 ) {
		  Spx->ChoixDeVariableSortanteAuHasard = NON_SPX;
      Spx->NombreDeChoixFaitsAuHasard      = 0;
			goto RechercheVariableSortante;
		}				
  }
  else {
    SPX_DualChoixDeLaVariableSortante( Spx );
  } 

  if ( Spx->NombreDeChoixFaitsAuHasard >= Spx->NombreMaxDeChoixAuHasard ) { 
    Spx->ChoixDeVariableSortanteAuHasard = NON_SPX;
    Spx->NombreDeChoixFaitsAuHasard      = 0;
  }
	
  if ( Spx->VariableSortante < 0 ) {
    /* Si on est dans un contexte de base reduite, il faut controler les variables basiques hors base reduite.
		   Si elle sont toutes admissible, c'est gagne.
			 Si certaines ne sont pas admissibles on les reintegre dans une base reduite et on recommence */
		# if VERBOSE_SPX == 1
		  printf("Plus de variable sortante  Spx->UtiliserLaBaseReduite %d\n",Spx->UtiliserLaBaseReduite);
		# endif
    if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
      SPX_TestPassageBaseReduiteBaseComplete( Spx );	
      goto Debut;									
		}
			 			
    OptimaliteAtteinte:
    /* Optimalite atteinte */		
    OptimaliteRencontree = OUI_SPX;			
    SPX_DualControlerOptimalite( Spx, &NbControlesFinaux, &ControleSolutionFait );		
    if ( Spx->YaUneSolution == OUI_SPX ) break;
		/* Sinon on repart dans les iterations */
    NbCyclesSansVerifDuale = 0;

    /* Au bout d'un certain nombre d'echecs, on repart de la base racine */
		if ( NbControlesFinaux == SEUIL_POUR_RECONSTRUCTION_BASE && BaseReconstruite == NON_SPX ) {
      #if VERBOSE_SPX
        printf(" \n");
        printf("------> On repart d'une BASE du noeud racine\n");
        printf(" \n");
      #endif			
      /* Attention il faut avoir resolu le noeud racine sans coupes */
			SPX_DualReconstruireUneBase( Spx , &Echec );
			if ( Echec == OUI_SPX ) {
				/* On continue avec la base courante car on ne peut pas faire mieux */
				goto Debut;
			}
			BaseReconstruite = OUI_SPX; 			
      SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );
			SPX_InitialiserLesIndicateursHyperCreux( Spx );
			
			Spx->ModifCoutsAutorisee = OUI_SPX;
			Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori *= 0.1;

      /* On remet les vrais couts */
			/*
      memcpy( (char *) Spx->C, (char *) Spx->Csv, Spx->NombreDeVariables * sizeof( double ) );
      Spx->LesCoutsOntEteModifies = NON_SPX;			
			*/
      for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) Spx->CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;
			
			Spx->YaUneSolution = OUI_SPX; /* Car c'est positionne a NON_SPX en cas de probleme */
      SPX_FactoriserLaBase( Spx );
      if ( Spx->YaUneSolution == NON_SPX ) {
        #if VERBOSE_SPX
			    printf("Base non inversible ou probleme pour trouver une base duale admissible\n");
        #endif
        break;				
      }
			Spx->YaUneSolution = NON_SPX;
      SPX_InitDualPoids( Spx );  			
      SPX_DualPhase1Simplexe( Spx );
      if ( Spx->LaBaseEstDualeAdmissible == NON_SPX ) { 
        #if VERBOSE_SPX
          printf("Pas de base duale admissible disponible\n");
        #endif
        Spx->YaUneSolution = NON_SPX;
        break;
      }			
      Spx->CalculerBBarre = OUI_SPX; 
      Spx->CalculerCBarre = OUI_SPX;			
		}		
		goto Debut;
		  
  }

	RechercheVariableEntrante:
	
  SPX_DualCalculerNBarreR( Spx, OUI_SPX, &ControlerAdmissibiliteDuale );
  if ( ControlerAdmissibiliteDuale == OUI_SPX ) {
		/* On a constate trop de derive dans les couts reduits */
	  /* Ici l'utilite du calcul du cout et le calcul de X en fonction de la position est de permettre l'affichage
		   du deroulement du calcul */
    /*SPX_CalculDuCout( Spx );*/ 	
		/* On verifie l'admissibilite avec les couts courants i.e. perturbes */
    SPX_VerifierAdmissibiliteDuale( Spx , &AdmissibiliteRestauree );
    NbCyclesSansVerifDuale = 0;
    if ( AdmissibiliteRestauree == OUI_SPX ) {
			# if VERBOSE_SPX
        printf( "Controle d'admissibilite duale iteration %d : la base n'etait pas duale realisable\n",Spx->Iteration); 
      # endif
			Spx->CalculerBBarre = OUI_SPX;
      Spx->CalculerCBarre = OUI_SPX; 
      goto Debut;
    }		
  }			
	
  SPX_DualVerifierErBMoinsUn( Spx );
  if ( Spx->FactoriserLaBase == OUI_SPX ) SPX_FactoriserLaBase( Spx );
	if ( Spx->FaireChangementDeBase == NON_SPX ) {
	  # if VERBOSE_SPX
		  printf("Iteration %d Pas de changement de base a cause de DualVerifierErBMoinsUn\n",Spx->Iteration);
		# endif		
		goto Debut;
	}	
  if ( Spx->NombreDeVariablesATester <= 0 ) {
	  if ( Spx->SeuilDePivotDual > VALEUR_DE_PIVOT_ACCEPTABLE ) {
      Spx->SeuilDePivotDual /= DIVISEUR_VALEUR_DE_PIVOT_ACCEPTABLE;		
	    if ( Spx->SeuilDePivotDual < VALEUR_DE_PIVOT_ACCEPTABLE ) Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;
			/* Apres avoir ajuste le seuil, on recherche a nouveau une variable entrante */			
      if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) goto RechercheVariableSortante;			
      goto RechercheVariableEntrante;		
	  }
    /* On ne cherche pas a confirmer l'absence de solution tant qu'on n'est pas dans une phase normale du simplexe */				
		if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) goto RechercheVariableSortante;
					
		if ( FoisVerifDualNonBorne > MX_DUAL_NON_BORNE ) ConfirmationDualNonBorneEnCours = OUI_SPX;
    SPX_DualControleDualNonBorne( Spx, &ConfirmationDualNonBorneEnCours );
		/* Il n'est pas clair si on doit ou non remettre les couts initiaux et
		   desactiver le bruitage anti degenerescence */			 
    if ( ConfirmationDualNonBorneEnCours == OUI_SPX ) goto Debut;
		
    if ( Spx->AdmissibilitePossible == OUI_SPX ) {  
      #if VERBOSE_SPX 
        printf("Spx->AdmissibilitePossible = OUI_SPX => on considere quand meme qu'il y a une solution admissible\n");
      #endif			
		  /* Mieux vaut restaurer les couts et refaire un calcul */
      Spx->VariableSortante = -1;
      goto OptimaliteAtteinte;
    } 
    else { 
      #if VERBOSE_SPX 
        printf("Spx->AdmissibilitePossible = NON_SPX\n");   
      #endif
      Spx->YaUneSolution = NON_SPX;
      break;  
    }
		
  }

	/* Ci-dessous pour mise au point uniquement */
	/*
	if ( Spx->PremierSimplexe != OUI_SPX ) {
 		if ( Spx->Iteration > 2000 ) {
	    printf("Ecriture des donnees iteration %d\n",Spx->Iteration);
      Spx_EcrireJeuDeDonneesLineaireAuFormatMPS( Spx );
      exit(0);
	 	}
	}
	*/
	
	/* Si on etait en train de confirmer un dual non borne, on avait remis les couts initiaux et interdit le bruitage des couts.
	   Si on en arrive la, c'est qu'on a pu s'en tirer donc on remet le bruitage des couts en service */
	/* Experimentalement, il semble preferable de ne pas re bruiter les couts et d'en reste a la confirmation
	   du dual non borne */		
	if ( ConfirmationDualNonBorneEnCours == OUI_SPX ) {
    Spx->ModifCoutsAutorisee        = OUI_SPX;
    ConfirmationDualNonBorneEnCours = NON_SPX;
		FoisVerifDualNonBorne++;		
		for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) Spx->CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT; 
	}

  /* Choix de la variable entrante */
  SPX_DualChoixDeLaVariableEntrante( Spx );	
  if ( Spx->VariableEntrante < 0 ) {  		
	  /* Par precaution */
    Spx->YaUneSolution = NON_SPX;
    break;               
  }

  if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
    if ( Spx->NombreDeChangementsDeBase == Spx->CycleDeRefactorisation ) Spx->FactoriserLaBase = OUI_SPX;
  }  

  SPX_CalculerABarreS( Spx );  /* On en a aussi besoin pour le steepest edge et pour les controles et aussi pour la mise a jour de BBarre */
  /* La verification de ABarreS peut entrainer une demande de factorisation de la base */
	SPX_VerifierABarreS( Spx );
  if ( Spx->FaireChangementDeBase == NON_SPX ) {
	  # if VERBOSE_SPX
		  printf("Iteration %d Pas de changement de base a cause de VerifierABarreS\n",Spx->Iteration);
	  # endif
		/* Attention a remettre les bound flip */
    if ( Spx->FactoriserLaBase == OUI_SPX ) SPX_FactoriserLaBase( Spx );
		Spx->CalculerBBarre = OUI_SPX;
		Spx->CalculerCBarre = OUI_SPX;				
    goto Debut;
	}
   
  SPX_DualComparerABarreSEtNBarreR( Spx );

  #if VERBOSE_SPX
    if ( Spx->FaireChangementDeBase == NON_SPX ) {
      printf(" changement de base refuse iteration %d \n",Spx->Iteration);
    }
	#endif
  
  if ( Spx->FaireChangementDeBase == NON_SPX ) {
	  # if VERBOSE_SPX
	    printf("Iteration %d Pas de changement de base a cause de DualComparerABarreSEtNBarreR\n",Spx->Iteration);
	  #endif	 
    if ( Spx->FactoriserLaBase == OUI_SPX ) SPX_FactoriserLaBase( Spx );
		Spx->CalculerBBarre = OUI_SPX;
		Spx->CalculerCBarre = OUI_SPX;		
	  goto Debut;
	}
	
  /* Mise a jour des poids de la methode projected steepest edge */
  SPX_MajPoidsDualSteepestEdge( Spx );	
  
  if ( Spx->FactoriserLaBase == NON_SPX ) {
    SPX_MettreAJourLesCoutsReduits( Spx );   		
	  if ( Spx->FaireMiseAJourDeBBarre == OUI_SPX && ControleSolutionFait == NON_SPX ) { 
      SPX_MettreAJourBBarre( Spx ); 
      Spx->CalculerBBarre = NON_SPX;
    }
    else Spx->CalculerBBarre = OUI_SPX;	
  }
	
  SPX_FaireLeChangementDeBase( Spx );
	
	/* Apres chaque changement de base reussi on essaie de revenir au seuil de pivotage initial */
	if ( Spx->SeuilDePivotDual > VALEUR_DE_PIVOT_ACCEPTABLE ) {
    Spx->SeuilDePivotDual /= DIVISEUR_VALEUR_DE_PIVOT_ACCEPTABLE;		
	  if ( Spx->SeuilDePivotDual < VALEUR_DE_PIVOT_ACCEPTABLE ) Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;	
	}
	
}

OptimaliteRencontree = NON_SPX;		

Spx->FaireDuRaffinementIteratif = 0;
Spx->CycleDeRefactorisation = CYCLE_DE_REFACTORISATION_DUAL;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX && Spx->YaUneSolution == OUI_SPX ) {
	Spx->YaUneSolution = OUI_SPX;
  SPX_TestPassageBaseReduiteBaseComplete( Spx );	
  goto Debut;									
}

# ifdef UTILISER_BORNES_AUXILIAIRES

  SPX_DualControlerLesBornesAuxiliaires( Spx, &Reoptimiser );
  if ( Reoptimiser == OUI_SPX ) {    
	  # if VERBOSE_SPX
      printf("On relance une phase 2 apres DualControlerLesBornesAuxiliaires\n");
    # endif
		ControleSolutionFait  = NON_SPX;
    NbCyclesSansVerifDuale = 0;
    ConfirmationDualNonBorneEnCours = NON_SPX;
    NbControlesFinaux = 0;
    NbIterDeFinition = 0;
    Spx->CycleDeRefactorisation = CYCLE_DE_REFACTORISATION_DUAL;
    Spx->CalculerBBarre = OUI_SPX; 
    Spx->CalculerCBarre = OUI_SPX; 			
    Spx->LesCoutsOntEteModifies = NON_SPX;		
    Spx->ModifCoutsAutorisee = NON_SPX /*OUI_SPX*/;
    Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori = 1.;
		
		Spx->ForcerUtilisationDeLaBaseComplete = 0;
		Spx->InitBaseReduite = OUI_SPX; 
		
		FoisVerifDualNonBorne = 0;
    for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) Spx->CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;		
	  goto Debut;		
  }
		
# endif

if ( Spx->Contexte != SIMPLEXE_SEUL && Spx->YaUneSolution == OUI_SPX ) {
  SPX_DualEpurerLaBaseSimplexe( Spx, &NombreDeCyclagesApresOptimalite, &EpurerLaBase, &Echec );																
  if ( Echec == OUI_SPX ) goto Debut;	
}

/* A faire si on n'appelle pas DualEpurerLaBaseSimplexe en branch and bound */
/*
if ( Spx->Contexte != SIMPLEXE_SEUL && Spx->YaUneSolution == OUI_SPX ) {
  SPX_FactoriserLaBase( Spx );
}
*/
/*printf("Fin a l'iteration %d YaUneSolution %d\n",Spx->Iteration,(int) Spx->YaUneSolution);*/

/* En test: pivotages degeneres pour obtenir une solution entiere */
/*if ( Spx->YaUneSolution == OUI_SPX ) SPX_DualPivotagesComplementaires( Spx );*/

return; 
}


