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

   FONCTION: Allocations et liberation du probleme 
                  
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
 
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_fonctions.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# include "prs_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_AllocProbleme( PROBLEME_PNE * Pne, 
                        int     NombreDeVariablesE  ,
		        int *   TypeDeVariableE     ,    
		        int *   TypeDeBorneE        ,
		        double * UmaxE               ,   
		        double * UminE               ,
		        int     NombreDeContraintesE,
		        int *   MdebE               ,
		        int *   NbtermE             ,
		        int *   IndicesColonnesE    , 
		        double * AE           
                      )
{
int NbVarAlloc; int NbCntAlloc; int NbGub; char Gub; int i; int ilMax;
int Cnt       ; int il        ; int Var  ; int NbV; int NbVmxGub     ;

Pne->ProblemeSpxDuSolveur = NULL;
Pne->ProblemeSpxDuNoeudRacine = NULL;
Pne->MatriceDesContraintesAuNoeudRacine = NULL;

NbVarAlloc = NombreDeVariablesE   + INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_VARIABLES_PNE;
NbCntAlloc = NombreDeContraintesE + INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_CONTRAINTES_PNE;  

#if VERBOSE_PNE
  printf("Allocations memoire du PNE:\n");
  printf("Nombre de variables allouees: %d\n", NbVarAlloc);
  printf("Nombre de contraintes allouees: %d\n", NbCntAlloc);
#endif

/* Calcul d'un majorant du nombre de Gub */
/* Recherche de GUB */
NbGub    = 0;
NbVmxGub = -1;
for ( Cnt = 0 ; Cnt < NombreDeContraintesE ; Cnt++ ) {
  il    = MdebE[Cnt];
  ilMax = il + NbtermE[Cnt];
  Gub   = OUI_PNE;
  NbV   = 0;
  if ( NbtermE[Cnt] <= 2 ) continue;
  while ( il < ilMax ) {
    Var = IndicesColonnesE[il];
    if ( TypeDeBorneE[Var] != VARIABLE_FIXE ) {
      if ( TypeDeVariableE[Var] != ENTIER || ( AE[il] != 1.0 && AE[il] != -1.0 ) ) {
        if ( fabs( UmaxE[Var] - UminE[Var] ) > ZERO_VARFIXE && TypeDeBorneE[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
          Gub = NON_PNE;
          break;
	}
      }
    }
    NbV++;
    il++;
  }  
  if ( Gub == OUI_PNE ) {
    if ( NbV > NbVmxGub ) NbVmxGub = NbV;
    NbGub++;
  }
}

if ( NbVmxGub <= 0 ) NbVmxGub = 1;

/* Estimation par exces du nombre de Gub, reduit apres les avoir determine */
NbGub = NbCntAlloc;

Pne->NombreDeVariablesAllouees   = NbVarAlloc;
Pne->NombreDeContraintesAllouees = NbCntAlloc;

Pne->CorrespondanceVarEntreeVarNouvelle = (int *) malloc( NbVarAlloc * sizeof( int ) );

Pne->VariableAInverser      = (char *)   malloc( NbVarAlloc * sizeof( char ) );

Pne->VariableElimineeSansValeur = (int *)   malloc( NbVarAlloc * sizeof( int ) );

Pne->NumeroDesVariablesNonFixes         = (int *) malloc( NbVarAlloc * sizeof( int ) );
Pne->NumerosDesVariablesEntieresTrav    = (int *) malloc( NbVarAlloc * sizeof( int ) );
Pne->TypeDeVariableTrav                 = (int *) malloc( NbVarAlloc * sizeof( int ) );
Pne->SeuilDeFractionnalite              = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->VariableBinaireBigM                = (char *) malloc( NbVarAlloc * sizeof( char ) );
Pne->TypeDeBorneTrav                    = (int *) malloc( NbVarAlloc * sizeof( int ) );
Pne->TypeDeBorneTravSv                  = (int *) malloc( NbVarAlloc * sizeof( int ) );

Pne->UTrav      = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->S1Trav     = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->S2Trav     = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->UmaxTrav   = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->UmaxTravSv = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->UminTrav   = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->UminTravSv = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->UmaxEntree = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->UminEntree = (double *) malloc( NbVarAlloc * sizeof( double ) );

Pne->LTrav        = (double *) malloc( NbVarAlloc * sizeof( double ) );
Pne->CoutsReduits = (double *) malloc( NbVarAlloc * sizeof( double ) );

Pne->BTrav              = (double *) malloc( NbCntAlloc * sizeof( double ) );
Pne->SensContrainteTrav = (char *)   malloc( NbCntAlloc * sizeof( char   ) );
Pne->ContrainteSaturee  = (int *)   malloc( NbCntAlloc * sizeof( int   ) );

Pne->CorrespondanceCntPneCntEntree     = (int *)   malloc( NbCntAlloc * sizeof( int   ) );
Pne->VariablesDualesDesContraintesTrav = (double *) malloc( NbCntAlloc * sizeof( double ) );

Pne->VariablesDualesDesContraintesTravEtDesCoupes = NULL;
Pne->TailleAlloueeVariablesDualesDesContraintesTravEtDesCoupes = 0;

Pne->MdebTrav   = (int *) malloc( NbCntAlloc * sizeof( int ) );
Pne->NbTermTrav = (int *) malloc( NbCntAlloc * sizeof( int ) );

for ( ilMax = -1 , i = 0 ; i < NombreDeContraintesE ; i++ ) {
  if ( ( MdebE[i] + NbtermE[i] - 1 ) > ilMax ) ilMax = MdebE[i] + NbtermE[i] - 1;
}

/*ilMax+= Pne->NombreDeContraintesAllouees;*/ /* Afin de pouvoir mettre tout de suite le probleme sous 
                                                 la forme standard si on le souhaite */
ilMax += MARGE_EN_FIN_DE_CONTRAINTE * NombreDeContraintesE;
																						
ilMax+= INCREMENT_DALLOCATION_POUR_LA_MATRICE_DES_CONTRAINTES_PNE; 

#if VERBOSE_PNE
  printf("Taille allouee pour la matrice des contrainte: %d \n", ilMax);
#endif

Pne->TailleAlloueePourLaMatriceDesContraintes = ilMax;

Pne->NuvarTrav = (int *)   malloc( ilMax * sizeof( int   ) );
Pne->ATrav     = (double *) malloc( ilMax * sizeof( double ) );

Pne->NumeroDeContrainteDeLaGub = (int *) malloc( NbGub * sizeof( int ) );
Pne->ValeurDInstanciationPourLaGub = (int *) malloc( NbGub * sizeof( int ) );

Pne->CdebTrav          = (int *) malloc( NbVarAlloc * sizeof( int ) );
Pne->CNbTermTrav       = (int *) malloc( NbVarAlloc * sizeof( int ) );
Pne->CsuiTrav          = (int *) malloc( ilMax      * sizeof( int ) );
Pne->NumContrainteTrav = (int *) malloc( ilMax      * sizeof( int ) );

Pne->PaquetDeGauche    = (int *) malloc( NbVmxGub   * sizeof( int ) );
Pne->PaquetDeDroite    = (int *) malloc( NbVmxGub   * sizeof( int ) );
Pne->DimBranchementGub = NbVmxGub;

Pne->LaVariableAUneValeurFractionnaire = (int *) malloc( NbVarAlloc * sizeof( int ) );
Pne->SuivFrac                          = (int *) malloc( NbVarAlloc * sizeof( int ) );

Pne->UStrongBranching  = (double *) malloc( NbVarAlloc * sizeof( double ) );

Pne->CoutOpt = LINFINI_PNE;
Pne->UOpt    = (double *) malloc( NbVarAlloc * sizeof( double ) );

if ( 
     Pne->CorrespondanceVarEntreeVarNouvelle == NULL ||
     Pne->VariableAInverser		               == NULL ||
     Pne->VariableElimineeSansValeur         == NULL ||
     Pne->NumeroDesVariablesNonFixes         == NULL ||		 
     Pne->NumerosDesVariablesEntieresTrav    == NULL ||
     Pne->TypeDeVariableTrav                 == NULL ||
		 Pne->SeuilDeFractionnalite              == NULL ||
		 Pne->VariableBinaireBigM                == NULL ||
     Pne->TypeDeBorneTrav                    == NULL ||
     Pne->TypeDeBorneTravSv                  == NULL ||
     Pne->UTrav                              == NULL ||
     Pne->S1Trav                             == NULL ||
     Pne->S2Trav                             == NULL ||
     Pne->UmaxTrav                           == NULL ||
     Pne->UmaxTravSv                         == NULL ||
     Pne->UminTrav                           == NULL ||
     Pne->UminTravSv                         == NULL ||
     Pne->UmaxEntree                         == NULL ||
     Pne->UminEntree                         == NULL ||		 		 
     Pne->LTrav                              == NULL ||
     Pne->CoutsReduits                       == NULL ||
     Pne->BTrav                              == NULL ||
     Pne->SensContrainteTrav                 == NULL ||
     Pne->CorrespondanceCntPneCntEntree      == NULL || 
     Pne->VariablesDualesDesContraintesTrav  == NULL ||
     Pne->MdebTrav                           == NULL ||
     Pne->NbTermTrav                         == NULL ||
     Pne->NuvarTrav                          == NULL ||
     Pne->ATrav                              == NULL ||
     Pne->NumeroDeContrainteDeLaGub          == NULL ||     
     Pne->ValeurDInstanciationPourLaGub      == NULL ||     
     Pne->CdebTrav                           == NULL ||
     Pne->CNbTermTrav                        == NULL ||
     Pne->CsuiTrav                           == NULL ||
     Pne->NumContrainteTrav                  == NULL ||
     Pne->PaquetDeGauche                     == NULL ||
     Pne->PaquetDeDroite                     == NULL ||
     Pne->LaVariableAUneValeurFractionnaire  == NULL ||
     Pne->SuivFrac                           == NULL ||
     Pne->UStrongBranching                   == NULL ||
     Pne->UOpt                               == NULL 
   ) {

  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AllocProbleme \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

Pne->ContrainteActivable = NULL;
 
Pne->NombreDeCoupesCalculees = 0;
Pne->Coefficient_CG         = NULL;
Pne->IndiceDeLaVariable_CG  = NULL;
Pne->ValeurLocale = NULL;
Pne->IndiceLocal  = NULL;
Pne->ContrainteKnapsack = NULL;
Pne->CntDeBorneSupVariable = NULL;
Pne->CntDeBorneInfVariable = NULL;
Pne->ContrainteMixte = NULL;
Pne->FoisCntSuccesMirMarchandWolseyTrouvees = NULL;

Pne->CoupesCalculees = NULL;
Pne->Coupes.B = NULL;
Pne->Coupes.PositionDeLaVariableDEcart = NULL;
Pne->Coupes.PositionDeLaVariableDEcartAGauche = NULL;
Pne->Coupes.PositionDeLaVariableDEcartADroite = NULL;
Pne->Coupes.Mdeb = NULL;
Pne->Coupes.NbTerm = NULL;
Pne->Coupes.Nuvar = NULL;
Pne->Coupes.A = NULL;
Pne->Coupes.TypeDeCoupe = NULL;

Pne->CoutsReduitsAuNoeudRacine = NULL;
Pne->PositionDeLaVariableAuNoeudRacine = NULL;
/*
Pne->NumeroDeVariableCoutReduit = NULL;
Pne->CoutsReduitsAuNoeudRacineFoisDeltaBornes = NULL;
*/

Pne->ProbingOuNodePresolve = NULL;
Pne->ConflictGraph = NULL;
Pne->Cliques = NULL;
Pne->CoupesDeProbing = NULL;
Pne->ContraintesDeBorneVariable = NULL;
Pne->CoupesKNegligees = NULL;
Pne->CoupesGNegligees = NULL;

/* Infos qui seront utilisees par le postsolve */
Pne->NombreDOperationsDePresolve = 0; 
Pne->TailleTypeDOperationDePresolve = 0;
Pne->TypeDOperationDePresolve = NULL;
Pne->IndexDansLeTypeDOperationDePresolve = NULL;

Pne->IndexLibreVecteurDeSubstitution = 0;
Pne->NbVariablesSubstituees = 0;
Pne->NumeroDesVariablesSubstituees = NULL;
Pne->CoutDesVariablesSubstituees = NULL;
Pne->ContrainteDeLaSubstitution = NULL;
Pne->ValeurDeLaConstanteDeSubstitution = NULL;
Pne->IndiceDebutVecteurDeSubstitution = NULL;
Pne->NbTermesVecteurDeSubstitution = NULL;
Pne->CoeffDeSubstitution = NULL;
Pne->NumeroDeVariableDeSubstitution = NULL;

Pne->NbCouplesDeVariablesColineaires = 0;
Pne->PremiereVariable = NULL;
Pne->XminPremiereVariable = NULL;
Pne->XmaxPremiereVariable = NULL;
Pne->DeuxiemeVariable = NULL;
Pne->XminDeuxiemeVariable = NULL;
Pne->XmaxDeuxiemeVariable = NULL;
Pne->ValeurDeNu = NULL;

Pne->NbLignesSingleton = 0;
Pne->NumeroDeLaContrainteSingleton = NULL;
Pne->VariableDeLaContrainteSingleton = NULL;
Pne->SecondMembreDeLaContrainteSingleton = NULL;

Pne->NbForcingConstraints = 0;
Pne->NumeroDeLaForcingConstraint = NULL;

Pne->NbSuppressionsDeContraintesColineaires = 0;
Pne->ContrainteConservee = NULL; 	
Pne->ContrainteSupprimee = NULL; 	

Pne->NombreDeContraintesInactives = 0;
Pne->NumeroDesContraintesInactives = NULL;
	 
return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLeNombreDeVariables( PROBLEME_PNE * Pne )
{
int NbVarAlloc;
/*
printf(" Augmentation du nombre de variables\n"); fflush(stdout);
*/
NbVarAlloc = Pne->NombreDeVariablesAllouees + INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_VARIABLES_PNE;

Pne->NombreDeVariablesAllouees = NbVarAlloc;

Pne->CorrespondanceVarEntreeVarNouvelle = (int *) realloc( Pne->CorrespondanceVarEntreeVarNouvelle , NbVarAlloc * sizeof( int ) );

Pne->VariableAInverser = (char *)   realloc( Pne->VariableAInverser      , NbVarAlloc * sizeof( char   ) );

Pne->VariableElimineeSansValeur = (int *) realloc( Pne->VariableElimineeSansValeur , NbVarAlloc * sizeof( int ) );

Pne->NumeroDesVariablesNonFixes         = (int *) realloc( Pne->NumeroDesVariablesNonFixes , NbVarAlloc * sizeof( int ) );
Pne->NumerosDesVariablesEntieresTrav    = (int *) realloc( Pne->NumerosDesVariablesEntieresTrav , NbVarAlloc * sizeof( int ) );
Pne->TypeDeVariableTrav                 = (int *) realloc( Pne->TypeDeVariableTrav              , NbVarAlloc * sizeof( int ) );
Pne->SeuilDeFractionnalite              = (double *) realloc( Pne->SeuilDeFractionnalite        , NbVarAlloc * sizeof( double ) );
Pne->VariableBinaireBigM                = (char *) realloc( Pne->VariableBinaireBigM            , NbVarAlloc * sizeof( char ) );
Pne->TypeDeBorneTrav                    = (int *) realloc( Pne->TypeDeBorneTrav                 , NbVarAlloc * sizeof( int ) );
Pne->TypeDeBorneTravSv                  = (int *) realloc( Pne->TypeDeBorneTravSv               , NbVarAlloc * sizeof( int ) );

Pne->UTrav      = (double *) realloc( Pne->UTrav      , NbVarAlloc * sizeof( double ) );
Pne->S1Trav     = (double *) realloc( Pne->S1Trav     , NbVarAlloc * sizeof( double ) );
Pne->S2Trav     = (double *) realloc( Pne->S2Trav     , NbVarAlloc * sizeof( double ) );
Pne->UmaxTrav   = (double *) realloc( Pne->UmaxTrav   , NbVarAlloc * sizeof( double ) );
Pne->UmaxTravSv = (double *) realloc( Pne->UmaxTravSv , NbVarAlloc * sizeof( double ) );
Pne->UminTrav   = (double *) realloc( Pne->UminTrav   , NbVarAlloc * sizeof( double ) );
Pne->UminTravSv = (double *) realloc( Pne->UminTravSv , NbVarAlloc * sizeof( double ) );
Pne->UmaxEntree = (double *) realloc( Pne->UmaxEntree , NbVarAlloc * sizeof( double ) );
Pne->UminEntree = (double *) realloc( Pne->UminEntree , NbVarAlloc * sizeof( double ) );

Pne->LTrav        = (double *) realloc( Pne->LTrav        , NbVarAlloc * sizeof( double ) );
Pne->CoutsReduits = (double *) realloc( Pne->CoutsReduits , NbVarAlloc * sizeof( double ) );

Pne->CdebTrav    = (int *) realloc( Pne->CdebTrav    , NbVarAlloc * sizeof( int ) );
Pne->CNbTermTrav = (int *) realloc( Pne->CNbTermTrav , NbVarAlloc * sizeof( int ) );

Pne->LaVariableAUneValeurFractionnaire = (int *) realloc( Pne->LaVariableAUneValeurFractionnaire , NbVarAlloc * sizeof( int ) );
Pne->SuivFrac                          = (int *) realloc( Pne->SuivFrac                          , NbVarAlloc * sizeof( int ) );

Pne->UStrongBranching = (double *) realloc( Pne->UStrongBranching , NbVarAlloc * sizeof( double ) );

Pne->UOpt = (double *) realloc( Pne->UOpt , NbVarAlloc * sizeof( double ) );

if ( 
     Pne->CorrespondanceVarEntreeVarNouvelle == NULL ||
     Pne->VariableAInverser                  == NULL ||
     Pne->VariableElimineeSansValeur         == NULL ||
     Pne->NumeroDesVariablesNonFixes         == NULL ||		 
     Pne->NumerosDesVariablesEntieresTrav    == NULL ||
     Pne->TypeDeVariableTrav                 == NULL ||
     Pne->SeuilDeFractionnalite              == NULL ||
     Pne->VariableBinaireBigM                == NULL ||		 
     Pne->TypeDeBorneTrav                    == NULL ||
     Pne->TypeDeBorneTravSv                  == NULL ||
     Pne->UTrav                              == NULL ||
     Pne->S1Trav                             == NULL ||
     Pne->S2Trav                             == NULL ||
     Pne->UmaxTrav                           == NULL ||
     Pne->UmaxTravSv                         == NULL ||
     Pne->UminTrav                           == NULL ||
     Pne->UminTravSv                         == NULL ||
     Pne->UmaxEntree                         == NULL ||
     Pne->UminEntree                         == NULL ||		 		 
     Pne->LTrav                              == NULL ||
     Pne->CoutsReduits                       == NULL ||
     Pne->CdebTrav                           == NULL ||
     Pne->CNbTermTrav                        == NULL ||
     Pne->LaVariableAUneValeurFractionnaire  == NULL ||
     Pne->SuivFrac                           == NULL ||
     Pne->UStrongBranching                   == NULL ||
     Pne->UOpt                               == NULL 
   ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AugmenterLeNombreDeVariables\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLeNombreDeContraintes( PROBLEME_PNE * Pne )
{
int NbCntAlloc;

/*printf(" Augmentation du nombre de contraintes\n"); fflush(stdout);*/

NbCntAlloc = Pne->NombreDeContraintesAllouees + INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_CONTRAINTES_PNE;  

Pne->NombreDeContraintesAllouees = NbCntAlloc;

Pne->BTrav              = (double *) realloc( Pne->BTrav              , NbCntAlloc * sizeof( double ) );

Pne->SensContrainteTrav = (char *)   realloc( Pne->SensContrainteTrav , NbCntAlloc * sizeof( char   ) );
Pne->ContrainteSaturee  = (int *)   realloc( Pne->ContrainteSaturee  , NbCntAlloc * sizeof( int   ) );

Pne->CorrespondanceCntPneCntEntree     = (int *)   realloc( Pne->CorrespondanceCntPneCntEntree     , NbCntAlloc * sizeof( int   ) );
Pne->VariablesDualesDesContraintesTrav = (double *) realloc( Pne->VariablesDualesDesContraintesTrav , NbCntAlloc * sizeof( double ) );
   
Pne->MdebTrav   = (int *) realloc( Pne->MdebTrav , NbCntAlloc * sizeof( int   ) );
Pne->NbTermTrav = (int *) realloc( Pne->NbTermTrav , NbCntAlloc * sizeof( int   ) );

if ( 
     Pne->BTrav                             == NULL ||
     Pne->SensContrainteTrav                == NULL ||
     Pne->ContrainteSaturee                 == NULL ||
     Pne->CorrespondanceCntPneCntEntree     == NULL ||
     Pne->VariablesDualesDesContraintesTrav == NULL ||
     Pne->MdebTrav                          == NULL ||
     Pne->NbTermTrav                        == NULL 
   ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AugmenterLeNombreDeContraintes\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

if ( Pne->ContrainteKnapsack != NULL ) {
  Pne->ContrainteKnapsack = (char *) realloc( Pne->ContrainteKnapsack, NbCntAlloc * sizeof( char   ) );
  if ( Pne->ContrainteKnapsack == NULL ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AugmenterLeNombreDeContraintes\n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
	}
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLaTailleDeLaMatriceDesContraintes( PROBLEME_PNE * Pne )
{
int ilMax;

/*printf(" Augmentation de la taille de la matrice des contraintes\n"); fflush(stdout);*/

ilMax = Pne->TailleAlloueePourLaMatriceDesContraintes + INCREMENT_DALLOCATION_POUR_LA_MATRICE_DES_CONTRAINTES_PNE; 

Pne->TailleAlloueePourLaMatriceDesContraintes = ilMax;

Pne->NuvarTrav = (int *)   realloc( Pne->NuvarTrav , ilMax * sizeof( int   ) );
Pne->ATrav     = (double *) realloc( Pne->ATrav     , ilMax * sizeof( double ) );

Pne->CsuiTrav          = (int *) realloc( Pne->CsuiTrav          , ilMax * sizeof( int ) );
Pne->NumContrainteTrav = (int *) realloc( Pne->NumContrainteTrav , ilMax * sizeof( int ) );

if ( 
     Pne->NuvarTrav         == NULL ||
     Pne->ATrav             == NULL ||
     Pne->CsuiTrav          == NULL ||
     Pne->NumContrainteTrav == NULL 
   ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AugmenterLaTailleDeLaMatriceDesContraintes\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AllocCoupes( PROBLEME_PNE * Pne )
{
int NbCntAlloc; int ilMax;

NbCntAlloc = INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_CONTRAINTES_PNE;  
ilMax      = INCREMENT_DALLOCATION_POUR_LA_MATRICE_DES_CONTRAINTES_PNE; 
/*   
printf("Allocations memoire pour les coupes du PNE\n");
*/
Pne->Coupes.NombreDeContraintesAllouees              = NbCntAlloc;
Pne->Coupes.TailleAlloueePourLaMatriceDesContraintes = ilMax;
  
Pne->Coupes.B                                 = (double *) malloc( NbCntAlloc * sizeof( double ) );
Pne->Coupes.PositionDeLaVariableDEcart        = (char *)   malloc( NbCntAlloc * sizeof( char   ) );
Pne->Coupes.PositionDeLaVariableDEcartAGauche = (char *)   malloc( NbCntAlloc * sizeof( char   ) );
Pne->Coupes.PositionDeLaVariableDEcartADroite = (char *)   malloc( NbCntAlloc * sizeof( char   ) );

Pne->Coupes.Mdeb   = (int *) malloc( NbCntAlloc * sizeof( int ) );
Pne->Coupes.NbTerm = (int *) malloc( NbCntAlloc * sizeof( int ) );

Pne->Coupes.Nuvar = (int *)   malloc( ilMax * sizeof( int   ) );
Pne->Coupes.A     = (double *) malloc( ilMax * sizeof( double ) );

Pne->Coupes.TypeDeCoupe = (char *) malloc( NbCntAlloc * sizeof( char ) );

if ( 
     Pne->Coupes.B                                 == NULL || Pne->Coupes.PositionDeLaVariableDEcart        == NULL ||
     Pne->Coupes.PositionDeLaVariableDEcartAGauche == NULL || Pne->Coupes.PositionDeLaVariableDEcartADroite == NULL ||
     Pne->Coupes.Mdeb                              == NULL || Pne->Coupes.NbTerm                            == NULL ||
     Pne->Coupes.Nuvar                             == NULL || Pne->Coupes.A                                 == NULL ||
		 Pne->Coupes.TypeDeCoupe                       == NULL 
   ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AllocCoupes\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLeNombreDeCoupes( PROBLEME_PNE * Pne )
{
int NbCntAlloc;  
/*
printf(" Augmentation du nombre de coupes\n"); fflush(stdout);
*/
NbCntAlloc = Pne->Coupes.NombreDeContraintesAllouees + INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_CONTRAINTES_PNE;  

Pne->Coupes.NombreDeContraintesAllouees = NbCntAlloc;

Pne->Coupes.B                          = (double *) realloc( Pne->Coupes.B                         , NbCntAlloc * sizeof( double ) );
Pne->Coupes.PositionDeLaVariableDEcart = (char *)   realloc( Pne->Coupes.PositionDeLaVariableDEcart, NbCntAlloc * sizeof( char   ) );

Pne->Coupes.PositionDeLaVariableDEcartAGauche = (char *) realloc( Pne->Coupes.PositionDeLaVariableDEcartAGauche, NbCntAlloc * sizeof( char ) );
Pne->Coupes.PositionDeLaVariableDEcartADroite = (char *) realloc( Pne->Coupes.PositionDeLaVariableDEcartADroite, NbCntAlloc * sizeof( char ) );

Pne->Coupes.Mdeb   = (int *) realloc( Pne->Coupes.Mdeb   , NbCntAlloc * sizeof( int ) );
Pne->Coupes.NbTerm = (int *) realloc( Pne->Coupes.NbTerm , NbCntAlloc * sizeof( int ) );

Pne->Coupes.TypeDeCoupe = (char *) realloc( Pne->Coupes.TypeDeCoupe , NbCntAlloc * sizeof( char ) );

if ( 
     Pne->Coupes.B                                 == NULL || Pne->Coupes.PositionDeLaVariableDEcart        == NULL ||
     Pne->Coupes.PositionDeLaVariableDEcartAGauche == NULL || Pne->Coupes.PositionDeLaVariableDEcartADroite == NULL ||
     Pne->Coupes.Mdeb                              == NULL || Pne->Coupes.NbTerm                            == NULL ||
		 Pne->Coupes.TypeDeCoupe                       == NULL 
   ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AugmenterLeNombreDeCoupes\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLaTailleDeLaMatriceDesCoupes( PROBLEME_PNE * Pne )
{
int ilMax;
/*
printf(" Augmentation de la taille de la matrice des coupes\n"); fflush(stdout);
*/
ilMax = Pne->Coupes.TailleAlloueePourLaMatriceDesContraintes + INCREMENT_DALLOCATION_POUR_LA_MATRICE_DES_CONTRAINTES_PNE; 

Pne->Coupes.TailleAlloueePourLaMatriceDesContraintes = ilMax;


Pne->Coupes.Nuvar = (int *)   realloc( Pne->Coupes.Nuvar, ilMax * sizeof( int   ) );
Pne->Coupes.A     = (double *) realloc( Pne->Coupes.A    , ilMax * sizeof( double ) );

if ( Pne->Coupes.Nuvar == NULL || Pne->Coupes.A == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AugmenterLaTailleDeLaMatriceDesCoupes\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_LibereProbleme( PROBLEME_PNE * Pne )
{
int i; COUPE_CALCULEE ** Coupe; 

if ( Pne == NULL ) return;

if ( Pne->ProblemeSpxDuSolveur != NULL ) {  
  SPX_LibererProbleme( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur);
  Pne->ProblemeSpxDuSolveur = NULL;
}

if ( Pne->ProblemeSpxDuNoeudRacine != NULL ) {  
  SPX_LibererProbleme( (PROBLEME_SPX *) Pne->ProblemeSpxDuNoeudRacine );
  Pne->ProblemeSpxDuNoeudRacine = NULL;
}

if ( Pne->MatriceDesContraintesAuNoeudRacine != NULL ) {
  PNE_LibererMatriceDeContraintesDuSimplexeAuNoeudRacine( Pne->MatriceDesContraintesAuNoeudRacine );
}

/* Par precaution liberation des structures du BB au cas ou on serait sorti
   suite a une erreur interne */
if ( Pne->ProblemeBbDuSolveur != NULL ) {
  BB_BranchAndBoundDesallouerProbleme( (BB *) Pne->ProblemeBbDuSolveur );   
}

if ( Pne->ProblemePrsDuSolveur != NULL ) {
  PRS_LiberationStructure( (PRESOLVE *) Pne->ProblemePrsDuSolveur );
}

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  MEM_Quit( Pne->Tas );
	return;
# endif

free( Pne->CorrespondanceVarEntreeVarNouvelle );
free( Pne->VariableAInverser );
free( Pne->VariableElimineeSansValeur );
free( Pne->NumeroDesVariablesNonFixes );
free( Pne->NumerosDesVariablesEntieresTrav );
free( Pne->TypeDeVariableTrav );
free( Pne->SeuilDeFractionnalite );
free( Pne->VariableBinaireBigM );
free( Pne->TypeDeBorneTrav );
free( Pne->TypeDeBorneTravSv );
free( Pne->UTrav );
free( Pne->S1Trav );
free( Pne->S2Trav );
free( Pne->UmaxTrav );
free( Pne->UmaxTravSv );
free( Pne->UminTrav );
free( Pne->UminTravSv );
free( Pne->UmaxEntree );
free( Pne->UminEntree );
free( Pne->LTrav );
free( Pne->CoutsReduits );
free( Pne->BTrav );  
free( Pne->SensContrainteTrav );
free( Pne->ContrainteSaturee );
free( Pne->ContrainteActivable );
free( Pne->CorrespondanceCntPneCntEntree );
free( Pne->VariablesDualesDesContraintesTrav );
free( Pne->VariablesDualesDesContraintesTravEtDesCoupes );
free( Pne->MdebTrav );
free( Pne->NbTermTrav );
free( Pne->NuvarTrav );
free( Pne->ATrav );
free( Pne->NumeroDeContrainteDeLaGub );
free( Pne->ValeurDInstanciationPourLaGub );
free( Pne->CdebTrav );
free( Pne->CNbTermTrav );
free( Pne->CsuiTrav );
free( Pne->NumContrainteTrav );
free( Pne->PaquetDeGauche );
free( Pne->PaquetDeDroite );
free( Pne->LaVariableAUneValeurFractionnaire );
free( Pne->SuivFrac );
free( Pne->UStrongBranching );
free( Pne->Coefficient_CG );
free( Pne->IndiceDeLaVariable_CG );
free( Pne->ValeurLocale );
free( Pne->IndiceLocal );
free( Pne->ContrainteKnapsack );
free( Pne->CntDeBorneSupVariable );
free( Pne->CntDeBorneInfVariable );
free( Pne->ContrainteMixte );
free( Pne->FoisCntSuccesMirMarchandWolseyTrouvees );

free( Pne->UOpt );

if ( Pne->ProbingOuNodePresolve != NULL ) {
  free( Pne->ProbingOuNodePresolve->Buffer );
  free( Pne->ProbingOuNodePresolve );	
}

if ( Pne->ConflictGraph != NULL ) {
  free( Pne->ConflictGraph->First );
  free( Pne->ConflictGraph->Adjacent );
  free( Pne->ConflictGraph->Next );
  free( Pne->ConflictGraph );
}

if ( Pne->Cliques != NULL ) {
  free( Pne->Cliques->First );
  free( Pne->Cliques->NbElements );
  free( Pne->Cliques->Noeud );
  free( Pne->Cliques->LaCliqueEstDansLePool );
  free( Pne->Cliques->CliqueDeTypeEgalite );
  free( Pne->Cliques->NumeroDeCliqueDuNoeud );	
  free( Pne->Cliques );
}

if ( Pne->CoupesDeProbing != NULL ) {
  free( Pne->CoupesDeProbing->SecondMembre );
  free( Pne->CoupesDeProbing->First );
  free( Pne->CoupesDeProbing->NbElements );
  free( Pne->CoupesDeProbing->Colonne );
  free( Pne->CoupesDeProbing->Coefficient );
  free( Pne->CoupesDeProbing->LaCoupDeProbingEstDansLePool );
  free( Pne->CoupesDeProbing );
}

if ( Pne->ContraintesDeBorneVariable != NULL ) {
  free( Pne->ContraintesDeBorneVariable->SecondMembre );
  free( Pne->ContraintesDeBorneVariable->First );
  free( Pne->ContraintesDeBorneVariable->Colonne );
  free( Pne->ContraintesDeBorneVariable->Coefficient );
  free( Pne->ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool );
  free( Pne->ContraintesDeBorneVariable );
}

if ( Pne->CoupesKNegligees != NULL ) {
  free( Pne->CoupesKNegligees->SecondMembre );
  free( Pne->CoupesKNegligees->First );
  free( Pne->CoupesKNegligees->NbElements );
  free( Pne->CoupesKNegligees->Colonne );
  free( Pne->CoupesKNegligees->Coefficient );
  free( Pne->CoupesKNegligees->LaCoupeEstDansLePool );
  free( Pne->CoupesKNegligees );
}

if ( Pne->CoupesGNegligees != NULL ) {
  free( Pne->CoupesGNegligees->SecondMembre );
  free( Pne->CoupesGNegligees->First );
  free( Pne->CoupesGNegligees->NbElements );
  free( Pne->CoupesGNegligees->Colonne );
  free( Pne->CoupesGNegligees->Coefficient );
  free( Pne->CoupesGNegligees->LaCoupeEstDansLePool );
  free( Pne->CoupesGNegligees );
}

/* Ce qui suit ne serait a faire que dans le cas d'une PNE */
if ( Pne->Coupes.NombreDeContraintesAllouees > 0 ) {
  free( Pne->Coupes.B );    
  free( Pne->Coupes.PositionDeLaVariableDEcart );    
  free( Pne->Coupes.PositionDeLaVariableDEcartAGauche );    
  free( Pne->Coupes.PositionDeLaVariableDEcartADroite );    
  free( Pne->Coupes.Mdeb );    
  free( Pne->Coupes.NbTerm );    
  free( Pne->Coupes.Nuvar);    
  free( Pne->Coupes.A );
  free( Pne->Coupes.TypeDeCoupe );
}

/* Par precaution, liberation des coupes au niveau pne */
if( Pne->NombreDeCoupesCalculees > 0 ) {
  Coupe = Pne->CoupesCalculees; /* Pointeur sur le tableau de pointeurs sur les coupes */  
  for ( i = 0 ; i < Pne->NombreDeCoupesCalculees ; i++ ) {
    free( Coupe[i]->Coefficient ); 
    free( Coupe[i]->IndiceDeLaVariable );
    free( Coupe[i] );
  }
  free( Pne->CoupesCalculees ); 
}

free( Pne->CoutsReduitsAuNoeudRacine );
free( Pne->PositionDeLaVariableAuNoeudRacine );
/*
free( Pne->NumeroDeVariableCoutReduit );
free( Pne->CoutsReduitsAuNoeudRacineFoisDeltaBornes );
*/

/* Informations pour le postsolve */
free( Pne->TypeDOperationDePresolve );
free( Pne->IndexDansLeTypeDOperationDePresolve );

free( Pne->NumeroDesVariablesSubstituees );
free( Pne->CoutDesVariablesSubstituees );
free( Pne->ContrainteDeLaSubstitution );
free( Pne->ValeurDeLaConstanteDeSubstitution );
free( Pne->IndiceDebutVecteurDeSubstitution );
free( Pne->NbTermesVecteurDeSubstitution );
free( Pne->CoeffDeSubstitution );
free( Pne->NumeroDeVariableDeSubstitution );

free( Pne->PremiereVariable );
free( Pne->XminPremiereVariable );
free( Pne->XmaxPremiereVariable );
free( Pne->DeuxiemeVariable );
free( Pne->XminDeuxiemeVariable );
free( Pne->XmaxDeuxiemeVariable );
free( Pne->ValeurDeNu );

free( Pne->NumeroDeLaContrainteSingleton );
free( Pne->VariableDeLaContrainteSingleton );
free( Pne->SecondMembreDeLaContrainteSingleton );

free( Pne->NumeroDeLaForcingConstraint );

free( Pne->ContrainteConservee );
free( Pne->ContrainteSupprimee );

free( Pne->NumeroDesContraintesInactives );

free( Pne->Controls );

free( Pne );

return;
}


