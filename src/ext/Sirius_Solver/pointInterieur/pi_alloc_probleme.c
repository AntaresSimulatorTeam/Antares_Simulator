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


/***********************************************************************************

   FONCTION: Allocations memoire pour le probleme d'optimisation
             Liberation memoire a la fin du probleme
                   
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# include "lu_define.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif

/*------------------------------------------------------------------------*/

void PI_AllocProbleme( PROBLEME_PI * Pi,
                int   NombreDeVariables_E, 
                int   NombreDeContraintes_E, 
		int * Nbter_E,
		char * Sens_E,
		char * VariableBinaire_E,
		int * TypeVar_E
		      )
{
int i; int NombreDeVariables; int NombreIneg; int NombredeTermesCnt;
int NombreDeContraintes      ; int NbVarBin  ;

Pi->MatriceFactorisee = NULL;

NombreDeVariables   = NombreDeVariables_E;
NombreDeContraintes = NombreDeContraintes_E;

NombreIneg        = 0;
NombredeTermesCnt = 0;
for ( i = 0 ; i < NombreDeContraintes_E ; i++ ) {
  if ( Sens_E[i] != '=' ) NombreIneg++;
  NombredeTermesCnt+= Nbter_E[i];
}

NombredeTermesCnt+= NombreIneg;
NombreDeVariables+= NombreIneg;

NbVarBin = 0;
if ( MPCC == OUI_PI ) {
  for ( i = 0 ; i < NombreDeVariables_E ; i++ ) {
    if ( VariableBinaire_E[i] == OUI_PI && TypeVar_E[i] != VARIABLE_FIXE ) {
      NbVarBin++;
      if ( NbVarBin == 1000000 ) {
        i++;
        for ( ; i < NombreDeVariables_E ; i++ ) VariableBinaire_E[i] = NON_PI;
        break;
      }
    }    
  }
  if ( NbVarBin > 0 ) {
    NombredeTermesCnt+= 3* NbVarBin;
    NombredeTermesCnt+= 1; /* Marge par rapport a la derniere contrainte (si la derniere contrainte est < ) */
    NombreDeVariables++; /* Car on va creer une variable Xi pour la norme l-infini */
    NombreDeContraintes+= NbVarBin; /* Une contrainte de complementarite par variable binaire */
    NombreDeVariables+= NbVarBin; /* Car  on va creer des variables d'ecart */
  }
  printf("Nombre de variables binaires dans le point interieur: %d\n",NbVarBin);
}

/* Pour les tests de splitting */
/*
NombreDeVariables*= 2;
NombreDeContraintes*= 2; 
NombredeTermesCnt*= 2;
*/
/* Fin pour les tests de splitting */
#if VERBOSE_PI
  printf("Nombre de termes de la matrice des contraintes %d\n",NombredeTermesCnt);
#endif
 
Pi->CorrespondanceVarEntreeVarPi = (int *) malloc( NombreDeVariables   * sizeof( int ) );
Pi->CorrespondanceCntEntreeCntPi = (int *) malloc( NombreDeContraintes * sizeof( int ) );

Pi->Q      = (double *) malloc( NombreDeVariables * sizeof( double ) );
Pi->L      = (double *) malloc( NombreDeVariables * sizeof( double ) );
Pi->Qpar2  = (double *) malloc( NombreDeVariables * sizeof( double ) );

Pi->NbTermesAllouesPourA = NombredeTermesCnt;
Pi->A      = (double *) malloc( NombredeTermesCnt   * sizeof( double ) );
Pi->B      = (double *) malloc( NombreDeContraintes * sizeof( double ) );
Pi->Mdeb   = (int *)   malloc( NombreDeContraintes * sizeof( int   ) );
Pi->NbTerm = (int *)   malloc( NombreDeContraintes * sizeof( int   ) );
Pi->Indcol = (int *)   malloc( NombredeTermesCnt   * sizeof( int   ) );

Pi->Cdeb    = (int *)   malloc( NombreDeVariables * sizeof( int   ) );
Pi->CNbTerm = (int *)   malloc( NombreDeVariables * sizeof( int   ) );
Pi->ACol               = (double *) malloc( NombredeTermesCnt * sizeof( double ) );
Pi->NumeroDeContrainte = (int *)   malloc( NombredeTermesCnt * sizeof( int   ) );
Pi->Csui               = (int *)   malloc( NombredeTermesCnt * sizeof( int   ) );

Pi->U               = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->U0              = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->Umax            = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->Umin            = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->Alpha           = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->Alpha2          = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->TypeDeVariable  = (char *)  malloc( NombreDeVariables * sizeof( char ) );
Pi->VariableBinaire = (char *)  malloc( NombreDeVariables * sizeof( char ) );
Pi->NumeroDeLaContrainteDeComplementarite = (int *) malloc( NombreDeVariables * sizeof( int ) );
Pi->S1      = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->S2      = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->Lambda  = (double *) malloc( NombreDeContraintes * sizeof( double ) );
Pi->Lambda0 = (double *) malloc( NombreDeContraintes * sizeof( double ) );

Pi->ScaleU      = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->ScaleB      = (double *) malloc( NombreDeContraintes * sizeof( double ) );

Pi->DeltaU           = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->DeltaLambda      = (double *) malloc( NombreDeContraintes * sizeof( double ) );
Pi->DeltaS1          = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->DeltaS2          = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->UkMoinsUmin      = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->UmaxMoinsUk      = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->UnSurUkMoinsUmin = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->UnSurUmaxMoinsUk = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->DeltaUDeltaS1    = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->DeltaUDeltaS2    = (double *) malloc( NombreDeVariables   * sizeof( double ) );

Pi->SecondMembre       = (double *) malloc( ( NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );
Pi->SecondMembreAffine = (double *) malloc( ( NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );
 
Pi->MukIteration = (double *) malloc( NOMBRE_MAX_DITERATION * sizeof( double ) );

Pi->MaxOptimalite    = (double *) malloc( NOMBRE_MAX_DITERATION * sizeof( double ) );
Pi->MaxContrainte    = (double *) malloc( NOMBRE_MAX_DITERATION * sizeof( double ) );
Pi->MaxStationnarite = (double *) malloc( NOMBRE_MAX_DITERATION * sizeof( double ) );
Pi->IterationTetakP  = (double *) malloc( NOMBRE_MAX_DITERATION * sizeof( double ) );
Pi->IterationTetakD  = (double *) malloc( NOMBRE_MAX_DITERATION * sizeof( double ) );

Pi->RegulVar                   = (double *) malloc( NombreDeVariables   * sizeof( double ) );
Pi->RegulContrainte            = (double *) malloc( NombreDeContraintes * sizeof( double ) );

Pi->Matrice = (MATRICE_A_FACTORISER *) malloc( sizeof( MATRICE_A_FACTORISER ) );

/* Test de validite */
if ( 
     Pi->CorrespondanceVarEntreeVarPi == NULL ||
     Pi->CorrespondanceCntEntreeCntPi == NULL ||
     Pi->Q     	                      == NULL ||
     Pi->L     	                      == NULL ||
     Pi->Qpar2 	                      == NULL ||
     Pi->A      	                     == NULL ||
     Pi->B      	                     == NULL ||
     Pi->Mdeb   	                     == NULL ||
     Pi->NbTerm 	                     == NULL ||
     Pi->Indcol 	                     == NULL ||
     Pi->Cdeb                         == NULL ||
     Pi->CNbTerm                      == NULL ||
     Pi->ACol                         == NULL ||
     Pi->NumeroDeContrainte           == NULL ||
     Pi->Csui                         == NULL ||
     Pi->U      	                     == NULL ||
     Pi->U0     	                     == NULL ||
     Pi->Umax   	                     == NULL ||
     Pi->Umin   	                     == NULL ||
     Pi->Alpha   	                     == NULL ||
     Pi->Alpha2  	                     == NULL ||
     Pi->TypeDeVariable               == NULL ||
     Pi->VariableBinaire              == NULL ||
     Pi->NumeroDeLaContrainteDeComplementarite == NULL ||
     Pi->S1     	                     == NULL ||
     Pi->S2     	                     == NULL ||
     Pi->Lambda 	                     == NULL ||
     Pi->Lambda0	                     == NULL ||
     Pi->ScaleU                       == NULL ||
     Pi->ScaleB                       == NULL ||
     Pi->DeltaU                       == NULL ||  
     Pi->DeltaLambda                  == NULL ||  
     Pi->DeltaS1                      == NULL || 
     Pi->DeltaS2                      == NULL ||   
     Pi->UkMoinsUmin                  == NULL ||   
     Pi->UmaxMoinsUk                  == NULL || 
     Pi->UnSurUkMoinsUmin             == NULL ||
     Pi->UnSurUmaxMoinsUk             == NULL ||
     Pi->DeltaUDeltaS1                == NULL || 
     Pi->DeltaUDeltaS2                == NULL || 
     Pi->SecondMembre                 == NULL ||     
     Pi->SecondMembreAffine           == NULL || 
     Pi->MukIteration                 == NULL ||
     Pi->MaxOptimalite    	          == NULL ||
     Pi->MaxContrainte   	            == NULL ||
     Pi->MaxStationnarite 	          == NULL ||
     Pi->IterationTetakP              == NULL ||
     Pi->IterationTetakD  	          == NULL ||
     Pi->RegulVar                     == NULL ||
     Pi->RegulContrainte              == NULL ||
     Pi->Matrice                      == NULL
   ) {

  printf(" Point interieur, memoire insuffisante. Sous-programme: PI_AllocProbleme\n");
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

Pi->MatricePi = NULL;

return;
}

/*------------------------------------------------------------------------*/

void PI_LibereProbleme( PROBLEME_PI * Pi )
{

free( Pi->CorrespondanceVarEntreeVarPi );
free( Pi->CorrespondanceCntEntreeCntPi );
free( Pi->Q );
free( Pi->L );
free( Pi->Qpar2 );
free( Pi->A );
free( Pi->B );
free( Pi->Mdeb );
free( Pi->NbTerm );
free( Pi->Indcol );
free( Pi->Cdeb );
free( Pi->CNbTerm );
free( Pi->ACol );
free( Pi->NumeroDeContrainte );
free( Pi->Csui );
free( Pi->U );
free( Pi->U0 );
free( Pi->Umax );
free( Pi->Umin );
free( Pi->Alpha );
free( Pi->Alpha2 );
free( Pi->TypeDeVariable );
free( Pi->VariableBinaire );
free( Pi->NumeroDeLaContrainteDeComplementarite );
free( Pi->S1 );
free( Pi->S2 );
free( Pi->Lambda );
free( Pi->Lambda0 );
free( Pi->ScaleU );
free( Pi->ScaleB );
free( Pi->DeltaU );
free( Pi->DeltaLambda );
free( Pi->DeltaS1 );
free( Pi->DeltaS2 );
free( Pi->UkMoinsUmin  );
free( Pi->UmaxMoinsUk );
free( Pi->UnSurUkMoinsUmin );
free( Pi->UnSurUmaxMoinsUk );
free( Pi->DeltaUDeltaS1 );
free( Pi->DeltaUDeltaS2 );
free( Pi->SecondMembre );
free( Pi->SecondMembreAffine );
free( Pi->MukIteration );
free( Pi->MaxOptimalite );
free( Pi->MaxContrainte );
free( Pi->MaxStationnarite );
free( Pi->IterationTetakP );
free( Pi->IterationTetakD );
free( Pi->RegulVar );
free( Pi->RegulContrainte );

free( Pi->Matrice );

return;
}







