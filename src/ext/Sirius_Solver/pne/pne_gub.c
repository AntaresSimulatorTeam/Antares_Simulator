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

   FONCTION: En presence de contraintes Gub, generation d'un schema
             de branchement particulier.
                            
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

# define NOMBRE_MIN_DE_VARIABLES_A_ARBITRER  3

# define NOMBRE_MAX_DE_GUB_TESTEES  1000

/*----------------------------------------------------------------------------*/

char PNE_Gub( PROBLEME_PNE * Pne,
              int *   PosVarAGauche , int * ComplBaseAGauche,
              char *   PositionDeLaVariableDEcartAGauche      ,
              int *   PosVarADroite , int * ComplBaseADroite,
              char *   PositionDeLaVariableDEcartADroite      ,	      
	            double   CritereInfini, double Marge            ,
	            double * SupDeNouveauCritere , double SeuilNouveauCout,
	            /*  */
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
              int   * BasesFilsDisponibles
           )
{
int i  ; int j  ; int il       ; int ilMax; int Cnt ; char OnInverse; int NbVarCnt;
int Var; int * NumeroDeVariable ; int r    ; double S ; char NextCnt  ; char YaUn    ;
int * NombreDeTermesLibresDeLaGub; double * XmaxSV      ; double F      ; double X     ;   
int VariableInstanciee           ; double * MesureEntierete             ; 

int ExistenceDUneSolAGauche; int ExistenceDUneSolADroite; char TypeDeSolution;
int NbVarComplAGauche      ; int NbVarComplADroite      ;

char ExistenceSolutionAGauche; char ExistenceSolutionADroite;
char ChoixDuTypeDeVariation  ;

double CritereAGauche; double CritereADroite; char ChoixDefinitif; char ChoixAmeliorant;
double DeltaXAGauche ; double DeltaXADroite ;

int NbVarGauche  ; int * PaquetDeGauche; int NbVarDroite; int * PaquetDeDroite;
int ValeurAGauche; int ValeurADroite   ; char OnYVa      ; int NbGubTestees    ;

double SupEcartDInstanciation; char YaLaVariableFractionnaire; 

Pne->NbVarGauche = 0;
Pne->NbVarDroite = 0;

ChoixDefinitif = NON_PNE;

OnYVa = OUI_PNE;

printf(" Nombre de Gub : %d\n",Pne->NombreDeGub); fflush(stdout);
/*
if ( Pne->YaUneSolutionEntiere != OUI_PNE ) OnYVa = NON_PNE;
*/
if ( Pne->NombreDeGub <= 0 ) OnYVa = NON_PNE;

if ( OnYVa == NON_PNE ) return( ChoixDefinitif );

/*printf("Test des Gub\n");*/
SupEcartDInstanciation = 1.e-7;

NbGubTestees = 0;

NumeroDeVariable            = (int *)   malloc( Pne->NombreDeVariablesTrav * sizeof( int   ) );
NombreDeTermesLibresDeLaGub = (int *)   malloc( Pne->NombreDeGub           * sizeof( int   ) );
MesureEntierete             = (double *) malloc( Pne->NombreDeGub           * sizeof( double ) );
XmaxSV                      = (double *) malloc( Pne->DimBranchementGub     * sizeof( double ) );
PaquetDeGauche              = (int *)   malloc( Pne->DimBranchementGub     * sizeof( int   ) );
PaquetDeDroite              = (int *)   malloc( Pne->DimBranchementGub     * sizeof( int   ) );
if ( NumeroDeVariable == NULL || NombreDeTermesLibresDeLaGub == NULL || MesureEntierete == NULL ||
     XmaxSV           == NULL || PaquetDeGauche              == NULL || PaquetDeDroite  == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_Gub \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

/* Mesure de la fractionalite de l'ensemble des variables de chaque GUB */
for ( j = 0 ; j < Pne->NombreDeGub ; j++ ) {
  NombreDeTermesLibresDeLaGub[j] = 0;
  MesureEntierete            [j] = 2. * Pne->NombreDeVariablesTrav;
  Cnt = Pne->NumeroDeContrainteDeLaGub[j];
  if ( Cnt < 0 ) continue;
  /* Decompte du nombre de variables non encore fixees */
  NbVarCnt = 0;
  F        = 0.;
  il    = Pne->MdebTrav[Cnt];
  ilMax = il + Pne->NbTermTrav[Cnt];
  while ( il < ilMax ) {  
    Var = Pne->NuvarTrav [il];
    if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) {
      if ( fabs( Pne->UminTrav[Var] - Pne->UmaxTrav[Var] ) > TOLERANCE_SUR_LES_ENTIERS ) {
        /* Norme L1 */
        F+= fabs( 0.5 - Pne->UTrav[Var] );
        NbVarCnt++;
      }
    }
    il++;
  }
  /* Si pas assez de variables entieres, on n'arbitre pas sur la Gub */
  if ( NbVarCnt >= NOMBRE_MIN_DE_VARIABLES_A_ARBITRER ) {
    MesureEntierete[j] = F;    
  }
  NombreDeTermesLibresDeLaGub[j] = NbVarCnt;
}

/* On reclasse les Gub */
OnInverse = OUI_PNE;
while ( OnInverse == OUI_PNE ) {
  OnInverse = NON_PNE;
  for ( j = 0 ; j < Pne->NombreDeGub - 1 ; j++ ) {
    if ( Pne->NumeroDeContrainteDeLaGub[j] >= 0 && Pne->NumeroDeContrainteDeLaGub[j+1] >= 0 ) {
      if ( NombreDeTermesLibresDeLaGub[j] < NombreDeTermesLibresDeLaGub[j+1] ) {
        OnInverse = OUI_PNE;
        il = Pne->NumeroDeContrainteDeLaGub[j+1];
        Pne->NumeroDeContrainteDeLaGub[j+1] = Pne->NumeroDeContrainteDeLaGub[j];
        Pne->NumeroDeContrainteDeLaGub[j] = il;
	
	      F = MesureEntierete[j+1];
	      MesureEntierete[j+1] = MesureEntierete[j];
	      MesureEntierete[j] = F;

        il = NombreDeTermesLibresDeLaGub[j+1];
        NombreDeTermesLibresDeLaGub[j+1] = NombreDeTermesLibresDeLaGub[j];
	      NombreDeTermesLibresDeLaGub[j] = il;
	
      }
    }
  }
}

/* On balaye les contraintes Gub */
for ( j = 0 ; j < Pne->NombreDeGub && NbGubTestees < NOMBRE_MAX_DE_GUB_TESTEES ; j++ ) {
  Cnt = Pne->NumeroDeContrainteDeLaGub[j];
  if ( Cnt < 0 ) continue;
  /* Decompte du nombre de variables non encore fixees */
  NbVarCnt = 0;
  NextCnt  = NON_PNE;
  YaUn     = NON_PNE;
  YaLaVariableFractionnaire = NON;  
  il    = Pne->MdebTrav[Cnt];
  ilMax = il + Pne->NbTermTrav[Cnt];
  while ( il < ilMax ) {  
    Var = Pne->NuvarTrav [il];

    if ( Var == Pne->VariableLaPlusFractionnaire ) YaLaVariableFractionnaire = OUI;

    if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) {

      if ( fabs( Pne->UminTrav[Var] - Pne->UmaxTrav[Var] ) > TOLERANCE_SUR_LES_ENTIERS ) {
        /* La variable n'a pas été instanciee */
        NumeroDeVariable[NbVarCnt] = Var;
        NbVarCnt++;
	      if ( fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] ) < TOLERANCE_SUR_LES_ENTIERS ) {
	        /* La variable se trouve sur borne max apres optimisation relaxee */
 	        /*NextCnt = OUI_PNE;*/
	      }
      }
      else {
        /* La variable est soit instanciee, soit deja fixee en entree */
        if ( Pne->UminTrav[Var] == 0.0 ) {
          /* On verifie que la variable n'etait pas fixee des le depart */
          if ( Pne->TypeDeBorneTrav[Var] != VARIABLE_FIXE ) {    
 	        /*NextCnt = OUI_PNE;*/
  	      }
        }
        else {
          /* La variable a ete instanciee ou fixee en entree a 1 */	  
	        YaUn = OUI_PNE;
	        break;	  
        }
      }

    }
    
    il++;
  }

  /*printf("          Gub %d YaLaVariableFractionnaire %d\n",j,YaLaVariableFractionnaire);*/

  if ( YaUn == OUI_PNE ) continue;
  if ( NbVarCnt < NOMBRE_MIN_DE_VARIABLES_A_ARBITRER ) continue;
  if ( NextCnt == OUI_PNE ) continue;

  if ( YaLaVariableFractionnaire == NON ) continue;
  
  /* Classement des variables en fonction de U */
  OnInverse = OUI_PNE;
  while ( OnInverse == OUI_PNE ) {
    OnInverse = NON_PNE;
    for ( i = 0 ; i < NbVarCnt - 1 ; i++ ) {
      if ( Pne->UTrav[NumeroDeVariable[i]] < Pne->UTrav[NumeroDeVariable[i+1]] ) {
        Var = NumeroDeVariable[i+1];
        NumeroDeVariable[i+1] = NumeroDeVariable[i];
        NumeroDeVariable[i] = Var;
        OnInverse = OUI_PNE;
      }
    }
  }
    
  /* Determination des paquets a instancier a 0 */
  S = 0.0;
  r = -1;
  for ( i = 0 ; i < NbVarCnt ; i++ ) {
    /* Il faut que les variables ne soient pas sur borne ou presque */
    X = Pne->UTrav[NumeroDeVariable[i]];
    /*if ( X > 0.99) continue;*/
    S+= X;
    if ( /*X < 0.5*/ /* <-old new ->*/ S > 0.5 ) {  
      /* r = i-1; <- old */
      r = i;
      break;
    }
  }
  /* Partitionnement refuse  si r >= NbVarCnt - 1 */
  
  r = (int) floor( NbVarCnt * 0.5 ); /* Test */
  
  if ( r >= NbVarCnt - 1 ) continue;
  if ( r < 0 ) continue;
  if ( r == 0 || r >= NbVarCnt - 2 ) continue; /* Si une seule variable instanciee, on ne le fait pas */
  
  /*if ( r <= 0 ) continue;*/ /* Car cela revient a separer sur une variable */
  
  ValeurAGauche = 0;
  ValeurADroite = 0;
  /* Construction du paquet de gauche */
  NbVarGauche   = 0;
  DeltaXAGauche = 0.0/*1.0*/; /* Pourquoi pas 0 ? */
  
  printf(" gauche: \n");
  
  for ( i = 0 ; i <= r ; i++ ) {
    if ( NbVarGauche >= Pne->DimBranchementGub ) goto FinDeBoucle;
    PaquetDeGauche[NbVarGauche] = NumeroDeVariable[i];
    DeltaXAGauche+= Pne->UTrav[NumeroDeVariable[i]];
    
    printf(" %e ",Pne->UTrav[NumeroDeVariable[i]]);
    
    NbVarGauche++;
  }
  /*printf("\n");*/
  /* Construction du paquet de droite */
  NbVarDroite   = 0;
  DeltaXADroite = 0.0;

  printf("\n droite: \n");
  
  for ( i = r+1 ; i < NbVarCnt ; i++ ) {
    if ( NbVarDroite >= Pne->DimBranchementGub ) goto FinDeBoucle;
    PaquetDeDroite[NbVarDroite] = NumeroDeVariable[i];
    DeltaXADroite+= Pne->UTrav[NumeroDeVariable[i]];
    
    printf(" %e ",Pne->UTrav[NumeroDeVariable[i]]);
    
    NbVarDroite++;
  }
  printf("\n"); fflush(stdout);

  NbGubTestees++;
  
  memcpy( (char *) Pne->UStrongBranching, (char *) Pne->UTrav, Pne->NombreDeVariablesTrav * sizeof( double ) );  
  NbVarComplAGauche = 0;
  
  SPX_DualStrongBranchingGUB(
                   (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur,
		               /* Les variables a brancher */ 
                   NbVarGauche   ,
		               PaquetDeGauche,
		               XmaxSV, /* Tableau de sauvegarde dimension nombre de variables a brancher */
	                 /* Sortie */
                   &ExistenceDUneSolAGauche,
                   &TypeDeSolution         ,
                   Pne->UStrongBranching    ,  
                   PosVarAGauche           ,                   
                   &NbVarComplAGauche      ,                   
                   ComplBaseAGauche        ,
                   /* Donnees initiales du probleme (elles sont utilisees par SPX_RecupererLaSolution)*/ 
                   Pne->NombreDeVariablesTrav  , 
                   Pne->TypeDeBorneTrav        ,  
                   Pne->NombreDeContraintesTrav,
                   /* Dans le but de recuperer les informations sur les coupes*/ 
                   Pne->Coupes.NombreDeContraintes,
		               Pne->Coupes.NbTerm,		   		   
                   PositionDeLaVariableDEcartAGauche	  		   
                            );
  /* Calcul du critere apres strong branching */
  if ( ExistenceDUneSolAGauche == OUI_SPX ) {
    ExistenceSolutionAGauche = OUI_PNE;    
    for ( CritereAGauche = Pne->Z0 , Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) { 
      CritereAGauche+= Pne->LTrav[Var] * Pne->UStrongBranching[Var];
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
    printf("              CritereAGauche : %e \n",CritereAGauche); 
  #endif
  
  memcpy( (char *) Pne->UStrongBranching, (char *) Pne->UTrav , Pne->NombreDeVariablesTrav * sizeof( double ) );  
  NbVarComplADroite = 0;

  /*if ( NbVarDroite > Pne->DimBranchementGub ) exit(0);*/ /* A supprimer */
 
  SPX_DualStrongBranchingGUB(
                   (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur,
		   /* Les variables a brancher */ 
                   NbVarDroite,
		               PaquetDeDroite,
		               XmaxSV, /* Tableau de sauvegarde dimension nombre de variables a brancher */
	                 /* Sortie */
                   &ExistenceDUneSolADroite,
                   &TypeDeSolution         ,
                   Pne->UStrongBranching    ,  
                   PosVarADroite           ,                   
                   &NbVarComplADroite      ,                   
                   ComplBaseADroite        ,
                   /* Donnees initiales du probleme (elles sont utilisees par SPX_RecupererLaSolution)*/ 
                   Pne->NombreDeVariablesTrav  , 
                   Pne->TypeDeBorneTrav        ,  
                   Pne->NombreDeContraintesTrav,
                   /* Dans le but de recuperer les informations sur les coupes*/ 
                   Pne->Coupes.NombreDeContraintes,
	                 Pne->Coupes.NbTerm,		   		   
                   PositionDeLaVariableDEcartADroite	  		   
                            );
  /* Calcul du critere apres strong branching */
  if ( ExistenceDUneSolADroite == OUI_SPX ) {
    ExistenceSolutionADroite = OUI_PNE;
    for ( CritereADroite = Pne->Z0 , Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) { 
      CritereADroite+= Pne->LTrav[Var] * Pne->UStrongBranching[Var];
    }      
  }
  else { 
    /* On multiplie par 2 pour que s'il n'y a pas de solution ni a entier inf. ni a entier sup., la variable
       puisse etre choisie */
    ExistenceSolutionADroite = NON_PNE;
    CritereADroite = 2 * CritereInfini; 
    #if VERBOSE_PNE
      printf("Pas de solution a droite \n"); 
    #endif
  }
    
  #if VERBOSE_PNE
    printf("              CritereADroite : %e\n",CritereADroite);  
  #endif
  
  printf("CritereAGauche: %e CritereADroite: %e SeuilNouveauCout: %e\n",CritereAGauche,CritereADroite,SeuilNouveauCout);
  /*
  ChoixDuTypeDeVariation = PENTE_DE_LA_VARIATION;
  if ( DeltaXAGauche <= 0.0 ) {
    CritereAGauche = Pne->Critere;
    DeltaXAGauche  = 1.0;
  }
  if ( DeltaXADroite <= 0.0 ) {
    DeltaXADroite = Pne->Critere;
    DeltaXADroite = 0.0;
  }
  */
  ChoixDuTypeDeVariation = VALEUR_ABSOLUE_DE_LA_VARIATION;
  ChoixDuTypeDeVariation = PENTE_DE_LA_VARIATION;
  
  VariableInstanciee = -1;

  /* Mise en commentaires car test sur branchement a partir des couts reduits */
  
  PNE_StrongBranchingClasserLeResultat( Pne         , ChoixDuTypeDeVariation           ,
                                        ExistenceSolutionAGauche   , ExistenceSolutionADroite         ,
                                        CritereAGauche             , DeltaXAGauche                    ,
					                              CritereADroite             , DeltaXADroite                    ,
			       	                          CritereInfini              , Marge                            ,
			         	                        SupDeNouveauCritere        , &SupEcartDInstanciation          , VariableInstanciee,
                                        &ChoixDefinitif            , &ChoixAmeliorant                 ,
					                              SeuilNouveauCout           ,
                                        MinorantEspereAGauche      , MinorantEspereADroite            ,	  
                                        PositionDeLaVariableAGauche, NbVarDeBaseComplementairesAGauche,    
                                        ComplementDeLaBaseAGauche  ,	PosVarAGauche                   , 
                                        NbVarComplAGauche          , ComplBaseAGauche                 ,
                                        PositionDeLaVariableDEcartAGauche,	  
                                        PositionDeLaVariableADroite      , NbVarDeBaseComplementairesADroite,    
                                        ComplementDeLaBaseADroite        , PosVarADroite                    ,	 
                                        NbVarComplADroite                , ComplBaseADroite                 ,        
                                        PositionDeLaVariableDEcartADroite,
                                        BasesFilsDisponibles                     
                                       ); 
 				       
				       
  if ( ChoixAmeliorant == OUI_PNE ) { 
    Pne->ValeurAGauche = ValeurAGauche;
    Pne->ValeurADroite = ValeurADroite;
    Pne->NbVarGauche = NbVarGauche;
    Pne->NbVarDroite = NbVarDroite;
    memcpy( Pne->PaquetDeGauche , PaquetDeGauche , NbVarGauche * sizeof( int ) );
    memcpy( Pne->PaquetDeDroite , PaquetDeDroite , NbVarDroite * sizeof( int ) );
  }
  
  /* On se contente de ce partitionnement */
  if ( ChoixDefinitif == OUI_PNE ) {
    
    printf("ChoixDefinitif %d ChoixAmeliorant %d\n",ChoixDefinitif,ChoixAmeliorant);
    printf("Strong branching accepte sur la contrainte Gub %d  r %d  NbVarCnt %d\n",Cnt,r,NbVarCnt);
    
    break;
  }
  FinDeBoucle:
  continue;
}


free( NumeroDeVariable );
free( NombreDeTermesLibresDeLaGub );
NumeroDeVariable            = NULL;
NombreDeTermesLibresDeLaGub = NULL;

free( MesureEntierete );
free( XmaxSV );
free( PaquetDeGauche );
free( PaquetDeDroite );
MesureEntierete  = NULL;
XmaxSV           = NULL;
PaquetDeGauche   = NULL;
PaquetDeDroite   = NULL;

/*printf("Pne->NbVarGauche %d Pne->NbVarDroite %d SupDeNouveauCritere %e\n",Pne->NbVarGauche,Pne->NbVarDroite,*SupDeNouveauCritere);*/

return( ChoixDefinitif );

}

