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

   FONCTION: Recherche heuristique en tentant de fixer des variables
             proches de leurs bornes.
	     Attention, ne peut etre appelee qu'apres le calcul des
	     coupes et le strong branching car ca change les matrices
	     internes du simplexe.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_constantes_externes.h"
# include "spx_definition_arguments.h"
# include "spx_define.h"
# include "spx_fonctions.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define SEUIL_MIN_DE_FRACTIONNALITE_MOYENNE 2.e+2 
# define SEUIL_MIN_DE_FRACTIONNALITE         2.e+2

# define POURCENTAGE_MIN_DE_VARIABLES_ENTIERES 0.1

void PNE_HeuristiqueResoudreLeProblemeRestreint( PROBLEME_PNE * , int * , int * , int * , char * ,
                                                 double * , double * , char * , double * , double ,
																								 int );
																								 
/*----------------------------------------------------------------------------*/

void PNE_HeuristiqueResoudreLeProblemeRestreint( PROBLEME_PNE * Pne, int * PosVarHeuristique,
                                                 int * NbVarBaseComplementairesHeuristique,
						 int * ComplementDeLaBaseHeuristique,
                                                 char * PosVarEcartCoupesHeuristique,
						 double * VarDualesContraintesHeuristique,
						 double * CoutsRedHeuristique,
						 char * YaUneSolution,
						 double * Critere,
						 double CoutMax,			      
	                                         int   UtiliserCoutMax )
{
double Crit; int i; PROBLEME_SIMPLEXE Probleme;
/* Lancement d'un simplexe */
Probleme.Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;
Probleme.NombreMaxDIterations = -1; 
Probleme.DureeMaxDuCalcul     = -1.;

Probleme.CoutLineaire      = Pne->LTrav;
Probleme.X                 = Pne->UTrav;
Probleme.Xmin              = Pne->UminTrav;
Probleme.Xmax              = Pne->UmaxTrav;
Probleme.NombreDeVariables = Pne->NombreDeVariablesTrav;
Probleme.TypeDeVariable    = Pne->TypeDeBorneTrav;

Probleme.NombreDeContraintes                   = Pne->NombreDeContraintesTrav;
Probleme.IndicesDebutDeLigne                   = Pne->MdebTrav;
Probleme.NombreDeTermesDesLignes               = Pne->NbTermTrav;
Probleme.IndicesColonnes                       = Pne->NuvarTrav;
Probleme.CoefficientsDeLaMatriceDesContraintes = Pne->ATrav;
Probleme.Sens                                  = Pne->SensContrainteTrav;
Probleme.SecondMembre                          = Pne->BTrav;  

Probleme.ChoixDeLAlgorithme = SPX_DUAL;

Probleme.TypeDePricing               = PRICING_STEEPEST_EDGE;  
Probleme.FaireDuScaling              = OUI_SPX;  
Probleme.StrategieAntiDegenerescence = AGRESSIF;

Probleme.BaseDeDepartFournie        = OUI_SPX;
Probleme.PositionDeLaVariable       = PosVarHeuristique;
Probleme.NbVarDeBaseComplementaires = *NbVarBaseComplementairesHeuristique;
Probleme.ComplementDeLaBase         = ComplementDeLaBaseHeuristique;
  
Probleme.LibererMemoireALaFin  = NON_SPX;	

Probleme.CoutMax         = CoutMax;
Probleme.UtiliserCoutMax = UtiliserCoutMax; 

Probleme.NombreDeContraintesCoupes        = Pne->Coupes.NombreDeContraintes;
Probleme.BCoupes                          = Pne->Coupes.B;
Probleme.PositionDeLaVariableDEcartCoupes = PosVarEcartCoupesHeuristique; 
Probleme.MdebCoupes                       = Pne->Coupes.Mdeb;
Probleme.NbTermCoupes                     = Pne->Coupes.NbTerm;
Probleme.NuvarCoupes                      = Pne->Coupes.Nuvar;
Probleme.ACoupes                          = Pne->Coupes.A;

Probleme.CoutsMarginauxDesContraintes = VarDualesContraintesHeuristique;

Probleme.CoutsReduits = CoutsRedHeuristique;

Probleme.AffichageDesTraces = Pne->AffichageDesTraces;

Pne->ProblemeSpxDuSolveur = SPX_Simplexe( &Probleme , Pne->ProblemeSpxDuSolveur );

*YaUneSolution = OUI_PNE;
if ( Probleme.ExistenceDUneSolution != OUI_SPX ) {
  printf("--> On tombe sur une solution non realisable dans l'heuristique <--\n");
  *YaUneSolution = NON_PNE;
  return;
}
*NbVarBaseComplementairesHeuristique = Probleme.NbVarDeBaseComplementaires;

Crit = Pne->Z0;
for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) Crit+= Pne->LTrav[i] * Pne->UTrav[i];    

*Critere = Crit;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_Heuristique( PROBLEME_PNE * Pne, 
	       double CoutMax,			      
	       int   UtiliserCoutMax,                     
               int * PositionDeLaVariable,                 
               int   NbVarDeBaseComplementaires,           
               int * ComplementDeLaBase
                           )
{
int i; char OnFixe; int NombreDeMacroIterations ; int NombreMaxDeMacroIterations;

double * UTravSv; double * UminTravSv; double * UmaxTravSv;
double * CoutsRedHeuristique; double * VarDualesContraintesHeuristique;
int *   PosVarHeuristique  ; int *   ComplementDeLaBaseHeuristique  ; char *   PosVarEcartCoupesHeuristique;

int NbVarBaseComplementairesHeuristique; double Critere; int VariableAInstancier; int SeuilMinDeVariablesEntieres;

char LeNoeudEstTerminal; char YaUneSolution; 
double CritereAuMax; BB * Bb;
int * NuVarFrac; int Nb; int Var; int kMax; int k;

double * UminX; double * UmaxX; double * UX; int * PX;
double * UminM; double * UmaxM; double * UM; int * PM;

int NbX; 
int * CplX; char * PecX;
int * CplM; char * PecM;

return; /* Heuristique de fixation pas concluante il y a encore des problemes a regler */

Bb = (BB *) Pne->ProblemeBbDuSolveur;

/* 1er test avant les alloc: on regarde s'il y a lieu de faire l'heuristique */
/* La determination de la fractionnalite a deja ete faite */
if ( Pne->NombreDeVariablesAValeurFractionnaire <= 1 ) return;
if ( Pne->NormeDeFractionnalite/Pne->NombreDeVariablesAValeurFractionnaire > SEUIL_MIN_DE_FRACTIONNALITE_MOYENNE ) return;

SeuilMinDeVariablesEntieres = (int) ceil( POURCENTAGE_MIN_DE_VARIABLES_ENTIERES * Pne->NombreDeVariablesEntieresTrav );
if ( SeuilMinDeVariablesEntieres < 10 ) SeuilMinDeVariablesEntieres = 10;

/*SeuilMinDeVariablesEntieres = Pne->NombreDeVariablesEntieresTrav;*/
/*printf(" SeuilMinDeVariablesEntieres %d NombreDeVariablesEntieres %d \n",SeuilMinDeVariablesEntieres,Pne->NombreDeVariablesEntieresTrav);*/

if ( Pne->NombreDeVariablesAValeurFractionnaire > SeuilMinDeVariablesEntieres ) return;

printf("On tente l'heuristique \n");

NombreDeMacroIterations = 0;
NombreMaxDeMacroIterations = (int) (20 * Pne->NombreDeVariablesAValeurFractionnaire);
LeNoeudEstTerminal = NON_PNE;

VariableAInstancier = Pne->VariableLaPlusFractionnaire; /* Sauvegarde apres le strong branching */

/* Sauvegardes */
UTravSv             = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) ); 
UminTravSv          = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) ); 
UmaxTravSv          = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
PosVarHeuristique   = (int *)   malloc( Pne->NombreDeVariablesTrav * sizeof( int   ) );
CoutsRedHeuristique = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );


UminX = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
UmaxX = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
UX    = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
PX    = (int *)   malloc( Pne->NombreDeVariablesTrav * sizeof( int   ) );
UminM = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
UmaxM = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
UM    = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
PM    = (int *)   malloc( Pne->NombreDeVariablesTrav * sizeof( int   ) );
CplX  = (int *)   malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );
PecX  = (char *)   malloc( Pne->Coupes.NombreDeContraintes  * sizeof( char ) );
CplM  = (int *)   malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );
PecM  = (char *)   malloc( Pne->Coupes.NombreDeContraintes  * sizeof( char ) );


if ( UTravSv             == NULL || UminTravSv == NULL || UmaxTravSv == NULL || PosVarHeuristique == NULL ||
     CoutsRedHeuristique == NULL ) {
  printf("PNE, memoire insuffisante dans le sous programme PNE_Heuristique  \n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}
memcpy( (char *) UTravSv           , (char *) Pne->UTrav            , Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) UminTravSv        , (char *) Pne->UminTrav         , Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) UmaxTravSv        , (char *) Pne->UmaxTrav         , Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) PosVarHeuristique , (char *) PositionDeLaVariable , Pne->NombreDeVariablesTrav * sizeof( int   ) );

ComplementDeLaBaseHeuristique    = (int *)   malloc( Pne->NombreDeContraintesTrav                                  * sizeof( int   ) );
VarDualesContraintesHeuristique  = (double *) malloc( ( Pne->NombreDeContraintesTrav + Pne->Coupes.NombreDeContraintes ) * sizeof( double ) ); 
if ( ComplementDeLaBaseHeuristique == NULL || VarDualesContraintesHeuristique == NULL ) {
  printf("PNE, memoire insuffisante dans le sous programme PNE_Heuristique  \n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

NbVarBaseComplementairesHeuristique = NbVarDeBaseComplementaires;
memcpy( (char *) ComplementDeLaBaseHeuristique , (char *) ComplementDeLaBase  , Pne->NombreDeContraintesTrav * sizeof( int ) );

PosVarEcartCoupesHeuristique = (char *) malloc( Pne->Coupes.NombreDeContraintes * sizeof( char ) );
if ( PosVarEcartCoupesHeuristique == NULL ) {
  printf("PNE, memoire insuffisante dans le sous programme PNE_Heuristique  \n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}
memcpy( (char *) PosVarEcartCoupesHeuristique , (char *) Pne->Coupes.PositionDeLaVariableDEcart , Pne->Coupes.NombreDeContraintes * sizeof( char ) );

TentativeDeFixationDeBorne:

/* On regarde quelle borne on peut fixer */
if ( Pne->NombreDeVariablesAValeurFractionnaire > 0 ) {
  if ( NombreDeMacroIterations > NombreMaxDeMacroIterations ) {
    /* Echec */
    printf("Fin heuristique par depassement du nombre de macroiterations\n");
    goto ArretHeuristique;
  }
  /* On tente de fixer des bornes */
  /* On regarde si la fractionnalite de toutes les variables est inferieure a un seuil */
  OnFixe = OUI_PNE;

  if ( Pne->NombreDeVariablesAValeurFractionnaire > SeuilMinDeVariablesEntieres ) OnFixe = NON_PNE;
	
  if ( OnFixe == NON_PNE ) goto ArretHeuristique;

	
  /* On fixe toutes les variables qui ont pris une valeur entiere */
	/*
  for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
    if ( Pne->LaVariableAUneValeurFractionnaire[i] == OUI_PNE ) continue;
    if ( Pne->UTrav[i] - Pne->UminTrav[i] < Pne->UmaxTrav[i] - Pne->UTrav[i] ) {
       Pne->UmaxTrav[i] = Pne->UminTrav[i]; Pne->UTrav[i] = Pne->UminTrav[i];
    }
    else {
       Pne->UminTrav[i] = Pne->UmaxTrav[i]; Pne->UTrav[i] = Pne->UmaxTrav[i];      
    }
  }
	*/
	
  /* On fixe un paquet de variables les moins fractionnaires */ 
	NuVarFrac = (int *) malloc( Pne->NombreDeVariablesAValeurFractionnaire * sizeof( int ) );
	Nb = 0;
  i = Pne->PremFrac;
	while ( i >= 0 ) {
    NuVarFrac[Nb] = i;
		Nb++;	
	  i = Pne->SuivFrac[i];
	}
	k = 0;
	kMax = 10;
	for ( i = Nb-1; i >= 0 ; i-- ) {
    Var = NuVarFrac[i];
		if ( fabs( Pne->UTrav[Var] - Pne->UminTrav[Var] ) < fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] ) ) {
      Pne->UmaxTrav[Var] = Pne->UminTrav[Var]; 
      /*printf("Fixation de la variable %d U %e Umin %e Umax %e\n",Var,Pne->UTrav[Var],Pne->UminTrav[Var],Pne->UmaxTrav[Var]);*/
      Pne->UTrav[Var] = Pne->UminTrav[Var];
			k++;
		}
		else {
      Pne->UminTrav[Var] = Pne->UmaxTrav[Var]; 
      /*printf("Fixation de la variable %d U %e Umin %e Umax %e\n",Var,Pne->UTrav[Var],Pne->UminTrav[Var],Pne->UmaxTrav[Var]);*/
      Pne->UTrav[Var] = Pne->UmaxTrav[Var];		
			k++;
		}
    if ( k >= kMax ) break;
	}
	free( NuVarFrac );

  /* On resout */
  PNE_HeuristiqueResoudreLeProblemeRestreint( Pne, PosVarHeuristique, &NbVarBaseComplementairesHeuristique,
				      	      ComplementDeLaBaseHeuristique, PosVarEcartCoupesHeuristique,
					      VarDualesContraintesHeuristique, CoutsRedHeuristique,
					      &YaUneSolution, &Critere,
					      CoutMax, UtiliserCoutMax );					      
  CritereAuMax = 2 * LINFINI_PNE;
  if ( YaUneSolution == OUI_PNE ) {
    printf("Critere %e\n",Critere);
    CritereAuMax = Critere;
    memcpy( (char *) UminX , (char *) Pne->UminTrav      , Pne->NombreDeVariablesTrav * sizeof( double ) );
    memcpy( (char *) UmaxX , (char *) Pne->UmaxTrav      , Pne->NombreDeVariablesTrav * sizeof( double ) );
    memcpy( (char *) UX    , (char *) Pne->UTrav         , Pne->NombreDeVariablesTrav * sizeof( double ) );
    memcpy( (char *) PX    , (char *) PosVarHeuristique , Pne->NombreDeVariablesTrav * sizeof( int   ) );
    
    NbX = NbVarBaseComplementairesHeuristique;
    memcpy( (char *) CplX , (char *) ComplementDeLaBaseHeuristique , Pne->NombreDeContraintesTrav * sizeof( int ) );
    memcpy( (char *) PecX , (char *) PosVarEcartCoupesHeuristique  , Pne->Coupes.NombreDeContraintes  * sizeof( char ) );
        
  }  

  if ( CritereAuMax > LINFINI_PNE ) {
      /* Pas de solution ni au min ni au max */
      goto ArretHeuristique;   
  }
  memcpy( (char *) Pne->UminTrav      , (char *) UminX , Pne->NombreDeVariablesTrav * sizeof( double ) );
  memcpy( (char *) Pne->UmaxTrav      , (char *) UmaxX , Pne->NombreDeVariablesTrav * sizeof( double ) );
  memcpy( (char *) Pne->UTrav         , (char *) UX    , Pne->NombreDeVariablesTrav * sizeof( double ) );
  memcpy( (char *) PosVarHeuristique , (char *) PX    , Pne->NombreDeVariablesTrav * sizeof( int   ) );
  NbVarBaseComplementairesHeuristique = NbX;
  memcpy( (char *) ComplementDeLaBaseHeuristique , (char *) CplX , Pne->NombreDeContraintesTrav * sizeof( int ) );
  memcpy( (char *) PosVarEcartCoupesHeuristique  , (char *) PecX , Pne->Coupes.NombreDeContraintes  * sizeof( char ) );    
  /* Le simplexe a trouve une solution */    
  PNE_DeterminerLesVariablesFractionnaires( Pne, PosVarHeuristique );
  NombreDeMacroIterations++;
  goto TentativeDeFixationDeBorne;              
}
else if ( Pne->NombreDeVariablesAValeurFractionnaire == 0 ) {
  /* L'heuristique a trouve une solution entiere */
  Critere = Pne->Z0;
  for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) Critere+= Pne->LTrav[i] * Pne->UTrav[i];    
  printf("!!!!!!!!!!!!!!!!!!!! critere de l'heuristique %e  valeur courante optimale %e\n",Critere,Pne->CoutOpt);

  if ( fabs( Critere - Pne->Critere ) < 1.e-7 ) {
    printf("Solution entiere trouvee le noeud sera un noeud terminal \n");
    LeNoeudEstTerminal = OUI_PNE;
  }  
  if ( Critere < Pne->CoutOpt ) {
    printf("On archive la solution trouvee par l'heuristique\n");
    /* On archive quand meme la solution dans la partie PNE mais attention, les variables duales archivees ne sont pas bonnes.
       Cependant on les aura pour la solution optimale */      
    PNE_ArchiverLaSolutionCourante( Pne );
    /* Il faut mettre les valeurs des variables duales en accord avec la solution archivee */
    for ( i = 0 ; i < Pne->NombreDeContraintesTrav ; i++ ) {
      Pne->VariablesDualesDesContraintesTrav[i] = VarDualesContraintesHeuristique[i];
    }
   
    /* On met a jour la partie branch and bound */

    /* Attention ce n'est pas encore parfait du point de vue du branch and bound */
   
    Bb->CoutDeLaMeilleureSolutionEntiere        = Critere; /* Afin de pouvoir la trouver */
    Bb->NoeudDeLaMeilleureSolutionEntiere       = Bb->NoeudEnExamen; /* Ce qui n'est pas tout a fait exact mais cela n'est pas genant */
    Bb->NumeroDeProblemeDeLaSolutionAmeliorante = Bb->NombreDeProblemesResolus;

    Bb->UtiliserCoutDeLaMeilleureSolutionEntiere = OUI_SPX;
    Bb->NombreDeSolutionsEntieresTrouvees++;
    
  }

  goto ArretHeuristique;
 
}

/* Recuperation des donnees initiales et liberation memoire */
ArretHeuristique:

if ( LeNoeudEstTerminal == NON_PNE ) {
  Pne->VariableLaPlusFractionnaire = VariableAInstancier;
  Pne->CestTermine                 = NON_PNE;
  /* Si le noeud n'est pas terminal on recupere les valeurs initiales */
  memcpy( (char *) Pne->UTrav    , (char *) UTravSv    , Pne->NombreDeVariablesTrav * sizeof( double ) );
  memcpy( (char *) Pne->UminTrav , (char *) UminTravSv , Pne->NombreDeVariablesTrav * sizeof( double ) );
  memcpy( (char *) Pne->UmaxTrav , (char *) UmaxTravSv , Pne->NombreDeVariablesTrav * sizeof( double ) );
}
else {
  /* Si le noeud est terminal on garde les valeurs et on renseigne les couts reduits et les variables duales */ 
  Pne->VariableLaPlusFractionnaire = -1;
  Pne->CestTermine                 = OUI_PNE;
  memcpy( (char *) Pne->CoutsReduits , (char *) CoutsRedHeuristique , Pne->NombreDeVariablesTrav * sizeof( double ) );
  for ( i = 0 ; i < Pne->NombreDeContraintesTrav ; i++ ) {
    Pne->VariablesDualesDesContraintesTrav[i] = VarDualesContraintesHeuristique[i];
  }    
}

free( UTravSv );
free( UminTravSv );
free( UmaxTravSv );
free( PosVarHeuristique );
free( CoutsRedHeuristique );
free( ComplementDeLaBaseHeuristique );
free( VarDualesContraintesHeuristique );
free( PosVarEcartCoupesHeuristique );

free( UminX );
free( UmaxX );
free( UX );
free( PX );
free( UminM );
free( UmaxM );
free( UM );
free( PM );
free( CplX );
free( PecX );
free( CplM );
free( PecM );

return;
}













