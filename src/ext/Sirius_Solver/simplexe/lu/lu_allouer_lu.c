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

   FONCTION: Allouer / desallouer les tableaux pour la factoristion LU
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

# define COEFF_MULT 1.5

/*----------------------------------------------------------------------------*/
/*            On initialise et on alloue les zones de travail                 */

void LU_AllouerLU( MATRICE * Matrice,
                   double * ValeurDesTermesDeLaMatrice, 
                   int   * IndicesDeLigne ,  
                   int   * Ideb,  
                   int   * NbTermesDesColonnes, 
                   int   * NbColonnes 
                 )

{  
int i      ; int il     ; int ilk     ; int NbTrm; int NbCol; 
int NbTrm_L; int NbTrm_U; int NbTrmAux; int Dispo;

/* Decompte du nombre de termes */	        
NbCol = *NbColonnes;
for ( NbTrm = 0 , i = 0 ; i < NbCol ; i++ ) NbTrm+= NbTermesDesColonnes[i];

#if VERBOSE_LU
  printf("LU_AllouerLU: Nombre de termes non nuls de la matrice %d\n",NbTrm);
#endif

/********* Pour sortir la matrice si debogage necessaire **********/
/*
{
FILE * Flot; int ilMax;

Flot = fopen( "Matrice.mtx", "w" ); 
if( Flot == NULL ) {
  printf("Erreur ouverture du fichier pour l'ecriture de la matrice \n");
  exit(0);
}

fprintf(Flot,"%d %d %d\n",NbCol,NbCol,NbTrm);

for ( i = 0 ; i < NbCol ; i++ ) {
  il    = Ideb[i];
  ilMax = il + NbTermesDesColonnes[i];
  while ( il < ilMax ) {
    fprintf(Flot,"%d %d %e\n",IndicesDeLigne[il]+1,i+1,ValeurDesTermesDeLaMatrice[il]);
    il++;
  }
}
fclose(Flot);
}
*/
/******************************************************************/
Matrice->NombreDeTermesDeLaMatriceAFactoriser     = NbTrm;
Matrice->NombreDeTermes                           = NbTrm;

NbTrm_L = NbTrm + INCREMENT_DALLOCATION_DE_L;
NbTrm_U = NbTrm + INCREMENT_DALLOCATION_DE_U;    
/* Calcul de la marge disponible pour la creation des termes */
/* On ajoute du disponible par rapport à la matrice d'entree */
Dispo = NbTrm;                 
Matrice->MargePourCreationDeTermesLignes = (int) floor( (double)Dispo / NbCol ) + 1;
if ( Matrice->MargePourCreationDeTermesLignes < MARGE_MINIMALE_POUR_CREATION_DE_TERMES ) {
  Matrice->MargePourCreationDeTermesLignes = MARGE_MINIMALE_POUR_CREATION_DE_TERMES;
}
Matrice->MargePourCreationDeTermesColonnes = Matrice->MargePourCreationDeTermesLignes << 1;

/* Calcul du total a allouer */

/* Prevoir de doubler au lieu d'ajouter les increments */

Matrice->TailleAlloueeLignes   = NbTrm + ( NbCol * Matrice->MargePourCreationDeTermesLignes ) 
                                 + L_INCREMENT_DALLOCATION; 
Matrice->TailleAlloueeColonnes = NbTrm + ( NbCol * Matrice->MargePourCreationDeTermesColonnes )
                                 + C_INCREMENT_DALLOCATION; 

Matrice->TailleAlloueeLignes   = (int) ( Matrice->TailleAlloueeLignes * COEFF_MULT );
Matrice->TailleAlloueeColonnes = (int) ( Matrice->TailleAlloueeColonnes * COEFF_MULT );
															 
Matrice->LDernierIndexLibrePossible = Matrice->TailleAlloueeLignes   - 1;
Matrice->CDernierIndexLibrePossible = Matrice->TailleAlloueeColonnes - 1;
/*
printf(" Matrice.TailleAllouee %d MargePourCreationDeTermes %d\n",Matrice->TailleAlloueeLignes,Matrice->MargePourCreationDeTermesLignes);
*/

Matrice->LIncrementDallocation = L_INCREMENT_DALLOCATION;
Matrice->CIncrementDallocation = C_INCREMENT_DALLOCATION;

Matrice->IncrementDallocationDeL = INCREMENT_DALLOCATION_DE_L;
Matrice->IncrementDallocationDeU = INCREMENT_DALLOCATION_DE_U;

Matrice->LignePrecedente  = (int *)    malloc( NbCol * sizeof( int ) );
Matrice->LigneSuivante    = (int *)    malloc( NbCol * sizeof( int ) );
Matrice->Ldeb             = (int *)    malloc( NbCol * sizeof( int ) );
Matrice->LNbTerm          = (int *)    malloc( NbCol * sizeof( int ) );
Matrice->LDernierPossible = (int *)    malloc( NbCol * sizeof( int ) );
Matrice->LIndiceColonne   = (int *)    malloc( Matrice->TailleAlloueeLignes * sizeof( int   ) );
Matrice->Elm              = (double *) malloc( Matrice->TailleAlloueeLignes * sizeof( double) );

Matrice->ColonnePrecedente    = (int *) malloc( NbCol * sizeof( int ) );
Matrice->ColonneSuivante      = (int *) malloc( NbCol * sizeof( int ) );
Matrice->Cdeb                 = (int *) malloc( NbCol * sizeof( int ) );
Matrice->CNbTerm              = (int *) malloc( NbCol * sizeof( int ) );
Matrice->CNbTermMatriceActive = (int *) malloc( NbCol * sizeof( int ) );
Matrice->CDernierPossible     = (int *) malloc( NbCol * sizeof( int ) );
Matrice->CIndiceLigne         = (int *) malloc( Matrice->TailleAlloueeColonnes * sizeof( int   ) );
   
if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) {
  Matrice->SuperLigneDeLaLigne       = (SUPER_LIGNE_DE_LA_MATRICE **) malloc( NbCol * sizeof( void * ) );
  Matrice->SuperLigneAScanner        = (SUPER_LIGNE_DE_LA_MATRICE **) malloc( NbCol * sizeof( void * ) );
  Matrice->SuperLigne                = (SUPER_LIGNE_DE_LA_MATRICE **) malloc( NbCol * sizeof( void * ) );
	Matrice->PoidsDesColonnes          = (int *)                        malloc( NbCol * sizeof( int    ) );

  Matrice->HashModuloSize = NbCol;
  Matrice->HashCodeLigne          = (unsigned int *) malloc( NbCol * sizeof( unsigned int ) );
  Matrice->HashCodeSuperLigne     = (unsigned int *) malloc( NbCol * sizeof( unsigned int ) );
  Matrice->HashModuloPrem         = (int *)          malloc( NbCol * sizeof( int          ) );
  Matrice->HashNbModuloIdentiques = (int *)          malloc( NbCol * sizeof( int          ) );
  i = 2 * NbCol;
  Matrice->TypeDeClassementHashCodeAFaire = (char *) malloc( i * sizeof( char ) );  
  Matrice->HashModuloSuiv                 = (int *)  malloc( i * sizeof( int ) );
  Matrice->HashModuloPrec                 = (int *)  malloc( i * sizeof( int ) );
}  

Matrice->LigneRejeteeTemporairementPourPivotage   = (char *) malloc( NbCol * sizeof( char ) );
Matrice->ColonneRejeteeTemporairementPourPivotage = (char *) malloc( NbCol * sizeof( char ) );

Matrice->AbsDuPlusGrandTermeDeLaLigne = (double *) malloc( NbCol * sizeof( double ) );

Matrice->PremLigne = (int *) malloc( ( NbCol + 1 ) * sizeof( int ) );
Matrice->SuivLigne = (int *) malloc(   NbCol       * sizeof( int ) );
Matrice->PrecLigne = (int *) malloc(   NbCol       * sizeof( int ) );

Matrice->PremColonne = (int *) malloc( ( NbCol + 1 ) * sizeof( int ) );
Matrice->SuivColonne = (int *) malloc(   NbCol       * sizeof( int ) );
Matrice->PrecColonne = (int *) malloc(   NbCol       * sizeof( int ) );

Matrice->W        = (double *) malloc( NbCol * sizeof( double ) );
Matrice->Marqueur = (char *)   malloc( NbCol * sizeof( char   ) );

Matrice->OrdreLigne          = (int *)   malloc( NbCol * sizeof( int ) );
Matrice->InverseOrdreLigne   = (int *)   malloc( NbCol * sizeof( int ) );
Matrice->OrdreColonne        = (int *)   malloc( NbCol * sizeof( int ) );
Matrice->InverseOrdreColonne = (int *)   malloc( NbCol * sizeof( int ) );

Matrice->NbTermesParColonneDeL = (int *)    malloc( NbCol   * sizeof( int    ) );
Matrice->CdebParColonneDeL     = (int *)    malloc( NbCol   * sizeof( int    ) );
Matrice->ElmDeL                = (double *) malloc( NbTrm_L * sizeof( double ) );
Matrice->IndiceLigneDeL        = (int *)    malloc( NbTrm_L * sizeof( int    ) );

Matrice->SolutionIntermediaire  = (double *) malloc( NbCol * sizeof( double ) );
 
if (
  Matrice->LignePrecedente   == NULL ||     
  Matrice->LigneSuivante     == NULL ||     
  Matrice->Ldeb              == NULL ||     
  Matrice->LNbTerm           == NULL ||     
  Matrice->LDernierPossible  == NULL ||     
  Matrice->LIndiceColonne    == NULL ||     
  Matrice->Elm               == NULL ||
	
  Matrice->ColonnePrecedente == NULL ||     
  Matrice->ColonneSuivante   == NULL ||     
  Matrice->Cdeb              == NULL ||     
  Matrice->CNbTerm           == NULL ||     
  Matrice->CNbTermMatriceActive == NULL ||     
  Matrice->CDernierPossible  == NULL ||     
  Matrice->CIndiceLigne      == NULL ||     

  ( Matrice->UtiliserLesSuperLignes == OUI_LU &&  
    ( Matrice->SuperLigneDeLaLigne        == NULL ||   
      Matrice->SuperLigneAScanner         == NULL ||  
      Matrice->SuperLigne                 == NULL ||
      Matrice->PoidsDesColonnes           == NULL ||

      Matrice->HashCodeLigne          == NULL ||
      Matrice->HashCodeSuperLigne     == NULL ||
      Matrice->HashModuloPrem         == NULL ||
      Matrice->HashNbModuloIdentiques == NULL ||

      Matrice->TypeDeClassementHashCodeAFaire == NULL ||    
      Matrice->HashModuloSuiv                 == NULL ||  
      Matrice->HashModuloPrec                 == NULL 
    )
  ) ||
  
  Matrice->LigneRejeteeTemporairementPourPivotage   == NULL ||
  Matrice->ColonneRejeteeTemporairementPourPivotage == NULL ||

  Matrice->AbsDuPlusGrandTermeDeLaLigne == NULL ||    

  Matrice->PremLigne == NULL || 
  Matrice->SuivLigne == NULL || 
  Matrice->PrecLigne == NULL || 

  Matrice->PremColonne == NULL || 
  Matrice->SuivColonne == NULL || 
  Matrice->PrecColonne == NULL || 

  Matrice->W        == NULL ||
  Matrice->Marqueur == NULL || 

  Matrice->OrdreLigne          == NULL ||
  Matrice->InverseOrdreLigne   == NULL ||
  Matrice->OrdreColonne        == NULL || 
  Matrice->InverseOrdreColonne == NULL || 

  Matrice->NbTermesParColonneDeL == NULL ||
  Matrice->CdebParColonneDeL     == NULL ||
  Matrice->ElmDeL                == NULL ||
  Matrice->IndiceLigneDeL        == NULL ||  
  
  Matrice->SolutionIntermediaire == NULL 
  
  )
  {

  printf("Factorisation LU, sous-programme LU_AllouerLU: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);

  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
 
}

if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) {
  il = (int) floor( 0.5 * RAND_MAX );
  for ( i = 0 ; i < NbCol ; i++ ) {
    Matrice->SuperLigneDeLaLigne[i]= NULL;
    if ( rand() < il ) Matrice->PoidsDesColonnes[i] = i;
    else Matrice->PoidsDesColonnes[i] = i << 1;        
  }    
  Matrice->NombreDeSuperLignes = 0;  
}

if ( Matrice->FaireScaling == OUI_LU ) {
  Matrice->ScaleX = (double *) malloc( NbCol * sizeof( double   ) );
  Matrice->ScaleB = (double *) malloc( NbCol * sizeof( double   ) );
  if ( Matrice->ScaleX == NULL || Matrice->ScaleB == NULL ) {
    printf("Factorisation LU, sous-programme LU_AllouerLU: \n");
    printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
    fflush(stdout);
    Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
    longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
  }
}

# ifdef HYPER_CREUX
  Matrice->ListeDesNoeuds   = NULL;
  Matrice->NoeudDansLaliste = NULL;
  Matrice->PseudoPile       = NULL;
# endif

if ( Matrice->FaireDuPivotageDiagonal == OUI_LU) {
  Matrice->AbsValeurDuTermeDiagonal = (double *) malloc( NbCol * sizeof( double ) );
  if ( Matrice->AbsValeurDuTermeDiagonal == NULL ) {
    printf("Factorisation LU, sous-programme LU_AllouerLU: \n");
    printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
    fflush(stdout);
    Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
    longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
  }    
}

/* Recherche du plus grand index adressable dans la matrice d'entree */
NbTrmAux = -1;
for ( i = 0 ; i < NbCol ; i++ ) { 
  ilk = Ideb[i] + NbTermesDesColonnes[i] - 1;
  if ( ilk > NbTrmAux ) NbTrmAux = ilk;
}
NbTrmAux++;
					     
LU_ConstruireProbleme( Matrice, ValeurDesTermesDeLaMatrice, IndicesDeLigne, Ideb, NbTermesDesColonnes, 
                       NbCol, NbTrm_L, NbTrm_U );

Matrice->NombreDeLuUpdates = 0; /* Car utilise dans la resolution */

Matrice->SauvegardeDuResultatIntermediaire = NON_LU; /* Car utilise dans la resolution */

if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) {

  Matrice->NombreDeVecteursHAlloues = NOMBRE_INITIAL_DE_VECTEURS_H_ALLOUES; 
  i = Matrice->NombreDeVecteursHAlloues * sizeof( int );
  Matrice->HDeb    = (int *) malloc( i ); 
  Matrice->HLigne  = (int *) malloc( i ); 
  Matrice->HNbTerm = (int *) malloc( i ); 

  /* En moyenne 10% de termes par colonne par rapport au nombre de lignes de la matrice */
  /*i = (int) (0.1 * (float) NbCol);*/
  /* Il ne faut pas etre trop restrictif sinon on refactorise trop souvent ce qui nuit a l'efficacite de la LU update */
  i = (int) (0.4 * (float) NbCol);
  if ( i < 100 ) i = 100;

  Matrice->NombreDElementsHAlloues = Matrice->NombreDeVecteursHAlloues * i;

  Matrice->HIndiceColonne = (int *)   malloc( Matrice->NombreDElementsHAlloues * sizeof( int ) ); 
  Matrice->HValeur        = (double *) malloc( Matrice->NombreDElementsHAlloues * sizeof( double ) ); 

  Matrice->IndicesLignesDuSpike = (int *)   malloc( Matrice->Rang * sizeof( int   ) );
  Matrice->ValeurElmSpike       = (double *) malloc( Matrice->Rang * sizeof( double ) );

  /* Matrice->NombreDeLuUpdates = 0; */
  Matrice->IndexHLibre     = 0;

  Matrice->NbTermesCreesLuUpdate = 0;

}

return;
}

/*----------------------------------------------------------------------------*/
/*         On augmente la taille d'une ligne de la matrice active             */

void LU_AugmenterLaTailleDeLaMatriceActive( MATRICE * Matrice , int Ligne , int TailleDemandee )
{
int NbElements; int MargePourCreationDeTermesLignes; int il; int ilMax; int Nb; int ix; 
int ilNew; int NbElemNew; int L; int L1; int L2; int CapaciteRestante;
int * Ldeb; 	int * LNbTerm;int * LIndiceColonne; double * Elm; int * LDernierPossible; 	
int * LIndiceColonneNew; double * ElmNew; int *	LignePrecedente; int *	LigneSuivante;
int Kp; int * InverseOrdreLigne;

/*printf("LU_AugmenterLaTailleDeLaMatriceActive Ligne %d  Kp %d\n",Ligne,Matrice->Kp);*/

MargePourCreationDeTermesLignes = Matrice->MargePourCreationDeTermesLignes;

/* S'il y a assez de place a la fin, on l'utilise */
NbElements = TailleDemandee + MargePourCreationDeTermesLignes;
if ( Matrice->LIndexLibre + NbElements - 1 <= Matrice->LDernierIndexLibrePossible ) {
  DeplacerLigne:
  il = Matrice->Ldeb[Ligne];
	Nb = Matrice->LNbTerm[Ligne];
	ix = Matrice->LDernierPossible[Ligne];
	memcpy( (char *) &(Matrice->LIndiceColonne[Matrice->LIndexLibre]), (char *) &(Matrice->LIndiceColonne[il]), Nb * sizeof( int    ) );
	memcpy( (char *) &(Matrice->Elm[Matrice->LIndexLibre])           , (char *) &(Matrice->Elm[il])           , Nb * sizeof( double ) );
  Matrice->Ldeb            [Ligne] = Matrice->LIndexLibre;
	Matrice->LDernierPossible[Ligne] = Matrice->LIndexLibre + NbElements - 1;	
	Matrice->LIndexLibre += NbElements;
	L1 = Matrice->LignePrecedente[Ligne];
	L2 = Matrice->LigneSuivante[Ligne];
	if ( L1 >= 0 ) {
	  Matrice->LDernierPossible[L1] = ix;
		if ( L2 >= 0 ) {
	    Matrice->LigneSuivante   [L1] = L2;
	    Matrice->LignePrecedente [L2] = L1;
		}
	}
  else {
	  if ( L2 >= 0 ) Matrice->LignePrecedente[L2] = -1;		
	}
	if ( Ligne != Matrice->DerniereLigne ) {
	  Matrice->LignePrecedente[Ligne] = Matrice->DerniereLigne;
	  Matrice->LigneSuivante[Matrice->DerniereLigne] = Ligne;
	  Matrice->LigneSuivante[Ligne] = -1;
	  Matrice->DerniereLigne = Ligne;
  }
	return;
}
else {
  /* Il faut redimensionner tout le triangle */
  # if VERBOSE_LU
	  printf(" Redimensionnement lignes necessaire Kp %d Rang %d\n",Matrice->Kp,Matrice->Rang);
  # endif
	/* On reconstitue les marges et on ajoute un increment */
  NbElemNew = 0;
	Kp = Matrice->Kp;
	InverseOrdreLigne = Matrice->InverseOrdreLigne;
  LNbTerm           =  Matrice->LNbTerm;
  NbElemNew = 0;
  for ( L = 0 ; L < Matrice->Rang ; L++ ) {
		NbElemNew += LNbTerm[L];		
		if ( InverseOrdreLigne[L] > Kp ) NbElemNew += MargePourCreationDeTermesLignes;
  }			
  NbElemNew += NbElements + Matrice->LIncrementDallocation;
  if ( Matrice->LIncrementDallocation < L_INCREMENT_DALLOCATION << SHIFT_MAX_INCREMENT ) {
    Matrice->LIncrementDallocation *= 2;
  }
	NbElemNew = (int) ( NbElemNew * COEFF_MULT );
  LIndiceColonneNew = (int *)    malloc( NbElemNew * sizeof( int    ) );
  ElmNew            = (double *) malloc( NbElemNew * sizeof( double ) );
  if ( LIndiceColonneNew == NULL || ElmNew == NULL ) {          
    printf("Factorisation LU, sous-programme LU_AugmenterLaTailleDeLaMatriceActive: \n");
    printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
    Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
    longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
  }
  /* On recopie terme a terme pour recuperer aussi la place qu'on perdue en supprimant une ligne
	   qui se trouvait etre la premiere dans le stockage, et les extensions exagerees */	
	Matrice->LDernierIndexLibrePossible = NbElemNew - 1;
	
  ilNew = 0;
	LignePrecedente  =  Matrice->LignePrecedente;
	LigneSuivante    =  Matrice->LigneSuivante;
	Ldeb             =  Matrice->Ldeb;
  LIndiceColonne   =  Matrice->LIndiceColonne;
  Elm              =  Matrice->Elm;
	LDernierPossible =  Matrice->LDernierPossible;
  for ( L = 0 ; L < Matrice->Rang ; L++ ) {
	  il = Ldeb[L];
		Nb = LNbTerm[L];
		ilMax   = il + Nb;
		
		if ( InverseOrdreLigne[L] > Kp ) {
		  /*
			CapaciteRestante = LDernierPossible[L] - ilMax + 1;		
	    if ( CapaciteRestante > MargePourCreationDeTermesLignes ) CapaciteRestante = MargePourCreationDeTermesLignes;
			else if ( CapaciteRestante < 0 ) CapaciteRestante = MargePourCreationDeTermesLignes;
			*/
			CapaciteRestante = MargePourCreationDeTermesLignes;
			if ( L == Ligne ) CapaciteRestante += TailleDemandee;
		}
		else CapaciteRestante = 0;
		
    Ldeb[L] = ilNew;
		while ( il < ilMax ) {       
		  LIndiceColonneNew[ilNew] = LIndiceColonne[il];
		  ElmNew           [ilNew] = Elm[il];
      ilNew++;
      il++;
		}
	  LignePrecedente[L] = L - 1;
  	LigneSuivante  [L] = L + 1;		
		LDernierPossible[L] = CapaciteRestante + ilNew - 1;		
		ilNew = LDernierPossible[L] + 1;		
	}
	Matrice->LIndexLibre = ilNew;
	
  LignePrecedente[0] = -1;
  LigneSuivante  [Matrice->Rang - 1] = -1;
	Matrice->DerniereLigne = Matrice->Rang - 1;
	
	free( Matrice->LIndiceColonne );
	free( Matrice->Elm );
	Matrice->LIndiceColonne = LIndiceColonneNew;
  Matrice->Elm            = ElmNew;
	
	goto DeplacerLigne;

}
 
return;
}  

/*----------------------------------------------------------------------------*/

void LU_AugmenterLaTailleDeLaMatriceActiveParColonne( MATRICE * Matrice , int Colonne , int TailleDemandee )
{
int NbElements; int ic; int Nb; int ix; int icNew; int icMax; int C1; int C2; 
int C; int NbElemNew; int CapaciteRestante;
int * Cdeb; int * CNbTerm; int * CIndiceLigne; int * CDernierPossible;
int * CIndiceLigneNew; int MargePourCreationDeTermesColonnes;
int * ColonnePrecedente; int * ColonneSuivante; int Kp; int * InverseOrdreColonne;
int * CNbTermMatriceActive; 

/*printf("LU_AugmenterLaTailleDeLaMatriceActiveParColonne Colonne %d Kp %d\n",Colonne,Matrice->Kp);*/

MargePourCreationDeTermesColonnes = Matrice->MargePourCreationDeTermesColonnes;

/* S'il y a assez de place a la fin, on l'utilise */
NbElements = TailleDemandee + MargePourCreationDeTermesColonnes;
if ( Matrice->CIndexLibre + NbElements - 1 <= Matrice->CDernierIndexLibrePossible ) {
  /* Compacter la colonne "Colonne" */	
  if ( Matrice->CNbTerm[Colonne] != Matrice->CNbTermMatriceActive[Colonne] ) {
    LU_SupprimerTermesInutilesDansColonne( Matrice , Colonne , Matrice->Cdeb[Colonne] );
  }
  DeplacerColonne:
  ic = Matrice->Cdeb[Colonne];
	Nb = Matrice->CNbTerm[Colonne];
	ix = Matrice->CDernierPossible[Colonne];
	memcpy( (char *) &(Matrice->CIndiceLigne[Matrice->CIndexLibre]), (char *) &(Matrice->CIndiceLigne[ic]), Nb * sizeof( int ) );
  Matrice->Cdeb            [Colonne] = Matrice->CIndexLibre;
	Matrice->CDernierPossible[Colonne] = Matrice->CIndexLibre + NbElements - 1;
	Matrice->CIndexLibre += NbElements;
	C1 = Matrice->ColonnePrecedente[Colonne];
	C2 = Matrice->ColonneSuivante[Colonne];
	if ( C1 >= 0 ) {
	  Matrice->CDernierPossible[C1] = ix;
		if ( C2 >= 0 ) {
	    Matrice->ColonneSuivante  [C1] = C2;
		  Matrice->ColonnePrecedente[C2] = C1;		  
		}
	}  
  else {
	  if ( C2 >= 0 ) Matrice->ColonnePrecedente[C2] = -1;		
	}
	if ( Colonne != Matrice->DerniereColonne ) {
	  Matrice->ColonnePrecedente[Colonne] = Matrice->DerniereColonne;
	  Matrice->ColonneSuivante[Matrice->DerniereColonne] = Colonne;
	  Matrice->ColonneSuivante[Colonne] = -1;
	  Matrice->DerniereColonne = Colonne;
  }
	return;
}
else {
  # if VERBOSE_LU
  printf(" Redimensionnement colonnes necessaire Kp %d Rang %d\n",Matrice->Kp,Matrice->Rang);
  # endif
	/* On reconstitue les marges et on ajoute un increment */
  NbElemNew = 0;
	Kp = Matrice->Kp;
	InverseOrdreColonne  = Matrice->InverseOrdreColonne;
  CNbTerm              = Matrice->CNbTerm;
  CNbTermMatriceActive = Matrice->CNbTermMatriceActive;
	Cdeb                 = Matrice->Cdeb;
  NbElemNew = 0;
  for ( C = 0 ; C < Matrice->Rang ; C++ ) {
    /* Compacter toutes les colonnes */
    if ( CNbTerm[C] != CNbTermMatriceActive[C] ) {
      LU_SupprimerTermesInutilesDansColonne( Matrice , C , Cdeb[C] );
    }	
		NbElemNew += CNbTerm[C];		
		if ( InverseOrdreColonne[C] > Kp ) NbElemNew += MargePourCreationDeTermesColonnes;
  }			
  NbElemNew += NbElements + Matrice->CIncrementDallocation;   		
  if ( Matrice->CIncrementDallocation < C_INCREMENT_DALLOCATION << SHIFT_MAX_INCREMENT ) {
    Matrice->CIncrementDallocation *= 2;
  }
	NbElemNew = (int) ( NbElemNew * COEFF_MULT );
  CIndiceLigneNew = (int *) malloc( NbElemNew * sizeof( int ) );
  if ( CIndiceLigneNew == NULL ) {          
    printf("Factorisation LU, sous-programme LU_AugmenterLaTailleDeLaMatriceActiveParColonne: \n");
    printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
    Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
    longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
  }
  /* On recopie terme a terme pour recuperer aussi la place qu'on a perdu en supprimant une ligne
	   qui se trouvait etre la premiere dans le stockage, et les extensions exagerees */	
	Matrice->CDernierIndexLibrePossible = NbElemNew - 1;
	
	icNew = 0;
	ColonnePrecedente =  Matrice->ColonnePrecedente;
  ColonneSuivante   =  Matrice->ColonneSuivante;	
  CIndiceLigne      =  Matrice->CIndiceLigne;
	CDernierPossible  =  Matrice->CDernierPossible;
  for ( C = 0 ; C < Matrice->Rang ; C++ ) {
	  ic = Cdeb[C];
		Nb = CNbTerm[C];	
		icMax   = ic + Nb;
		
		if ( InverseOrdreColonne[C] > Kp ) {
			CapaciteRestante = MargePourCreationDeTermesColonnes;
			if ( C == Colonne ) CapaciteRestante += TailleDemandee;
		}
		else CapaciteRestante = 0;
			
    Cdeb[C] = icNew;
		while ( ic < icMax ) {       
		  CIndiceLigneNew[icNew] = CIndiceLigne[ic];
      icNew++;
      ic++;
		}
	  ColonnePrecedente[C] = C - 1;
  	ColonneSuivante  [C] = C + 1;		
		CDernierPossible[C] = CapaciteRestante + icNew - 1;		
		icNew = CDernierPossible[C] + 1;			
	}
	Matrice->CIndexLibre = icNew;

  ColonnePrecedente[0] = -1;
  ColonneSuivante  [Matrice->Rang - 1] = -1;
	Matrice->DerniereColonne = Matrice->Rang - 1;	
 
	free( CIndiceLigne ); 
	Matrice->CIndiceLigne = CIndiceLigneNew;
	
	goto DeplacerColonne;
	
}

return;
}

/*----------------------------------------------------------------------------*/

void LU_AugmenterLaTailleDuTriangleL( MATRICE * Matrice , int TailleDemandee )
{
int NbElemNew; int NbElemAv;

/*printf("LU_AugmenterLaTailleDuTriangleL  \n");*/

NbElemAv                     = Matrice->DernierIndexLibreDeL + 1;
NbElemNew                    = NbElemAv + TailleDemandee + Matrice->IncrementDallocationDeL;

if ( Matrice->IncrementDallocationDeL < INCREMENT_DALLOCATION_DE_L << SHIFT_MAX_INCREMENT ) {
  Matrice->IncrementDallocationDeL*= 2;
}

/*Matrice.IndexLibreDeL        = NbElemAv;*/ 
Matrice->DernierIndexLibreDeL = NbElemNew - 1;
/*
printf("-- Nouvelle valeur du nombre d elements alloues %d  \n",NbElemNew); 
*/
Matrice->ElmDeL         = (double *) realloc( Matrice->ElmDeL         , NbElemNew * sizeof( double ) );
Matrice->IndiceLigneDeL = (int *)   realloc( Matrice->IndiceLigneDeL , NbElemNew * sizeof( int   ) );

if ( Matrice->ElmDeL == NULL || Matrice->IndiceLigneDeL == NULL ) {
  printf(" LU_AugmenterLaTailleDuTriangleL \n"); 
  printf(" Etape de factorisation, memoire insuffisante. Nombre de termes atteint: %d \n", NbElemAv); 

  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 

}

return;
}

/*----------------------------------------------------------------------------*/

void LU_AugmenterLaTailleDuTriangleU( MATRICE * Matrice , int TailleDemandee )
{
int NbElemNew; int NbElemAv; 
 
/*printf("LU_AugmenterLaTailleDuTriangleU\n");*/

NbElemAv                     = Matrice->DernierIndexLibreDeU + 1;
NbElemNew                    = NbElemAv + TailleDemandee + Matrice->IncrementDallocationDeU;

if ( Matrice->IncrementDallocationDeU < INCREMENT_DALLOCATION_DE_U << SHIFT_MAX_INCREMENT ) {
  Matrice->IncrementDallocationDeU*= 2;
}

/*Matrice.IndexLibreDeU        = NbElemAv;*/ 
Matrice->DernierIndexLibreDeU = NbElemNew - 1;
/*
printf("-- Nouvelle valeur du nombre d elements alloues %d  \n",NbElemNew); 
*/
Matrice->ElmDeU             = (double *) realloc( Matrice->ElmDeU           , NbElemNew * sizeof( double ) );
Matrice->IndiceColonneDeU   = (int *)   realloc( Matrice->IndiceColonneDeU , NbElemNew * sizeof( int   ) );

if ( Matrice->LuUpdateEnCours == OUI_LU ) {
	Matrice->StockageLigneVersColonneDeU = (int *) realloc( Matrice->StockageLigneVersColonneDeU , NbElemNew * sizeof( int ) );
}

if ( Matrice->ElmDeU == NULL || Matrice->IndiceColonneDeU  == NULL || 
    (Matrice->LuUpdateEnCours == OUI_LU &&  Matrice->StockageLigneVersColonneDeU == NULL) ) {
  printf(" LU_AugmenterLaTailleDuTriangleU \n"); 
  printf(" Etape de factorisation, memoire insuffisante. Nombre de termes atteint: %d \n", NbElemAv); 
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}
if ( Matrice->LuUpdateEnCours == OUI_LU ) {
  if ( Matrice->StockageLigneVersColonneDeU == NULL ) {
	  printf(" LU_AugmenterLaTailleDuTriangleU \n"); 
    printf(" Etape de factorisation, memoire insuffisante. Nombre de termes atteint: %d \n", NbElemAv); 
    Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
    longjmp( Matrice->Env, Matrice->AnomalieDetectee );
	}
}

return;
}

/*----------------------------------------------------------------------------*/

void LU_AugmenterLaTailleDuTriangleUParColonne( MATRICE * Matrice , int TailleDemandee )
{
int NbElemNew; int NbElemAv;

/*printf("LU_AugmenterLaTailleDuTriangleUParColonne  \n");*/

NbElemAv  = Matrice->DernierIndexLibreDeUParColonne + 1;
NbElemNew = NbElemAv + TailleDemandee + Matrice->IncrementDallocationDeU;

if ( Matrice->IncrementDallocationDeU < INCREMENT_DALLOCATION_DE_U << SHIFT_MAX_INCREMENT ) {
  Matrice->IncrementDallocationDeU*= 2;
}

/*Matrice.IndexLibreDeU        = NbElemAv;*/ 
Matrice->DernierIndexLibreDeUParColonne = NbElemNew - 1;
/*
printf("-- Nouvelle valeur du nombre d elements alloues %d  \n",NbElemNew); 
*/
Matrice->IndiceLigneParColonneDeU = (int *) realloc( Matrice->IndiceLigneParColonneDeU , NbElemNew * sizeof( int   ) );
if ( Matrice->IndiceLigneParColonneDeU  == NULL ) {
  printf("LU_AugmenterLaTailleDuTriangleUParColonne  \n"); 
  printf("Etape de factorisation, memoire insuffisante. Nombre de termes atteint: %d \n", NbElemAv); 
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}

if ( Matrice->LuUpdateEnCours == OUI_LU ) {
  Matrice->StockageColonneVersLigneDeU = (int *)   realloc( Matrice->StockageColonneVersLigneDeU, NbElemNew * sizeof( int   ) );
  Matrice->ElmDeUParColonne            = (double *) realloc( Matrice->ElmDeUParColonne           , NbElemNew * sizeof( double ) );
  if ( Matrice->StockageColonneVersLigneDeU == NULL || Matrice->ElmDeUParColonne == NULL ) {
    printf("LU_AugmenterLaTailleDuTriangleUParColonne  \n"); 
    printf("Etape de factorisation, memoire insuffisante. Nombre de termes atteint: %d \n", NbElemAv); 
    Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
    longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
  }
}

return;
}

/*----------------------------------------------------------------------------*/

void LU_LibererLesTableauxTemporaires( MATRICE * Matrice )
{

free( Matrice->LigneRejeteeTemporairementPourPivotage );

free( Matrice->ColonneRejeteeTemporairementPourPivotage );

/* On reutilise la place */
Matrice->SecondMembreSV = Matrice->AbsDuPlusGrandTermeDeLaLigne;

free( Matrice->PremLigne );
free( Matrice->SuivLigne );
free( Matrice->PrecLigne );

free( Matrice->PremColonne );
free( Matrice->SuivColonne );
free( Matrice->PrecColonne );

if ( Matrice->ContexteDeLaFactorisation != LU_SIMPLEXE ) {
  free( Matrice->W );
  free( Matrice->Marqueur );
}

if ( Matrice->FaireDuPivotageDiagonal == OUI_LU) {
  free( Matrice->AbsValeurDuTermeDiagonal );
}

if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) {
  free( Matrice->SuperLigneDeLaLigne );
  free( Matrice->SuperLigneAScanner );
  free( Matrice->SuperLigne );
  free( Matrice->PoidsDesColonnes );  

  free( Matrice->HashCodeLigne );
  free( Matrice->HashCodeSuperLigne );
  free( Matrice->HashModuloPrem );
  free( Matrice->HashNbModuloIdentiques );
  free( Matrice->TypeDeClassementHashCodeAFaire );
  free( Matrice->HashModuloSuiv );
  free( Matrice->HashModuloPrec );
}

/* Conserve pour la LU Update et la refactorisation
free( Matrice->InverseOrdreLigne );
free( Matrice->InverseOrdreColonne );
*/

/* Conserve pour la resolution hyper creux */
/*free( Matrice->LDernierPossible );*/

free( Matrice->Cdeb );
free( Matrice->CNbTerm );
free( Matrice->CNbTermMatriceActive );
free( Matrice->CDernierPossible );
free( Matrice->CIndiceLigne );

return;
}

/*----------------------------------------------------------------------------*/
/* Methode systematique: apres avoir libere une zone memoire on met NULL. Car
   selon le contexte, 2 tableaux peuvent partager une meme zone (simple changement
	 de nom )*/
void LU_LibererMemoireLU( MATRICE * Matrice )
{

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  MEM_Quit( Matrice->Tas );
	return;
# endif

free( Matrice->OrdreLigne );

free( Matrice->OrdreColonne );

free( Matrice->NbTermesParColonneDeL );
free( Matrice->CdebParColonneDeL );
free( Matrice->ElmDeL );
free( Matrice->IndiceLigneDeL );

free( Matrice->CapaciteParLigneDeU );

free( Matrice->ElmDeUParColonne );
free( Matrice->IndiceLigneParColonneDeU );
free( Matrice->CapaciteParColonneDeU );

free( Matrice->StockageColonneVersLigneDeU );
free( Matrice->StockageLigneVersColonneDeU );

free( Matrice->SolutionIntermediaire );

free( Matrice->AbsDuPlusGrandTermeDeLaLigne );
free( Matrice->SolutionSV );  

free( Matrice->NumeroDeTriangleDeLaVariable );

free( Matrice->InverseOrdreLigne );
free( Matrice->InverseOrdreColonne );

/* Car: Matrice->FaireScaling peut avoir ete mis a NON_LU dans CalculerLeScaling */
if ( Matrice->ScaleX != NON_LU ) free( Matrice->ScaleX );
if ( Matrice->ScaleB != NON_LU ) free( Matrice->ScaleB );

if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) {
  free( Matrice->HDeb );
  free( Matrice->HLigne );
  free( Matrice->HNbTerm );
  free( Matrice->HIndiceColonne );
  free( Matrice->HValeur );

  free( Matrice->W );
  free( Matrice->Marqueur );

  free( Matrice->IndicesLignesDuSpike );
  free( Matrice->ValeurElmSpike );

	free( Matrice->OrdreUcolonne );
	free( Matrice->InverseOrdreUcolonne );

	free( Matrice->LdebParLigneDeL );
  free( Matrice->NbTermesParLigneDeL );
  free( Matrice->IndiceColonneParLigneDeL );
	free( Matrice->ElmDeLParLigne );	
}

if ( Matrice->ContexteDeLaFactorisation == LU_POINT_INTERIEUR ) {
  free( Matrice->DebutInfosAdressesQueKpModifie );
  free( Matrice->AdresseDeUModifie );
  free( Matrice->AdresseUHaut );
  free( Matrice->DebutInfosLignesQueKpModifie );
  free( Matrice->NombreDeLignesQueKpModifie );
  free( Matrice->AdresseUGauche );
  free( Matrice->NombreDeTermesParLigneQueKpModifie );
  free( Matrice->IndexKpDeUouL );
}

# ifdef HYPER_CREUX
  /* free( Matrice->PseudoPile );     <- car on reutilise Matrice->Ldeb */
  /* free( Matrice->ListeDesNoeuds ); <- car on reutilise Matrice->LNbTerm; */
  /* free( Matrice->NoeudDansLaliste ); <- car on reutilise Matrice->LDernierPossible; */
# endif

free( Matrice->LignePrecedente );
free( Matrice->LigneSuivante );

free( Matrice->Ldeb );
free( Matrice->LNbTerm );
free( Matrice->Elm );
free( Matrice->LIndiceColonne );
free( Matrice->LDernierPossible );

if ( Matrice->ContexteDeLaFactorisation != LU_GENERAL ) {
  free( Matrice->ElmDeU );
  free( Matrice->IndiceColonneDeU );
}

free( Matrice->ColonnePrecedente );
free( Matrice->ColonneSuivante );

free( Matrice );

return;
}











