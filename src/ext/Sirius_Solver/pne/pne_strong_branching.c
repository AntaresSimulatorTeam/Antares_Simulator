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

   FONCTION: Strong branching pour le choix de la variable a instancier.   
             Appelé par le branch and bound, ce sous-programme retourne 
             le numéro de la variable a instancier et la base de depart 
             qu'il faudra utiliser lorsque l'on évaluera le noeud dans
             une étape ultérieure.
                            
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
							     
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define SEUIL_BIG_M 1.e-2

void PNE_StrongBranchingTesterLesGUB( PROBLEME_PNE * );
void PNE_StrongBranchingGraphedeConflit( PROBLEME_PNE * , int , double );

/*----------------------------------------------------------------------------*/

double PNE_ValeurOptimaleDuProblemeCourantAvantNouvellesCoupes( PROBLEME_PNE * Pne )
{
return( Pne->ValeurOptimaleDuProblemeCourantAvantNouvellesCoupes );
} 

/*----------------------------------------------------------------------------*/

void PNE_StrongBranching( PROBLEME_PNE * Pne,
          /*int   * PositionDeLaVariable,*/ 
          /* Choix de la variable a instancier */
          double * MinorantEspereAGauche,
          double * MinorantEspereADroite,	 
	  /* Base correspondante pour l'instanciation a gauche */
          int   * PositionDeLaVariableAGauche      , 
          int   * NbVarDeBaseComplementairesAGauche,    
          int   * ComplementDeLaBaseAGauche        ,
	  /* Base correspondante pour l'instanciation a droite */
          int   * PositionDeLaVariableADroite      , 		    
          int   * NbVarDeBaseComplementairesADroite,    
          int   * ComplementDeLaBaseADroite        ,
          /* Indicateur de disponibilite des bases a entier sup et inf */
          int   * BasesFilsDisponibles,                     
          /* Si on demande le calcul des coupes, on ne fait pas de strong branching */
          char   * CalculerDesCoupes,
          /*char     NumeroDePasse,*/
	        char     ChoisirLaVariableAInstancier
                        )  
{
int i ; int Count; int Var; int NbSeuil; int il; char ChoixDefinitif ; char ChoixAmeliorant;
double Seuil; int ExistenceDUneSolAGauche ; double r; int ExistenceDUneSolADroite;  char TypeDeSolution ;
int * PosVarAGauche; int * ComplBaseAGauche; int * PosVarADroite; double Frac;
int * ComplBaseADroite; double DeltaXAGauche ; double DeltaXADroite; int NbVarComplAGauche; int NbVarComplADroite; 
double CritereAGauche; double CritereADroite; double SupEcartDInstanciation; double SupDeNouveauCritere;
double SeuilNouveauCout ; double CritereInfini ; double Marge; double Crit; double MinCritere;   
char * PositionDeLaVariableDEcartAGauche; char * PositionDeLaVariableDEcartADroite; char ExistenceSolutionAGauche;
char ExistenceSolutionADroite; char VariableSelectionnee; int SeuilMx;
char ChoixDuTypeDeVariation; char TesterLeBranchementSurLesGub; double CoutReduitInf; double CoutReduitSup;
int NombreDeVariablesTrav; int NombreDeContraintesTrav; double X; int PremFrac; int * SuivFrac;
int * TypeDeBorneTrav; int * TypeDeVariableTrav; double * UTrav;	double * UminTrav; double * UmaxTrav;
double * UStrongBranching; double * LTrav; char * VariableBinaireBigM; int VarChoix; double PremiereFractionnalite;
BB * Bb; NOEUD * Noeud; double FracMoy; int Nb;

CritereInfini = LINFINI_PNE;

*MinorantEspereAGauche = Pne->Critere; 
*MinorantEspereADroite = Pne->Critere;

if ( Pne->FaireDuStrongBranching != OUI_PNE ) goto PasStrongBranching;   

if ( *CalculerDesCoupes == OUI_PNE ) {     
  if ( ChoisirLaVariableAInstancier == NON_PNE ) {
    goto PasStrongBranching;
  }
}

Bb = Pne->ProblemeBbDuSolveur; 
Noeud = Bb->NoeudEnExamen;

if ( Pne->SolveurPourLeProblemeRelaxe != POINT_INTERIEUR && Pne->NombreDeVariablesAValeurFractionnaire >= 1 ) {

  PremFrac = Pne->PremFrac;
  SuivFrac = Pne->SuivFrac;
  UTrav = Pne->UTrav;
	
  VariableBinaireBigM = Pne->VariableBinaireBigM;
  if ( Pne->YaDesBigM == OUI_PNE && Pne->YaUneSolutionEntiere == NON_PNE ) {	
	  /* S'il y a des Big M sur des variables entieres on va privilegier l'instanciation de ces variables */
		r = -1.;
		VarChoix = -1;
    i = PremFrac;
    while ( i >= 0 ) {
      /* Si la fractionalite d'une variable est tres proche de 0 on regarde si en la multipliant par
			   son plus grand coeff, on obtient un nombre non negligeable. Si c'est le cas cela signifie
				 que l'on aura du mal a dire si la variable est a 0 oui a 1 et dans ce cas le mieux est de
				 l'instancier pour lever le doute. Comme il peut y avoir plusieurs variables dans ce cas,
				 on prend celle dont le produit s'eloigne le plus de 0 */		
		  if ( VariableBinaireBigM[i] == OUI_PNE ) {
        X = UTrav[i];
        if ( X - floor( X ) < ceil( X ) - X ) X = X - floor( X );
        else X = ceil( X ) - X;
        Frac = X;
				if ( Frac < TOLERANCE_SUR_LES_ENTIERS ) {
					X = Frac * ( SEUIL_DADMISSIBILITE / Pne->SeuilDeFractionnalite[i] );
          if ( X > r ) {
            r = X;
						VarChoix = i;
					}
				}		  
      }			
      i = SuivFrac[i];
    }
    if ( VarChoix >= 0 && r > SEUIL_BIG_M ) {
      Pne->VariableLaPlusFractionnaire = VarChoix;
			/*
			printf("Instanciation variable big M %d de valeur %e Ai %e\n",VarChoix,UTrav[VarChoix],SEUIL_DADMISSIBILITE / Pne->SeuilDeFractionnalite[VarChoix]);
      */
			goto PasStrongBranching;
		}
  }

  PosVarAGauche    = (int *) malloc( Pne->NombreDeVariablesTrav   * sizeof( int ) );
  ComplBaseAGauche = (int *) malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );
  PosVarADroite    = (int *) malloc( Pne->NombreDeVariablesTrav   * sizeof( int ) );
  ComplBaseADroite = (int *) malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );

  PositionDeLaVariableDEcartAGauche  = (char *) malloc( Pne->Coupes.NombreDeContraintes * sizeof( char ) );
  PositionDeLaVariableDEcartADroite  = (char *) malloc( Pne->Coupes.NombreDeContraintes * sizeof( char ) );

  if (
       PosVarAGauche                     == NULL || ComplBaseAGauche                  == NULL || 
       PosVarADroite                     == NULL || ComplBaseADroite                  == NULL ||
       PositionDeLaVariableDEcartAGauche == NULL || PositionDeLaVariableDEcartADroite == NULL 
     ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_StrongBranching \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); 
  }

  #if VERBOSE_PNE
    printf(" Phase de Strong Branching \n");
  #endif
	
  NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
  NombreDeContraintesTrav = Pne->NombreDeContraintesTrav;
  TypeDeBorneTrav = Pne->TypeDeBorneTrav; 
  TypeDeVariableTrav = Pne->TypeDeVariableTrav;

  UminTrav = Pne->UminTrav;
  UmaxTrav = Pne->UmaxTrav;

  UStrongBranching = Pne->UStrongBranching ;
  LTrav = Pne->LTrav;


  NbSeuil = (int) floor( 0.5 * Pne->NombreDeVariablesAValeurFractionnaire );  
  if ( NbSeuil < 5  ) NbSeuil = 5;
  SeuilMx = 50;
  if ( Pne->YaUneSolutionEntiere == OUI_PNE ) SeuilMx = 50;
	
	/* Valeur par defaut: PENTE_DE_LA_VARIATION */
  ChoixDuTypeDeVariation = PENTE_DE_LA_VARIATION;

  Seuil   = 1.e-3;
  NbSeuil = 0;	
	
	/* Si la fractionnalite chute brutalement d'un rapport r on arrete */
	r = 1.e-2 /*1.e-2*/;
	Crit = 0.0;
  i = PremFrac;
  while ( i >= 0 ) {
    X = UTrav[i];   
    if ( X - floor( X ) < ceil( X ) - X ) X = X - floor( X );
    else X = ceil( X ) - X;
		/* X est la fractionnalite */
		if ( X < r * Crit && X < Seuil ) break;
		Crit = X;
    NbSeuil++;
    i = SuivFrac[i];
  }
   
  if ( NbSeuil < 4/*2*/ ) NbSeuil = 4/*2*/;
  else if ( NbSeuil > SeuilMx ) NbSeuil = SeuilMx;
		
	/* Si les fractionnalites sont trop disparates, on evite d'utiliser la pente de variation */
  /* Si la fractionnalite moyenne est trop faible on prend VALEUR_ABSOLUE_DE_LA_VARIATION */

	Nb = 0;
	FracMoy = 0.0;
	
	i = PremFrac;	
  X = UTrav[i];   
  if ( X - floor( X ) < ceil( X ) - X ) X = X - floor( X );
  else X = ceil( X ) - X;
  il = i;

	FracMoy += X;
	Nb++;
		
  i = SuivFrac[i];
	Count = 1;
  while ( i >= 0 && Count < NbSeuil ) {

    X = UTrav[i];   
    if ( X - floor( X ) < ceil( X ) - X ) X = X - floor( X );
    else X = ceil( X ) - X;
	  FracMoy += X;
	  Nb++;
		
	  il = i;
    i = SuivFrac[i];
		Count++;
	}
	
  Crit = UTrav[il];   
  if ( Crit - floor( Crit ) < ceil( Crit ) - Crit ) Crit = Crit - floor( Crit );
  else Crit = ceil( Crit ) - Crit;
  if ( X / Crit > 1.e+6 ) {
	  ChoixDuTypeDeVariation = VALEUR_ABSOLUE_DE_LA_VARIATION;		
	}

	if ( Nb > 0 ) {
	  FracMoy /= Nb;
		/*printf("FracMoy %e \n",FracMoy);*/
		if ( FracMoy < 1.e-2 ) {
      ChoixDuTypeDeVariation = VALEUR_ABSOLUE_DE_LA_VARIATION;
			/*printf("FracMoy %e => VALEUR_ABSOLUE_DE_LA_VARIATION\n",FracMoy);*/ 
		}
	}	

  #if VERBOSE_PNE
    if ( ChoixDuTypeDeVariation == VALEUR_ABSOLUE_DE_LA_VARIATION ) printf("ChoixDuTypeDeVariation = VALEUR_ABSOLUE_DE_LA_VARIATION\n");
    if ( ChoixDuTypeDeVariation == PENTE_DE_LA_VARIATION ) printf("ChoixDuTypeDeVariation = PENTE_DE_LA_VARIATION\n");
  #endif

	/* Test */
  if ( Bb->NombreDeSolutionsEntieresTrouvees > 0 ) {
    if ( Bb->EcartBorneInf < 1.e-2 ) ChoixDuTypeDeVariation = PENTE_DE_LA_VARIATION;			
	}
	/* Fin test */

  SupDeNouveauCritere    = -CritereInfini;
  SupEcartDInstanciation = 1.e-6;
  
  MinCritere		= CritereInfini;
  SeuilNouveauCout = Pne->Critere + ( 0.1 * fabs( Pne->Critere ) );
  Marge            = 0.; 
  Count            = 0;
  
  Pne->NbVarGauche = 0; /* Par precaution */
  Pne->NbVarDroite = 0; /* Par precaution */
 
  /* Soit il n'y a pas de Gub, soit les variations de critere sont trop faible. Mais dans ce cas,
     SupDeNouveauCritere a pu etre initialise */

	PremiereFractionnalite = Pne->UTrav[PremFrac] - floor( Pne->UTrav[PremFrac] );
  if ( PremiereFractionnalite > 	ceil( Pne->UTrav[PremFrac] ) - Pne->UTrav[PremFrac] ) {
    PremiereFractionnalite = ceil( Pne->UTrav[PremFrac] ) - Pne->UTrav[PremFrac];
	}

	CritereAGauche = Pne->Critere; /* Pour ne pas avoir de warning a la compilation */
	CritereADroite = Pne->Critere; /* Pour ne pas avoir de warning a la compilation */
	
  VariableSelectionnee = NON_PNE;	
  i = PremFrac;  
  while ( i >= 0 && Count < NbSeuil ) {
  
	  Frac = Pne->UTrav[i] - floor( Pne->UTrav[i] );
    if ( Frac > ceil( Pne->UTrav[i] ) - Pne->UTrav[i] ) {
      Frac = ceil( Pne->UTrav[i] ) - Pne->UTrav[i];
	  }
		
		if ( Frac < 0.1 * PremiereFractionnalite && Count > 20 ) break;    
		else if ( Frac < 0.01 * PremiereFractionnalite && 0 ) break;
		else if ( Frac < 1.e-8 && 0 ) break;		
		
    #if VERBOSE_PNE
      printf(" Variable fractionnaire etudiee -> %d cout %e valeur %e UminTrav %lf UmaxTrav %lf SeuilDeFractionnalite %e\n",
               i,Pne->LTrav[i],Pne->UTrav[i],Pne->UminTrav[i],Pne->UmaxTrav[i],Pne->SeuilDeFractionnalite[i]); 
      printf("       ----> Fixation a Umin:\n");
    #endif
		       
    memcpy( (char *) UStrongBranching, (char *) UTrav , NombreDeVariablesTrav * sizeof( double ) );
    NbVarComplAGauche = 0;
    DeltaXAGauche     = UTrav[i];

    PNE_StrongBranchingGraphedeConflit( Pne, i, 0.0 );
		
    SPX_DualStrongBranching(
                   (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur,
                   /* La variable a brancher */
                   i, 
                   LTrav[i], 
                   UminTrav[i],
                   UminTrav[i], 
	                 /* Sortie */
		               &CoutReduitInf,
                   &ExistenceDUneSolAGauche,
                   &TypeDeSolution      ,
                   UStrongBranching , 
                   PosVarAGauche     ,                   
                   &NbVarComplAGauche,                   
                   ComplBaseAGauche  ,             
		               /* Donnees complementaires */
                   NombreDeVariablesTrav,
                   TypeDeBorneTrav, 
                   NombreDeContraintesTrav,
		               /* Coupes */
                   Pne->Coupes.NombreDeContraintes,
                   Pne->Coupes.NbTerm,		   
                   PositionDeLaVariableDEcartAGauche	  
                           );     
    /* Calcul du critere apres strong branching */
    if ( ExistenceDUneSolAGauche == OUI_SPX ) {
      ExistenceSolutionAGauche = OUI_PNE;      
      for ( CritereAGauche = Pne->Z0 , Var = 0 ; Var < NombreDeVariablesTrav ; Var++ ) {
        CritereAGauche += LTrav[Var] * UStrongBranching[Var];
      }			
    }
    else {								  
      ExistenceSolutionAGauche = NON_PNE;    
      CritereAGauche = CritereInfini; 
      #if VERBOSE_PNE
        printf("Pas de solution a gauche\n"); 
      #endif      	
    }		
    
    #if VERBOSE_PNE
      printf("             CritereAGauche : %e \n",CritereAGauche); 
    #endif
		
    #if VERBOSE_PNE
      printf("       ----> Fixation a Umax:\n");
    #endif
					
    memcpy( (char *) UStrongBranching, (char *) UTrav , NombreDeVariablesTrav * sizeof( double ) );	      
    NbVarComplADroite = 0;
    DeltaXADroite     = UmaxTrav[i] - UTrav[i];

	  PNE_StrongBranchingGraphedeConflit( Pne, i, 1.0 );
	
    SPX_DualStrongBranching(
                   (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur,
                   /* La variable a brancher */
                   i,
                   LTrav[i], 
                   UmaxTrav[i],
                   UmaxTrav[i],
	                 /* Sortie */
		               &CoutReduitSup,
                   &ExistenceDUneSolADroite,
                   &TypeDeSolution     ,		       
                   UStrongBranching, 
                   PosVarADroite       ,                   
                   &NbVarComplADroite,                   
                   ComplBaseADroite  ,
		               /* Donnees complementaires */
                   NombreDeVariablesTrav,
                   TypeDeBorneTrav, 	     
                   NombreDeContraintesTrav,   
		               /* Coupes */
                   Pne->Coupes.NombreDeContraintes,
                   Pne->Coupes.NbTerm,		   		   
                   PositionDeLaVariableDEcartADroite       
                            );    
    /* Calcul du critere après strong branching */
    if ( ExistenceDUneSolADroite == OUI_SPX ) {
      ExistenceSolutionADroite = OUI_PNE;						      
			for ( CritereADroite = Pne->Z0 , Var = 0 ; Var < NombreDeVariablesTrav ; Var++ ) { 
        CritereADroite += LTrav[Var] * UStrongBranching[Var];
      }			
    }
    else {						
      /* On multiplie par 2 pour que s'il n'y a pas de solution ni a entier inf. ni a entier sup., la variable
         puisse etre choisie */
      ExistenceSolutionADroite = NON_PNE;
      CritereADroite = 2 * CritereInfini; 
      #if VERBOSE_PNE
        printf("Pas de solution a entier sup\n"); 
      #endif
    }
	     
    #if VERBOSE_PNE
      printf("              CritereADroite : %e\n",CritereADroite); 
    #endif		
		
    PNE_StrongBranchingClasserLeResultat( Pne                        , ChoixDuTypeDeVariation           ,
                                          ExistenceSolutionAGauche   , ExistenceSolutionADroite         ,
                                          CritereAGauche             , DeltaXAGauche                    ,
					                                CritereADroite             , DeltaXADroite                    ,
			       	                            CritereInfini              , Marge                            ,
					                                &SupDeNouveauCritere       , &SupEcartDInstanciation          , i ,
                                          &ChoixDefinitif            , &ChoixAmeliorant                 ,
					                                SeuilNouveauCout           ,					  
                                          MinorantEspereAGauche      , MinorantEspereADroite            ,	  
                                          PositionDeLaVariableAGauche, NbVarDeBaseComplementairesAGauche,    
                                          ComplementDeLaBaseAGauche  , PosVarAGauche                    , 
                                          NbVarComplAGauche          , ComplBaseAGauche                 ,
                                          PositionDeLaVariableDEcartAGauche,	  
                                          PositionDeLaVariableADroite      , NbVarDeBaseComplementairesADroite,    
                                          ComplementDeLaBaseADroite        , PosVarADroite                    ,	 
                                          NbVarComplADroite                , ComplBaseADroite                 ,        
                                          PositionDeLaVariableDEcartADroite,
                                          BasesFilsDisponibles                     
                                         );
					    
		if ( ChoixAmeliorant == OUI_PNE || ChoixDefinitif == OUI_PNE ) {
      VariableSelectionnee = OUI_PNE;
      if ( Pne->NbVarGauche > 0 && Pne->NbVarDroite > 0 ) {
        Pne->NbVarGauche = 0;     
        Pne->NbVarDroite = 0;
      }
    }

    if ( ChoixDefinitif == OUI_PNE ) break;

    i = SuivFrac[i];
    Count++;
  }
 
  /* Si une variable a ete choisie et si elle fait partie d'une gub, on cherche a faire
     le branchement sur les gub */

  TesterLeBranchementSurLesGub = NON_PNE; /* NON_PNE*/
  # if UTILISER_LES_GUB == OUI_PNE	
	  TesterLeBranchementSurLesGub = OUI_PNE; 
	# endif
  if ( VariableSelectionnee == OUI_PNE && TesterLeBranchementSurLesGub == OUI_PNE ) {
	  if ( Noeud->ProfondeurDuNoeud <= PROFONDEUR_LIMITE_POUR_UTILISATION_DES_GUB || Pne->YaUneSolutionEntiere == NON_PNE ) {
      PNE_StrongBranchingTesterLesGUB( Pne );
		}
  }
  
  free( PosVarAGauche ); 
  free( ComplBaseAGauche ); 
  free( PosVarADroite ); 
  free( ComplBaseADroite ); 
  free( PositionDeLaVariableDEcartAGauche ); 
  free( PositionDeLaVariableDEcartADroite );
	
}

PasStrongBranching:

if ( Pne->VariableLaPlusFractionnaire >= 0 ) {
  #if VERBOSE_PNE
    printf(" Variable la plus fractionnaire: %d Valeur: %e \n",
             Pne->VariableLaPlusFractionnaire,Pne->UTrav[Pne->VariableLaPlusFractionnaire]);
  #endif
  if ( *BasesFilsDisponibles == OUI_PNE ) {
    if ( PositionDeLaVariableAGauche[Pne->VariableLaPlusFractionnaire] < 0 ||
         PositionDeLaVariableADroite[Pne->VariableLaPlusFractionnaire] < 0 ) { printf("Bug dans le strong branching\n"); exit(0); }
  }
  fflush(stdout);

  if ( *CalculerDesCoupes != OUI_PNE && 0 ) {     
	  /* test: on cherche les symetries (seulement colonnes identiques) */
    PNE_RechercheSymetries( Pne, Pne->VariableLaPlusFractionnaire, BasesFilsDisponibles );
  }

	
}

return;
}

/*----------------------------------------------------------------------------*/
/* Si une variable a ete choisie et si elle fait partie d'une gub, on cherche a faire
   le branchement sur les gub */
void PNE_StrongBranchingTesterLesGUB( PROBLEME_PNE * Pne )
{
char LaVariableInstancieeEstDansUneGub; int NbTGub; int GubChoisie; int NbVarNonFix;
int TaillePaquet1; int i; int Cnt; int il; int ilMax; int Var; int NombreDeGub;
int * NumeroDeContrainteDeLaGub; int * Mdeb; int * NbTerm; int * Nuvar; int * TypeDeBorne;
double * Xmin; double * Xmax; int Var0; int Prem; int VarPrec; double Frac; int * Suiv;
double * X; char Dernier;

NombreDeGub = Pne->NombreDeGub;
NumeroDeContrainteDeLaGub = Pne->NumeroDeContrainteDeLaGub;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
X = Pne->UTrav;

NbTGub = -1;
GubChoisie = -1;
for ( i = 0 ; i < NombreDeGub ; i++ ) { 
  Cnt = NumeroDeContrainteDeLaGub[i];
  if ( Cnt < 0 ) continue;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
	NbVarNonFix = 0;
	LaVariableInstancieeEstDansUneGub = NON_PNE;
  while ( il < ilMax ) {
		Var = Nuvar[il];		
		if ( TypeDeBorne[Var] != VARIABLE_FIXE && Xmin[Var] != Xmax[Var] ) {
			NbVarNonFix++;
      if ( Var == Pne->VariableLaPlusFractionnaire ) LaVariableInstancieeEstDansUneGub = OUI_PNE;
    }
    il++;
  }
  if ( LaVariableInstancieeEstDansUneGub == OUI_PNE ) {
    if ( NbVarNonFix > NbTGub && NbVarNonFix >= MIN_TERMES_GUB && NbVarNonFix >= MAX_TERMES_GUB ) {
		  GubChoisie = i;
			NbTGub = NbVarNonFix;
		}
	}			
}
  
if ( GubChoisie < 0 ) return;

Suiv = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
if ( Suiv == NULL ) return;

/* Si la variable instanciee se trouve dans une gub on instancie plutot la GUB */
/* Il y aura quand meme un pb de base de depart et de minorant espere de chaque cote */		
/* On instancie une partie des variables de la GUB */
/*printf("La variable selectionee dans la premiere partie est dans une GUB a plus de 2 termes \n");*/
Cnt = NumeroDeContrainteDeLaGub[GubChoisie];

/* On classe les variables de la GUB dans l'ordre croissant des fractionnalites puis on fait 2
   paquets de taille equivalente a 1 pres */

Prem = -1;
il = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
while ( il < ilMax ) {
  Var = Nuvar[il];	
	if ( TypeDeBorne[Var] != VARIABLE_FIXE && Xmin[Var] != Xmax[Var] ) {
    Frac = fabs( 0.5 - X[Var] );
    /* Classement de la variable */
    if ( Prem == -1 ) { /* C'est la premiere variable */
      Prem = Var;
      Suiv[Var] = -1;
    }
    else {
      /* C'est pas la premiere variable: on lui cherche un emplacement */
      Var0 = Prem;
      if ( Frac < fabs( 0.5 - X[Var0] ) ) {
        Prem = Var;
        Suiv[Var] = Var0; 
      }
      else {
			  /* C'est pas la plus petite fractionnalite */
			  Dernier = OUI_PNE;
        VarPrec = Var0;  
        Var0 = Suiv[Var0];
        while ( Var0 >= 0 ) {
          if ( Frac <fabs( 0.5 - X[Var0] ) ) {
            /* Emplacement trouve */
            Suiv[VarPrec] = Var;
            Suiv[Var] = Var0;
					  Dernier = NON_PNE;
            break; 
          }
          VarPrec = Var0;
          Var0 = Suiv[Var0];
        }
			 	if ( Dernier == OUI_PNE ) {
          /* C'est la plus mauvaise: classement de la variable a la fin de la liste */
          Suiv[VarPrec] = Var;
          Suiv[Var] = -1;
				}
			}
		}				
  }								
 	il++;
}
						
TaillePaquet1 = NbTGub / 2;
Pne->NbVarGauche = 0;
Pne->ValeurAGauche = Pne->ValeurDInstanciationPourLaGub[GubChoisie];
Var = Prem;
while ( Pne->NbVarGauche < TaillePaquet1 && Var >= 0 ) {
  Pne->PaquetDeGauche[Pne->NbVarGauche] = Var;        
  Pne->NbVarGauche++;
  Var = Suiv[Var];
}
Pne->NbVarDroite = 0;
Pne->ValeurADroite = Pne->ValeurDInstanciationPourLaGub[GubChoisie];
while ( Var >= 0 ) {
  Pne->PaquetDeDroite[Pne->NbVarDroite] = Var;        
  Pne->NbVarDroite++;   
  Var = Suiv[Var];
}
					
/* Choix de l'instanciation terminee */
/*
printf("Instanciation sur une GUB NbVarGauche %d NbVarDroite %d ValeurDInstanciation %d\n",
        Pne->NbVarGauche,Pne->NbVarDroite,Pne->ValeurDInstanciationPourLaGub[GubChoisie]);
*/
if ( Pne->NbVarGauche <= 0 || Pne->NbVarDroite <= 0 ) {
  /*
	printf("Bug dans PNE_StrongBranchingTesterLesGUB NbVarGauche %d NbVarDroite %d\n",Pne->NbVarGauche,Pne->NbVarDroite);
	*/
}

free( Suiv );

return;
}

/*----------------------------------------------------------------------------*/

void PNE_StrongBranchingGraphedeConflit( PROBLEME_PNE * Pne, int Var, double ValeurDeVar )
{
double * Xmin; double * Xmax; int Edge; int Noeud; int Complement; int Nv; int Pivot;
int * First; int * Adjacent; int * Next; 

return;

if ( Pne->ConflictGraph == NULL ) return;

printf("Application du conflict graph variable %d: %e\n",Var,ValeurDeVar);

Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;

Pivot = Pne->ConflictGraph->Pivot;
First = Pne->ConflictGraph->First;
Adjacent = Pne->ConflictGraph->Adjacent;
Next = Pne->ConflictGraph->Next;

if ( ValeurDeVar == 1.0 ) { Noeud = Var; Complement = Pivot + Var; }
else { Noeud = Pivot + Var; Complement = Var; }

printf("Pivot %d Noeud %d Complement %d\n",Pivot,Noeud,Complement);

Edge = First[Noeud];
while ( Edge >= 0 ) {
  Nv = Adjacent[Edge];
	/* Attention a ne pas prendre le complement */
	if ( Nv == Complement ) goto NextEdge;
  if ( Nv < Pivot ) {
	  Var = Nv;
		/* On ne doit pas avoir X[Var] = 1.0 */
		if ( Xmin[Var] > 0.0001 ) {
		  printf("Instanciation infaisable\n");
			return;
		}
		if ( Xmin[Var] == Xmax[Var] ) goto NextEdge;
		ValeurDeVar = 0.0;				
		printf("Variable %d fixee a %e\n",Var,ValeurDeVar);
		fflush( stdout );
    PNE_StrongBranchingGraphedeConflit( Pne, Var, ValeurDeVar );
	}
  else {
    /* La valeur borne inf est interdite pour la variable */
		/* On doit donc fixer la variable a Umax et fixer les voisins de ce noeud */
	  Var = Nv - Pivot;
		/* On ne doit pas avoir X[Var] = 0.0 */
		if ( Xmax[Var] < 0.9999 ) {
		  printf("Instanciation infaisable\n");
			return;
		}		
		if ( Xmin[Var] == Xmax[Var] ) goto NextEdge;
		ValeurDeVar = 1.0;						
		printf("Variable %d fixee a %e\n",Var,ValeurDeVar);
		fflush( stdout );
    PNE_StrongBranchingGraphedeConflit( Pne, Var, ValeurDeVar );		
	}
	NextEdge:
  Edge = Next[Edge];
}			

return;
}
