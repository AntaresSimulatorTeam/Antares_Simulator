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

   FONCTION: Appele par le branch and bound (ou branch and cut) pour
             resoudre un probleme relaxe.

Le sous_programme recoit:
 - la liste des variables entieres fixees
 - les variables entieres non encore fixees

il renvoit:
 - la valeur du critere optimise
 - un indicateur d'existence de solution
 - un indicateur: la solution est entiere ou non
 - les valeurs des variables entieres pour permettre le branchement
 - les valeurs de toutes les variables 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"
# include "prs_fonctions.h"

# include "spx_define.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_BranchAndBoundSolvePbRlx( 
          PROBLEME_PNE * Pne,
              /* Entree */
	        char   PremiereResolutionAuNoeudRacine, /* Vaut OUI_PNE ou NON_PNE */
					int    NumeroDePasse,
          char * CalculerDesCoupes,  /* Vaut OUI_PNE ou NON_PNE */
	        char   ChoisirLaVariableAInstancier, /* Vaut OUI_PNE ou NON_PNE */
          int * NombreDeVariablesInstanciees  , 
          int * IndicesDesVariablesInstanciees, 
          char * VariableAZeroOuAUn            , 
              /* Pour le simplexe */
          double * CoutDeLaMeilleureSolutionEntiere          , /* En entree seulement */
          int   * UtiliserCoutDeLaMeilleureSolutionEntiere  , 
          int   * BaseFournie                               , /* En entree seulement */
          int   * PositionDeLaVariable                      , /* En entree et sortie */
          int   * NbVarDeBaseComplementaires                , /* En entree et sortie */   
          int   * ComplementDeLaBase                        , /* En entree et sortie */ 
              /*  */
              /* Sortie */
          double * Critere                          , 
          int   * Faisabilite                      , 
          int   * LaSolutionEstEntiere             , 
          int   * VariableProposee                 , 
          double * MinorantEspereAEntierInf         ,
          double * MinorantEspereAEntierSup         ,	 
          double * ResultatSurLesVariablesEntieres  , 
          double * ResultatSurToutesLesVariables    ,  
           /* Bases proposées associées à la variable proposée pour l'instanciation */
          int   * PositionDeLaVariableAEntierInf      , 
          int   * NbVarDeBaseComplementairesAEntierInf,    
          int   * ComplementDeLaBaseAEntierInf        ,   
          int   * PositionDeLaVariableAEntierSup      , 
          int   * NbVarDeBaseComplementairesAEntierSup,    
          int   * ComplementDeLaBaseAEntierSup        , 
          /* Indicateur de disponibilite des bases a entier sup et inf */
          int   * BasesFilsDisponibles                   
                          )   
{
int i ; int j ; NOEUD * Noeud; BB * Bb; /*int * TypeDeBorneTravSv;*/ double * UminTrav;
double * UmaxTrav; int * NumeroDeLaVariableModifiee; char * TypeDeBorneModifiee;
double * NouvelleValeurDeBorne; char RelancerUnSimplexe; double * UTrav;

# if UTILISER_UNE_CONTRAINTE_DE_COUT_MAX == OUI_PNE
  *UtiliserCoutDeLaMeilleureSolutionEntiere = NON;
# endif

Bb = Pne->ProblemeBbDuSolveur;
Noeud = Bb->NoeudEnExamen;

*Faisabilite          = OUI_PNE ;
*LaSolutionEstEntiere = NON_PNE;
*BasesFilsDisponibles = NON_PNE;

/* Pour les Gub */
Pne->NbVarGauche = 0;
Pne->NbVarDroite = 0;

memcpy( (char *) Pne->TypeDeBorneTrav , (char *) Pne->TypeDeBorneTravSv, Pne->NombreDeVariablesTrav * sizeof( int ) );

/* On met les bornes des variables au noeud */
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
UTrav = Pne->UTrav;

memcpy( (char *) UminTrav, (char *) Pne->UminTravSv, Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) UmaxTrav, (char *) Pne->UmaxTravSv, Pne->NombreDeVariablesTrav * sizeof( double ) );

NumeroDeLaVariableModifiee = Noeud->NumeroDeLaVariableModifiee;
TypeDeBorneModifiee  = Noeud->TypeDeBorneModifiee;
NouvelleValeurDeBorne = Noeud->NouvelleValeurDeBorne;

for( i = 0 ; i < Noeud->NombreDeBornesModifiees ; i++ ) {
	if ( TypeDeBorneModifiee[i] == BORNE_INF ) {		
	  UminTrav[NumeroDeLaVariableModifiee[i]] = NouvelleValeurDeBorne[i];
	}
	else if ( TypeDeBorneModifiee[i] == BORNE_SUP ) {
	  UmaxTrav[NumeroDeLaVariableModifiee[i]] = NouvelleValeurDeBorne[i];
	}
	else {
	  if ( Pne->AffichageDesTraces == OUI_PNE ) {
		  printf("PNE_BranchAndBoundSolvePbRlx: erreur dans la limitation des bornes (etape reduced cost fixing)\n");
		}
	}
}

/* Modification des types de bornes */
for ( i = 0 ; i < Pne->NombreDeVariablesTrav  ; i++ ) {
  if ( Pne->TypeDeBorneTrav[i] == VARIABLE_BORNEE_INFERIEUREMENT ) {
    if ( Pne->UmaxTrav[i] - Pne->UminTrav[i] < PLAGE_REDUCED_COST_FIXING ) { 
      Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_DES_DEUX_COTES;
    }
  }
  else if (  Pne->TypeDeBorneTrav[i] == VARIABLE_NON_BORNEE ) {
    if ( Pne->UmaxTrav[i] - Pne->UminTrav[i] < PLAGE_REDUCED_COST_FIXING ) { 
      Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_DES_DEUX_COTES;
    }
    else if ( Pne->UminTrav[i] > -PLAGE_REDUCED_COST_FIXING ) {
      Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_INFERIEUREMENT;
    }
    else if ( Pne->UmaxTrav[i] < PLAGE_REDUCED_COST_FIXING ) {
      /* A ce stade on ne peut plus utiliser le type de borne VARIABLE_BORNEE_SUPERIEUREMENT sans trop de complication */
      /* Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_SUPERIEUREMENT; */
    }   
  }
}

for ( j = 0 ; j < *NombreDeVariablesInstanciees ; j++ ) {
  i = IndicesDesVariablesInstanciees[j];
  if ( i < 0 ) {
    printf("PNE_SolvePbRlxBranchAndBound: bug dans la connexion au Branch and Bound variable instanciee %d \n", i);
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }

  if ( Pne->SolveurPourLeProblemeRelaxe != SIMPLEXE ) Pne->TypeDeBorneTrav[i] = VARIABLE_FIXE;  
  else { /* Cas du simplexe, on ne peut fixer la variable que si elle n'etait pas en base */  
    /* On ne gagne pas grand chose a fixer les variables alors on laisse le type de borne inchangé et 
       on se contente de mettre a jour les bornes de la variable */ 
    /* Inhibition, maintenant on ne change plus les types des variables en cours de route */ 
    /*if ( PositionDeLaVariable[i] != EN_BASE ) Pne->TypeDeBorneTrav[i] = VARIABLE_FIXE;*/  
  }

  if ( VariableAZeroOuAUn[j] == '0' ) {
		
    if ( Pne->UminTravSv[i] > 0.1 ) {
		  /*printf("Probleme relaxe: pas de solution des le depart\n");*/
      *Faisabilite = NON_PNE ;
      goto Fin;
		}
			
    UminTrav[i] = 0.;  /* Umin est egal a 0 car il y a eu une translation des bornes en entree */
    UmaxTrav[i] = 0.;
    UTrav   [i] = 0.;
  }
  else if ( VariableAZeroOuAUn[j] == '1' ) {
    
    if ( Pne->UmaxTravSv[i] < 0.9 ) {
		  /*printf("Probleme relaxe: pas de solution des le depart\n");*/
      *Faisabilite = NON_PNE ;
      goto Fin;
		}
	  	
    UminTrav[i] = 1./*Pne->UmaxTrav[i]*/;
    UmaxTrav[i] = 1./*Pne->UmaxTrav[i]*/;
    UTrav   [i] = 1./*Pne->UmaxTrav[i]*/;
  }
  else {
    printf("PNE_SolvePbRlxBranchAndBound: bug dans la connexion au Branch and Bound \n");
    printf("  variable instanciee %d\n  valeur variable instanciee %c \n",i,VariableAZeroOuAUn[j]);
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }
}

/* Nouvelle modification des types de bornes */
for ( i = 0 ; i < Pne->NombreDeVariablesTrav  ; i++ ) {
  if ( Pne->TypeDeBorneTrav[i] == VARIABLE_BORNEE_INFERIEUREMENT ) {
    if ( Pne->UmaxTrav[i] - Pne->UminTrav[i] < PLAGE_REDUCED_COST_FIXING ) { 
      /*printf("Ancien TypeDeBorneTrav %d  modifie\n",Pne->TypeDeBorneTrav[i]);*/ 
      Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_DES_DEUX_COTES;
    }
  }
  else if (  Pne->TypeDeBorneTrav[i] == VARIABLE_NON_BORNEE ) {
    if ( Pne->UmaxTrav[i] - Pne->UminTrav[i] < PLAGE_REDUCED_COST_FIXING ) { 
      Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_DES_DEUX_COTES;
    }
    else if ( Pne->UminTrav[i] > -PLAGE_REDUCED_COST_FIXING ) {
      Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_INFERIEUREMENT;
    }
    else if ( Pne->UmaxTrav[i] < PLAGE_REDUCED_COST_FIXING ) {
      /* A ce stade on ne peut plus utiliser le type de borne VARIABLE_BORNEE_SUPERIEUREMENT sans trop de complication */
      /* Pne->TypeDeBorneTrav[i] = VARIABLE_BORNEE_SUPERIEUREMENT; */
    }   
  }
  if ( Pne->TypeDeBorneTrav[i] == VARIABLE_NON_BORNEE ) {
	  /* On remet l'infini */
		UminTrav[i] = -LINFINI_PNE;
		UmaxTrav[i] = LINFINI_PNE;
  }	
}

/* Node presolve */
# if FAIRE_DU_NODE_PRESOLVE == OUI_PNE
  if ( Pne->YaDesVariablesEntieres == OUI_PNE && NumeroDePasse == 1 ) {  
    /* Attention il ne faut l'appeler qu'au premier calcul */
    PNE_NodePresolve( Pne, Faisabilite );			
  }		
# endif

PNE_ControleCliquesAvantResolutionProblemeRelaxe( Pne, Faisabilite );

if ( Pne->SolveurPourLeProblemeRelaxe == POINT_INTERIEUR ) {
  /* Resolution du probleme relaxe par le point interieur */
  PNE_SolvePbRlxPi( Pne, Critere, Faisabilite );
  if ( *Faisabilite == OUI_PNE ) {
    PNE_CalculerLaValeurDuCritere( Pne );  
    *Critere = Pne->Critere;
	}	
}
else if ( Pne->SolveurPourLeProblemeRelaxe == SIMPLEXE ) {
  if ( *Faisabilite == OUI_PNE ) {
    SimplexeDual:
    /* On essaie de restreindre les bornes des variables entieres (et donc de les fixer) */
    { int NombreDeVariablesEntieresFixees; char InfaisabiliteDetectee; char Mode;
      Mode = CALCUL;
      PNE_CalculerLesRestrictionsDeBornes( Pne, &NombreDeVariablesEntieresFixees, &InfaisabiliteDetectee, Mode );
    }
    /*   */
		/* On essaie de fixer des variables entieres */
    if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
      PNE_FixationDesVariablesEntieresSurCritere( Pne );			
		}		
    /* Resolution du probleme par le simplexe dual */			
		PNE_SolvePbRlxSpxDual( Pne, PremiereResolutionAuNoeudRacine, *CoutDeLaMeilleureSolutionEntiere,
		                       *UtiliserCoutDeLaMeilleureSolutionEntiere, *BaseFournie, PositionDeLaVariable,
													 NbVarDeBaseComplementaires, ComplementDeLaBase, Faisabilite );																					
													 
		if ( *Faisabilite == OUI_PNE ) {					
      PNE_CalculerLaValeurDuCritere( Pne );			
      *Critere = Pne->Critere;						
      if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
			  if ( Pne->YaUneSolutionEntiere == OUI_PNE ) {
				  if ( Pne->Critere >= Pne->CoutOpt ) *Faisabilite = NON_PNE;         					
				}
			}			
	  }

    if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
      if ( *Faisabilite == OUI_PNE ) {
        if ( Bb != NULL ) {
				  if ( Bb->NoeudEnExamen == Bb->NoeudRacine ) {
            PNE_ArchivagesPourReducedCostFixingAuNoeudRacine( Pne, PositionDeLaVariable, Pne->CoutsReduits, Pne->Critere );
					}
				}
		  }
		}		
		
    /* Reduced cost fixing */		
    if ( *Faisabilite == OUI_PNE && Pne->YaUneSolutionEntiere == OUI_PNE ) {
      RelancerUnSimplexe = PNE_ReducedCostFixing( Pne, PositionDeLaVariable );
      if ( RelancerUnSimplexe == OUI_PNE ) {
        #if VERBOSE_PNE
          printf(" ReducedCostFixing : OnAFixeDesVariablesEntieres\n");
        #endif
        PremiereResolutionAuNoeudRacine = NON_PNE;
        *BaseFournie = OUI_SPX;	
	      goto SimplexeDual;	
      }        
    }      
  }
	
}
else {
  printf(" Bug dans PNE_BranchAndBoundSolvePbRlx, Pne->SolveurPourLeProblemeRelaxe mal initialise\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

if ( *Faisabilite == OUI_PNE ) {
  
	Pne->FaireDuStrongBranching = OUI_PNE;
	
  PNE_DeterminerLesVariablesFractionnaires( Pne, PositionDeLaVariable );
	
  /* En test: clonage du simplexe au noeud racine pour le reactiver de temps en temps */
	/* S'il s'agit d'un probleme a variables entieres */
	# if RELANCE_PERIODIQUE_DU_SIMPLEXE_AU_NOEUD_RACINE == OUI_PNE
	  if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
	    /* S'il s'agit du dernier simplexe au noeud etudie */
      if ( ChoisirLaVariableAInstancier == OUI_PNE ) {
	      /* S'il s'agit du noeud racine */
	      if ( Bb->NoeudEnExamen == Bb->NoeudRacine && Pne->Controls == NULL ) {
 		      /* Si le probleme a une solution et qu'il reste des variables fractionnaires */		
          if ( *Faisabilite == OUI_PNE && Pne->NombreDeVariablesAValeurFractionnaire != 0 ) {
            PNE_CloneProblemeSpxDuNoeudRacine( Pne, PositionDeLaVariable, NbVarDeBaseComplementaires, ComplementDeLaBase ); 																					 
          }
			  }
		  }
    }
	# endif

  # if UTILISER_LE_GRAPHE_DE_CONFLITS == OUI_PNE
	  # if PROBING_JUSTE_APRES_LE_PRESOLVE == NON_PNE
      if ( Pne->YaDesVariablesEntieres == OUI_PNE && Noeud == Bb->NoeudRacine ) {
        PNE_VariableProbing( Pne );
	      if ( Pne->ProbingOuNodePresolve != NULL ) {
	        if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) *Faisabilite = NON_PNE;
			    else PNE_CliquesConflictGraph( Pne );
        }			
      }	
    # endif
  # endif
	
}
	
if ( *Faisabilite == OUI_PNE ) {
  
  PNE_StrongBranching( Pne,
          /*PositionDeLaVariable,*/ 
          /* Choix de la variable a instancier */
          MinorantEspereAEntierInf,
          MinorantEspereAEntierSup,	 
	        /* Base correspondante pour l'instanciation a 0 */
          PositionDeLaVariableAEntierInf      , 
          NbVarDeBaseComplementairesAEntierInf,    
          ComplementDeLaBaseAEntierInf        ,
	        /* Base correspondante pour l'instanciation a 1 */
          PositionDeLaVariableAEntierSup      , 
          NbVarDeBaseComplementairesAEntierSup,    
          ComplementDeLaBaseAEntierSup        ,	   
          /*  */
    	    BasesFilsDisponibles                ,
	        CalculerDesCoupes	                  ,
          /*NumeroDePasse,*/
	        ChoisirLaVariableAInstancier        
                        );
    
  if( Pne->CestTermine == OUI_PNE ) {
    #if VERBOSE_PNE
      printf(" Probleme relaxe: on a trouve une solution entiere \n");
    #endif
    *LaSolutionEstEntiere = OUI_PNE;
    if ( 0 ) { /* Pas de traces s'il y a un 0 dans le if */
    { int Nb; int Var; int NbVar;
      for ( Var = 0 , NbVar = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
        if ( Pne->UmaxTravSv[Var] != Pne->UminTravSv[Var] ) NbVar++;
      }
      for ( Var = 0 , Nb = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
        if ( Pne->UmaxTravSv[Var] == Pne->UminTravSv[Var] ) {
          if ( fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] ) < 1.e-6 && Pne->LTrav[Var] != 0.0 ) Nb++;
	}
      }       
      printf("-------------------- nombre de variables a cout non nul a 1 par le presolve : %d  nombre de variables %d\n",Nb,NbVar);    
      for ( Var = 0 , Nb = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
        if ( fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] ) < 1.e-6 ) Nb++;
      }
      printf("-------------------- nombre de variables a 1 : %d  nombre de variables %d\n",Nb,NbVar);
      for ( Var = 0 , Nb = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
        if ( fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] ) < 1.e-6 && Pne->LTrav[Var] != 0.0 ) Nb++;
      }
      printf("-------------------- nombre de variables a cout non nul a 1 : %d  nombre de variables %d\n",Nb,NbVar);
      for ( Var = 0 , Nb = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
        if ( Pne->UmaxTravSv[Var] == Pne->UminTravSv[Var] ) continue;
        if ( Pne->UmaxTrav[Var] == Pne->UminTrav[Var] ) {
          if ( fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] ) < 1.e-6 && Pne->LTrav[Var] != 0.0 ) Nb++;	
	}
      }
      printf("-------------------- nombre de variables a cout non nul a 1 par instanciation : %d  nombre de variables %d\n",Nb,NbVar);      
      for ( Var = 0 , Nb = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
        if ( Pne->UmaxTravSv[Var] == Pne->UminTravSv[Var] ) continue;
        if ( Pne->UmaxTrav[Var] == Pne->UminTrav[Var] ) {
          if ( fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] ) < 1.e-6 ) Nb++;	
	}
      }
      printf("-------------------- nombre de variables a 1 par instanciation : %d  nombre de variables %d\n",Nb,NbVar);     
    }
    }
  }
  else { 
    *VariableProposee = Pne->VariableLaPlusFractionnaire;
  }
  
  /* On recupere la sortie */
  for ( j = 0 , i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
    ResultatSurToutesLesVariables[i] = Pne->UTrav[i];
    if ( Pne->TypeDeVariableTrav[i] == ENTIER ) {
      ResultatSurLesVariablesEntieres[j] = Pne->UTrav[i];
      j++;
    }
  }

}
else {
  Pne->ResolutionDuNoeudReussie = NON_PNE;
}

/* Calcul de coupes et stockage */
/* On calcule les coupes, puis on relance un simplexe au meme noeud. Ensuite on ne stocke reellement 
   que les coupes pour lesquelles la variable d'ecart est non basique c'est à dire les coupes saturees */ 
if ( *Faisabilite == OUI_PNE ) {
  if( ChoisirLaVariableAInstancier == OUI_PNE ) {
    if ( Bb != NULL ) BB_RechercherLesCoupesViolees( Bb, Pne->UTrav );
  }
	/* Init necessaire au cas ou on n'appellerait pas PNE_CalculerLesCoupes car alors les valeurs ne seraient
	   pas initialisees */
  Pne->NombreDeCoupesCalculeesNonEvaluees = 0;
  Pne->NombreDeCoupesAjoute = 0;		 
  if ( *CalculerDesCoupes == OUI_PNE ) {
    if ( Pne->SolveurPourLeProblemeRelaxe != POINT_INTERIEUR && Pne->PremFrac >= 0 ) {
      /* Identification des contraintes saturees */
      /*PNE_RechercheDesContraintesSaturees( Pne, *NbVarDeBaseComplementaires, ComplementDeLaBase );*/             
      PNE_CalculerLesCoupes( Pne );                
      *VariableProposee = Pne->VariableLaPlusFractionnaire;      
    } 
  }
  else if ( Pne->SolveurPourLeProblemeRelaxe != POINT_INTERIEUR ) {
    /* Tri des coupes */
    /* L'appel doit se faire au niveau du branch and bound
    PNE_TrierLesCoupesDeGomory( *BasesFilsDisponibles , CoupeSaturee , CoupeSatureeAEntierInf , CoupeSatureeAEntierSup ); 
    */
  }
}
else {
  /* Remise a 0 necessaire car on peut calculer des coupes et sortir ensuite par depassement du majorant */
  /* Pne->NombreDeCoupesCalculees = 0; */
}

/* Le calcul est termine, on tente l'heuristique */
if ( *Faisabilite == OUI_PNE && ChoisirLaVariableAInstancier == OUI_PNE ) {
  PNE_Heuristique( Pne, *CoutDeLaMeilleureSolutionEntiere, *UtiliserCoutDeLaMeilleureSolutionEntiere, 
                   PositionDeLaVariable, *NbVarDeBaseComplementaires, ComplementDeLaBase );
  if( Pne->CestTermine == OUI_PNE ) {
    *LaSolutionEstEntiere = OUI_PNE;
  }         
}

Fin: 

/* On remet les bornes comme elles etaient et l'indicateur fixe ou non comme il etait */
memcpy( (char *) Pne->TypeDeBorneTrav , (char *) Pne->TypeDeBorneTravSv, Pne->NombreDeVariablesTrav * sizeof( int ) );

/* Par prudence a cause du reduced cost fixing: il faut mieux repartir de UminTravSv et UmaxTravSv */
memcpy( (char *) UminTrav, (char *) Pne->UminTravSv, Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) UmaxTrav, (char *) Pne->UmaxTravSv, Pne->NombreDeVariablesTrav * sizeof( double ) );

return;
}

