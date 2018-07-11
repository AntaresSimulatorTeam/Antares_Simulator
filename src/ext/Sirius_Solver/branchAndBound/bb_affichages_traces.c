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

   FONCTION: Resolution d'un probleme relaxe  
        
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"
#include "pne_fonctions.h"
											  
#define TAILLE_DU_MEGA_OCTET  1048576 /* 1024 * 1024 */

/*---------------------------------------------------------------------------------------------------------*/

void BB_AfficherLesTraces( BB * Bb, NOEUD * NoeudCourant )
{
double TempsEcoule; ldiv_t QuotientEtReste; PROBLEME_PNE * Pne; double X; NOEUD * Noeud;
int AverageG; int AverageI; int AverageK; char Tracer; 
# if VERBOSE_BB
int cpt; int i; int * pt1 ; char * pt2;
# endif

if ( NoeudCourant == Bb->NoeudRacine ) {
  Bb->TempsDuDernierAffichage = 0;
  Bb->NombreDeProblemesDepuisLeDernierAffichage = 0;
  Bb->NombreDAffichages = 0;
}

if ( Bb->NombreDeProblemesResolus > 0 ) {
 
  Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;	
  time( &(Pne->HeureDeCalendrierCourant) );
  TempsEcoule = difftime( Pne->HeureDeCalendrierCourant , Pne->HeureDeCalendrierDebut );
  if ( TempsEcoule <= 0.0 ) TempsEcoule = 0.0;  
  
  QuotientEtReste = ldiv( (int)TempsEcoule , (int)CYCLE_DINFORMATIONS ); 

  if ( Bb->TempsDexecutionMaximum > 0 ) {
    if ( TempsEcoule >= Bb->TempsDexecutionMaximum ) {					
      Bb->ArreterLesCalculs = OUI;
			if ( Bb->AffichageDesTraces == OUI ) printf("Stopping calculation because time limit reached.\n");
    }
  }

  if ( Bb->AffichageDesTraces == OUI && 
      ( (QuotientEtReste.rem == 0 && QuotientEtReste.quot != Bb->TempsDuDernierAffichage) || 
        Bb->NombreDeProblemesDepuisLeDernierAffichage >= CYCLE_DINFORMATIONS_EN_NOMBRE_DE_PROBLEMES || 
        Bb->ForcerAffichage == OUI ) ) {
    Bb->TempsDuDernierAffichage = QuotientEtReste.quot;
    Bb->NombreDeProblemesDepuisLeDernierAffichage = 0;

    BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT );
    
    if ( Bb->NombreDAffichages == 0 || Bb->NombreDAffichages >= CYCLE_DAFFICHAGE_LEGENDE ) {    
      Bb->NombreDAffichages = 0;
      printf(" "); 
      printf("|  Nodes  |");      
      printf(" Depth  (max)  |");      
      printf(" Active nodes |");
      printf(" Sol |");      
      printf("     Best Sol.   |");     
      printf("    Best bound   |");
      printf("    Gap %%   |");       
      printf("  Seconds |");
      printf(" Cuts: average in use (over) |");			
      printf("\n");
    } 
    Bb->NombreDAffichages++;

    printf(" "); 
    printf("| %7d |",Bb->NombreDeProblemesResolus); 
    printf(" %5d (%5d) |",NoeudCourant->ProfondeurDuNoeud,Bb->NombreDeVariablesEntieresDuProbleme);		
    printf(" %6d       |",Bb->NbNoeudsOuverts);    
    printf(" %3d |",Bb->NombreDeSolutionsEntieresTrouvees);
		
    if ( Bb->NombreDeSolutionsEntieresTrouvees > 0 ) {
      if ( Bb->ForcerAffichage == OUI ) {
			  if ( Bb->CoutDeLaMeilleureSolutionEntiere >= 0.0 ) {
          printf("* %12.8e |",Bb->CoutDeLaMeilleureSolutionEntiere);
				}
				else {
          printf("* %11.7e |",Bb->CoutDeLaMeilleureSolutionEntiere);
				}
      }
      else {
			  if ( Bb->CoutDeLaMeilleureSolutionEntiere >= 0.0 ) {
          printf("  %12.8e |",Bb->CoutDeLaMeilleureSolutionEntiere);
				}
				else {
          printf("  %11.7e |",Bb->CoutDeLaMeilleureSolutionEntiere);
				}
      }
    }
    else {
      printf("  -------------- |"); 
    }

    if ( Bb->NoeudDuMeilleurMinorant != 0 ) X = Bb->ValeurDuMeilleurMinorant;
    else                                   X = Bb->CoutDeLaMeilleureSolutionEntiere;

		if ( X >= 0.0 ) printf("  %12.8e |",X);
		else printf("  %11.7e |",X);
		
    if ( Bb->NombreDeSolutionsEntieresTrouvees > 0 && fabs( Bb->ValeurDuMeilleurMinorant ) > 1.e-9 ) {
      if ( Bb->NoeudDuMeilleurMinorant != 0 ) {
        Bb->EcartBorneInf = (Bb->CoutDeLaMeilleureSolutionEntiere - Bb->ValeurDuMeilleurMinorant) / (0.01 * Bb->ValeurDuMeilleurMinorant );
	      Bb->EcartBorneInf = fabs( Bb->EcartBorneInf );	
      }
      else Bb->EcartBorneInf = 0.0;
      /*printf(" %6.2f |", Bb->EcartBorneInf );*/   
      printf(" %6.4e |", Bb->EcartBorneInf );    
    }
    else {
      printf(" ---------- |"); 
    }
    printf("  %6d  |" , (int) TempsEcoule);		
    if ( Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes <= 0 ) {
      printf("  ---------------------  |");
      printf("\n");			
    }
    else {
		  AverageG = (int) ceil(Bb->NombreTotalDeGDuPoolUtilisees/Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes);
		  AverageI = (int) ceil(Bb->NombreTotalDeIDuPoolUtilisees/Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes);
		  AverageK = (int) ceil(Bb->NombreTotalDeKDuPoolUtilisees/Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes);
			Bb->AverageG = AverageG;
			Bb->AverageI = AverageI;
			Bb->AverageK = AverageK;			
			Tracer = NON;
		  if ( Bb->NoeudRacine->NombreDeG >= 0 ) {
			  printf("       G: %5d (%5d)      !",AverageG,Bb->NoeudRacine->NombreDeG);
				Tracer = OUI;
			}
		  if ( Bb->NoeudRacine->NombreDeI >= 0 ) {
			  if ( Tracer == NON ) {
          printf("\n");				
		      printf("       I: %5d (%5d)      !",AverageI,Bb->NoeudRacine->NombreDeI);
				}
				else {
          printf("\n");				
          printf(" "); 
          printf("|         |");      
          printf("               |");      
          printf("              |");
          printf("     |");      
          printf("                 |");     
          printf("                 |");
          printf("            |");       
          printf("          |");
		      printf("       I: %5d (%5d)      !",AverageI,Bb->NoeudRacine->NombreDeI);
				}
				Tracer = OUI;
			}
		  if ( Bb->NoeudRacine->NombreDeK >= 0 ) {
			  if ( Tracer == NON ) {
          printf("\n");				
		      printf("       K: %5d (%5d)      !",AverageK,Bb->NoeudRacine->NombreDeK);
				}
				else {
          printf("\n");								
          printf(" "); 
          printf("|         |");      
          printf("               |");      
          printf("              |");
          printf("     |");      
          printf("                 |");     
          printf("                 |");
          printf("            |");       
          printf("          |");
		      printf("       K: %5d (%5d)      !",AverageK,Bb->NoeudRacine->NombreDeK);
				}
				Tracer = OUI;
			}					
    }		
    printf("\n");

    if ( Bb->NombreDeNoeudsElagues != 0 && Bb->NombreDeSimplexes != 0 ) {
		  Bb->AveragePruningDepth = (int) ceil( Bb->SommeDesProfondeursDElaguage / Bb->NombreDeNoeudsElagues );
      printf("Pruned nodes: %d / Average pruning depth: %d / Average simplex iterations: %d\n",
			        Bb->NombreDeNoeudsElagues,
			        Bb->AveragePruningDepth,
							(int) ceil( (double) Bb->SommeDuNombreDIterations / (double) Bb->NombreDeSimplexes ) );
    }
		
    fflush(stdout);
    Bb->ForcerAffichage = NON;
  }
  if ( Bb->NombreMaxDeSolutionsEntieres >= 0 ) {
    if ( Bb->NombreDeSolutionsEntieresTrouvees >= Bb->NombreMaxDeSolutionsEntieres ) {		
      Bb->ArreterLesCalculs = OUI;
			if ( Bb->AffichageDesTraces == OUI ) {
			  printf("Stopping calculation. Integer solutions found has reached max. allowed (%d).\n",Bb->NombreMaxDeSolutionsEntieres);
			}			
    }
  }
  if ( Bb->NombreDeSolutionsEntieresTrouvees > 0 && fabs( Bb->ValeurDuMeilleurMinorant ) > 1.e-9 ) {
    X = (Bb->CoutDeLaMeilleureSolutionEntiere - Bb->ValeurDuMeilleurMinorant) / (0.01 * fabs( Bb->ValeurDuMeilleurMinorant ));
    X = fabs( X );				
    if ( X <= Bb->ToleranceDOptimalite ) {		
      Bb->ArreterLesCalculs = OUI;      
      Bb->EcartBorneInf = X;			
    }
		/* Si on n'a pas choisi d'arreter on refait le test sans la partie fixe */
		if ( Bb->ArreterLesCalculs != OUI ) {
      /* On ne fait ce calcul que si la partie fixe est du meme ordre que ValeurDuMeilleurMinorant */		 
			if ( fabs( Pne->Z0 ) < 10. * fabs( Bb->ValeurDuMeilleurMinorant ) ) {			
        X = (Bb->CoutDeLaMeilleureSolutionEntiere - Bb->ValeurDuMeilleurMinorant) / (0.01 * fabs( Bb->ValeurDuMeilleurMinorant - Pne->Z0 ));
        X = fabs( X );				
        if ( X <= Bb->ToleranceDOptimalite ) {		
          Bb->ArreterLesCalculs = OUI;      
          Bb->EcartBorneInf = X;
        }
			}
		}
  }
  else {
    if ( Bb->NombreDeSolutionsEntieresTrouvees > 0 ) {  
      BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT );
      if ( fabs( Bb->ValeurDuMeilleurMinorant ) > 1.e-9 ) {
        Bb->EcartBorneInf = (Bb->CoutDeLaMeilleureSolutionEntiere - Bb->ValeurDuMeilleurMinorant) / (0.01 * fabs( Bb->ValeurDuMeilleurMinorant ));						
	      Bb->EcartBorneInf = fabs( Bb->EcartBorneInf );
        if ( Bb->EcartBorneInf <= Bb->ToleranceDOptimalite ) {				
	        Bb->ArreterLesCalculs = OUI;
	      }
		    /* Si on n'a pas choisi d'arreter on refait le test sans la partie fixe */
		    if ( Bb->ArreterLesCalculs != OUI ) {
          /* On ne fait ce calcul que si la partie fixe est du meme ordre que ValeurDuMeilleurMinorant */		 
			    if ( fabs( Pne->Z0 ) < 10. * fabs( Bb->ValeurDuMeilleurMinorant ) ) {										
            X = (Bb->CoutDeLaMeilleureSolutionEntiere - Bb->ValeurDuMeilleurMinorant) / (0.01 * fabs( Bb->ValeurDuMeilleurMinorant - Pne->Z0 ));						
	          X = fabs( X );
            if ( X <= Bb->ToleranceDOptimalite ) {				
	            Bb->ArreterLesCalculs = OUI;					
              Bb->EcartBorneInf = X;
						}
	        }				
		    }				
      }
    }    
  }
  
}

#if VERBOSE_BB

printf("\n     Resolution du probleme relaxe \n ");
printf("    Etat des variables entieres du probleme: \n "); 

printf("        variable instanciee  / valeurs : \n");  fflush(stdout);
for( cpt = 0 , i = 0 , pt1 = NoeudCourant->IndicesDesVariablesEntieresInstanciees; 
     i < NoeudCourant->NombreDeVariablesEntieresInstanciees ; i++ , pt1++ , cpt++ ) {
  printf(" %4d ",*pt1);
  if ( cpt == 30 ) { printf("\n"); cpt = 0;}
}
printf("\n");
for( cpt = 0 , i = 0 , pt2 = NoeudCourant->ValeursDesVariablesEntieresInstanciees; 
     i < NoeudCourant->NombreDeVariablesEntieresInstanciees ; i++ , pt2++ , cpt++ ) {  
  printf(" %4c ",*pt2);
  if ( cpt == 30 ) { printf("\n"); cpt = 0;}
}
printf("\n ");
fflush(stdout);

#endif

Noeud = NoeudCourant->NoeudAntecedent;
/*
if ( Noeud != 0 ) {
  printf("Nombre de variables fractionnaires du noeud pere: %d\n",Noeud->NbValeursFractionnairesApresResolution);
}
*/

return; 
}



