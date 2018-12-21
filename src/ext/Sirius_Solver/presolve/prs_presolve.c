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

   FONCTION: Presolve
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"  
# include "pne_define.h"

# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

# define NI_AU_DEBUT_NI_A_LA_FIN  0
# define AU_DEBUT  1
# define A_LA_FIN  2
# define AU_DEBUT_ET_A_LA_FIN  3
# define AMELIORATION_DES_COEFF_BINAIRES  AU_DEBUT 
# if UTILISER_LE_GRAPHE_DE_CONFLITS == NON_PNE
  # define AMELIORATION_DES_COEFF_BINAIRES  AU_DEBUT_ET_A_LA_FIN 
# endif

# define TRACES 0

# define MARGE 1.e-5

/*----------------------------------------------------------------------------*/

void PRS_Presolve( void * PneE )
{		      
int NbModifications; int Relancer; int VariableEntiereFixee; int NombreDeVariables;
int BorneAmelioree; int VariableFixee; int NbCycles; int * ContrainteBornanteInferieurement;
int * ContrainteBornanteSuperieurement; int NbCntInact; int Var; int Cnt; char * ContrainteInactive;
int il; int ilMax; double A; double Amn; double Amx; int * TypeDeBornePourPresolve;
int * TypeDeBorneTrav; int NbMetaCycles; double * UTrav; double * UminTrav; double * UmaxTrav;
double * ValeurDeXPourPresolve; double * BorneInfPourPresolve; double * BorneSupPourPresolve;
char * ConserverLaBorneSupDuPresolve; char * ConserverLaBorneInfDuPresolve;  int NbVarNb;
char BorneInfPresolveDisponible; char BorneSupPresolveDisponible; char TypeBrn;
double Marge; PRESOLVE * Presolve; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) PneE;

# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
  void * Tas;
  Tas = MEM_Init();
  Presolve = (PRESOLVE *) MEM_Malloc( Tas, sizeof( PRESOLVE ) );
  if ( Presolve == NULL ) {
    printf("Saturation memoire, impossible d'allouer un objet PRESOLVE\n");
    goto FinDuPresolve;
  }			
	Presolve->Tas = Tas;		
# else
  Presolve = (PRESOLVE *) malloc( sizeof( PRESOLVE ) );
  if ( Presolve == NULL ) {
    printf("Saturation memoire, impossible d'allouer un objet PRESOLVE\n");
    goto FinDuPresolve;  
  }		
  Presolve->Tas = NULL;
# endif
 
Pne->ProblemePrsDuSolveur = (void *) Presolve;  
Presolve->ProblemePneDuPresolve = PneE;

for ( Pne->NombreDeVariablesEntieresTrav = 0 , Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) Pne->NombreDeVariablesEntieresTrav++;
}	

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  printf("Starting computations ->");
  printf(" rows: %6d",Pne->NombreDeContraintesTrav);
  printf(" columns: %6d",Pne->NombreDeVariablesTrav);
  printf(" binaries: %d",Pne->NombreDeVariablesEntieresTrav);	
  printf("\n");
}

#if VERBOSE_PRS  
  printf("Phase de Presolve\n");  
#endif

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

PRS_AllocationsStructure( Presolve );

NbCycles = 0; /* Pour eviter les warnign de compilation */
NbMetaCycles = 0;

/*if ( Pne->FaireDuPresolve == NON_PNE ) goto FinDuPresolve;*/

/* Calcul du plus grand et du plus petit terme (different de zero) */
Amn =  LINFINI_PNE;
Amx = -LINFINI_PNE;
for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  il    = Pne->MdebTrav[Cnt];
  ilMax = il + Pne->NbTermTrav[Cnt];
  while ( il < ilMax ) {
    A = fabs ( Pne->ATrav[il] ); 
    if ( A > ZERO_PRESOLVE ) {
      if ( A < Amn ) Amn = A;
      else if ( A > Amx ) Amx = A; 
    }
    il++;
  }
}

Pne->PlusGrandTerme = Amx;  
Pne->PlusPetitTerme = Amn;

if ( Pne->FaireDuPresolve == NON_PNE ) goto FinDuPresolve;

/***********************************************************************************/

/* Attention: ne peut etre appele que sur les bornes natives des variables ou sur les
   bornes definitives. Donc pas pendant les iterations de presolve car les bornes
	 du presolve ne sont pas toutes conservees. */
# if AMELIORATION_DES_COEFF_BINAIRES == AU_DEBUT || AMELIORATION_DES_COEFF_BINAIRES == AU_DEBUT_ET_A_LA_FIN 
  PNE_AmeliorerLesCoefficientsDesVariablesBinaires( Pne, (void *) Presolve, MODE_PRESOLVE );	
# endif

DebutDesCycles:
NbCycles = 0;
NbMetaCycles++;
Relancer = OUI_PNE;   /* C'est juste pour passer dans le while */
while ( Relancer == OUI_PNE && NbCycles < 5 /*5*/ && Pne->YaUneSolution != PROBLEME_INFAISABLE ) {

  NbCycles++;
	
  #if VERBOSE_PRS
    printf("Cycle de presolve numero %d \n",NbCycles);
  #endif
  Relancer = NON_PNE;
		
	/* Singleton sur ligne */	
	NbModifications = 1;	
	while( NbModifications > 0 ) {  
	  NbModifications = 0;
	  PRS_SingletonsSurLignes( Presolve, &NbModifications );
		# if TRACES == 1
	    if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_SingletonsSurLignes\n");
		# endif
	  if ( Pne->YaUneSolution != OUI_PNE ) break;
    if ( NbModifications != 0 ) Relancer = OUI_PNE;		
  }	
			
	/* Singleton sur colonne */	
  NbModifications = 1;
  while( NbModifications > 0 ) {
	  NbModifications = 0;
	  PRS_SingletonsSurColonnes( Presolve, &NbModifications );
		# if TRACES == 1
	    if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_SingletonsSurColonnes\n");
		# endif		
	  if ( Pne->YaUneSolution != OUI_PNE ) break;
	  if ( NbModifications != 0 ) Relancer = OUI_PNE;
	}	
	
	/* On cherche a borner les limites infinies a l'aide contraintes dont il manque
	   des bornes a une seule variable */
  
	PRS_BornerLesVariablesNonBornees( Presolve, &NbModifications );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_BornerLesVariablesNonBornees\n");
	# endif			
  if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( NbModifications != 0 ) Relancer = OUI_PNE;
		
  /* On cherche a ameliorer les bornes */		
  PRS_CalculerLesBornesDeToutesLesContraintes( Presolve, &NbModifications );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_CalculerLesBornesDeToutesLesContraintes\n");
	# endif			
	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( NbModifications != 0 ) Relancer = OUI_PNE;	    	
	
  PRS_ContraintesToujoursInactives( Presolve, &NbCntInact );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_ContraintesToujoursInactives\n");
	# endif			
	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( NbCntInact != 0 ) Relancer = OUI_PNE;
	
	PRS_AmeliorerLesBornes( Presolve, &BorneAmelioree );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_AmeliorerLesBornes\n");
	# endif		
 	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( BorneAmelioree == OUI_PNE ) Relancer = OUI_PNE;	
	
  PRS_BornerLesVariablesDualesNonBornees( Presolve, &NbModifications );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_BornerLesVariablesDualesNonBornees\n");
	# endif			
  if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( NbModifications != 0 ) Relancer = OUI_PNE;	

	/* A verifier: il ne faut pas de subititution de variable ou si presolve uniquement */
  PRS_VariablesDualesEtCoutsReduits( Presolve, &BorneAmelioree );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_VariablesDualesEtCoutsReduits\n");
	# endif			
	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( BorneAmelioree == OUI_PNE ) Relancer = OUI_PNE;	
		
  /* On regarde si on peut fixer des variables a l'aide de leurs couts: si une variable 
  n'intervient que dans des inegalites (ici toujours <) et que le signe de son cout et 
  le meme que ceux de ses coeff. dans les contraintes alors on peut fixer la variable 
  sur une de ses bornes */
	
  PRS_FixerVariablesSurCritere( Presolve, &VariableFixee );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_FixerVariablesSurCritere\n");
	# endif				
	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( VariableFixee == OUI_PNE ) Relancer = OUI_PNE;	
	
  /* Si une variable n'intervient pas dans les contraintes et n'est pas deja fixee on peut la 
     fixer sur une de ses bornes en fonction de son cout */		 
	
 	PRS_VariablesHorsContraintes( Presolve, &NbModifications );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_VariablesHorsContraintes\n");
	# endif		
	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( NbModifications != 0 ) Relancer = OUI_PNE;
			 
	/* A mettre en service parce que ca marche */	
  PRS_SubstituerVariables( Presolve, &NbModifications );
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_SubstituerVariables\n");
	# endif			
	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( NbModifications != 0 ) Relancer = OUI_PNE;	
		
	PRS_ContraintesColineaires( Presolve, &NbModifications );	
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_ContraintesColineaires\n");
	# endif			
	if ( Pne->YaUneSolution != OUI_PNE ) break;
  if ( NbModifications != 0 ) Relancer = OUI_PNE;

	/* Si presolve uniquement par appel pendant un branch and bound: on ne supprime pas les colonnes colineaires */
  if ( Pne->Controls != NULL ) {
    if ( Pne->Controls->PresolveUniquement == OUI_PNE ) {
	    NbModifications = 0;
	  }
  }
	else {	
    PRS_ColonnesColineaires( Presolve, &NbModifications ); 
	}
	# if TRACES == 1
	  if ( Pne->YaUneSolution != OUI_PNE ) printf("Probleme infaisable apres PRS_ColonnesColineaires\n");
	# endif			
	if ( Pne->YaUneSolution != OUI_PNE ) break;
	if ( NbModifications != 0 ) Relancer = OUI_PNE;
	
}

/* Experimentalement on constate que ce type de transformation peut conduire a des
   imprecisions sur les coefficients des contraintes telles que parfois on peut
	 se retrouver avec un probleme sans solution */
/*
if ( Pne->YaUneSolution != PROBLEME_INFAISABLE && NbMetaCycles <= 5 ) {
  Relancer = NON_PNE;
  NbModifications = 1;
  while( NbModifications > 0 ) {
    NbModifications = 0;
    PRS_CombinaisonDeContraintes( Presolve, &NbModifications );
    if ( NbModifications != 0 ) Relancer = OUI_PNE;
  }	
  if ( Relancer == OUI_PNE ) goto DebutDesCycles;
}
*/

#if VERBOSE_PRS
  printf("Fin des cycles de presolve\n");
#endif

/* Les contraintes dont toutes les variables sont fixees ne servent a rien */

PRS_SupprimerLesContraintesAvecQueDesVariablesFixees( Presolve );

if ( Pne->Controls != NULL ) {
  if ( Pne->Controls->PresolveUniquement == OUI_PNE ) {
	  Pne->Controls->Presolve = (void *) Presolve;
	  return;
	}
}

NombreDeVariables = Pne->NombreDeVariablesTrav;
UTrav = Pne->UTrav;  
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;

TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;
ContrainteInactive = Presolve->ContrainteInactive;
ContrainteBornanteInferieurement = Presolve->ContrainteBornanteInferieurement;
ContrainteBornanteSuperieurement = Presolve->ContrainteBornanteSuperieurement;

Marge = 1.e-5;  

for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {																																					
  if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) { /* C'est entre autres toujours vrai si c'est dans les donnees de depart */
    UTrav[Var] = ValeurDeXPourPresolve[Var];
    UminTrav[Var] = BorneInfPourPresolve[Var];
    UmaxTrav[Var] = BorneSupPourPresolve[Var];		

    if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
      /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	       duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		     faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		     recalculer exactement les variables duales. */
			if ( TypeDeBorneTrav[Var] != VARIABLE_FIXE ) {
        TypeDeBorneTrav[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
			}
		  else {
        TypeDeBorneTrav[Var] = VARIABLE_FIXE;
		  }					
    }
		else {
      TypeDeBorneTrav[Var] = TypeDeBornePourPresolve[Var];
		}
		
		continue;
	}
	else if ( ConserverLaBorneInfDuPresolve[Var] == OUI_PNE ) {			
	  UTrav[Var] = 0.0; /* Pas d'importance */
    UminTrav[Var] = BorneInfPourPresolve[Var];	
	  if ( ConserverLaBorneSupDuPresolve[Var] == OUI_PNE ) {		
      UmaxTrav[Var] = BorneSupPourPresolve[Var];		   
      TypeDeBorneTrav[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;			
		}
		else {
		  if ( TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) TypeDeBorneTrav[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
			else if ( TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) TypeDeBorneTrav[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;			
		}
	}
	else if ( ConserverLaBorneSupDuPresolve[Var] == OUI_PNE ) {	
	  /* Donc ConserverLaBorneInfDuPresolve[Var] est egal a NON_PNE */  
	  UTrav[Var] = 0.0; /* Pas d'importance */
    UmaxTrav[Var] = BorneSupPourPresolve[Var];
		if ( TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) TypeDeBorneTrav[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
		else if ( TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) TypeDeBorneTrav[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;						
	}
  else {			
    /* On garde les bornes du presolve mais on les elargi un petit peu */
		/* Si c'est un entier on le garde */		
		if ( Pne->TypeDeVariableTrav[Var] != ENTIER ) {		
      TypeDeBorneTrav[Var] = TypeDeBornePourPresolve[Var];
	    UTrav[Var] = 0.0; 
			Marge = 1.e-5;
			if ( BorneInfPourPresolve[Var] - Marge >= UminTrav[Var] ) UminTrav[Var] = BorneInfPourPresolve[Var] - Marge;
			if ( BorneSupPourPresolve[Var] + Marge <= UmaxTrav[Var] ) UmaxTrav[Var] = BorneSupPourPresolve[Var] + Marge;			
		}				
	}	
}

/* Attention: ne peut etre appele que sur les bornes natives des variables ou sur les
   bornes definitives. Donc pas pendant les iterations de presolve car les bornes
	 du presolve ne sont pas toutes conservees. */
# if AMELIORATION_DES_COEFF_BINAIRES == A_LA_FIN || AMELIORATION_DES_COEFF_BINAIRES == AU_DEBUT_ET_A_LA_FIN 
  /* Ici on beneficie de la creation de bornes la ou il n'y en n'avait pas */
  for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
    TypeDeBornePourPresolve[Var] = TypeDeBorneTrav[Var];
		BorneInfPourPresolve[Var] = UminTrav[Var];
    BorneSupPourPresolve[Var] = UmaxTrav[Var];
  }
  PNE_AmeliorerLesCoefficientsDesVariablesBinaires( Pne, (void *) Presolve, MODE_PRESOLVE );		
# endif

FinDuPresolve:

/* Pour sortir le graphe au format DIMACS */
/*PRS_ConstruireLeGrapheAuFormatDIMACS( Presolve );*/

/* Meme lorsqu'on ne fait pas de presolve on fait cet appel qui enleve les termes strictement
   nuls de la matrice */
	 
PRS_EnleverLesContraintesInactives( Presolve );

if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
  /* Attention: il est imperatif de le faire apres avoir enleve le contraintes inactives et pas avant.
	   De plus si on le fait avant le presolve, celui-ci risque de la supprimer en la detectant inactive
		 ce qui est vrai tant qu'on n'a pas trouve de solution entiere. */
  PNE_AjouterLaContrainteDeCoutMax( Pne );
}

/*********************************************/
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
UTrav = Pne->UTrav;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( UminTrav[Var] == UmaxTrav[Var] ) {
    TypeDeBorneTrav[Var] = VARIABLE_FIXE;
    Pne->TypeDeVariableTrav[Var] = REEL;
    UTrav[Var] = UminTrav[Var];
	}
}
/*****************************************/

#if VERBOSE_PRS
  printf("Nombre de cycles de presolve %d \n",NbCycles);
#endif

PRS_LiberationStructure( Presolve );

return;
}
 
