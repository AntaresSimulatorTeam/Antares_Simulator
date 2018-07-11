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

   FONCTION: Pour le simplexe uniquement. Update de la factorisation LU 
             dans le cas d'une base adjacente.  
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"		     
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif
   
# define MARGE 2

# define TEST_DE_STABILITE  OUI_LU
# define SEUIL_DE_STABILITE 1.e+8

/*--------------------------------------------------------------------------------------------------*/
/* On recoit: 
- le numero de la colonne qui correspond a la variable sortante
- un pointeur sur la colonne A de la variable entrante 
- la colonne A
*/

void LU_UpdateLuSpikePret( MATRICE * Matrice, int ColonneNativeConcernee, int * SuccesUpdate )
{
int Kp  ; int i  ; int il   ; int ilMax; double X; int K2    ; int NbTermes ; int Colonne           ;
int il1 ; int ic ; int icMax; int ic1  ; int icU; int ilDiag; int ilU      ; int NbTermesLigneK2   ;
int j   ; int iad; double PlusGrandTermeDeLaLigne  ; int CapaciteDemandee      ; double * ValeurElmSpike;
int Kp0C; int K1 ; int KpColonneInitial           ; int KpLigneInitial        ; int KpInitial         ;
char * Marqueur    ; double * W; int * OrdreUcolonne; int * InverseOrdreUcolonne; double * ElmDeU        ; 
int * IndicesLignesDuSpike       ; int NbTermesNonNulsDuSpike       ; int * CdebParColonneDeU    ;
int * NbTermesParColonneDeU      ; int * IndiceLigneParColonneDeU   ; int * LdebParLigneDeU      ;
int * NbTermesParLigneDeU        ; int * StockageColonneVersLigneDeU; int * InverseOrdreColonne  ;
int * StockageLigneVersColonneDeU; int * IndiceColonneDeU           ; int * CapaciteParColonneDeU;
int * CapaciteParLigneDeU        ; int * OrdreColonne               ; double * ElmDeUParColonne   ;
char LigneModifiee; int NbTrm; double SeuilPiv; double LambdaMax;

Matrice->LuUpdateEnCours = OUI_LU;
 
/* Trop de termes crees => on refuse de continuer, il est preferable de refactoriser */
if ( Matrice->NbTermesCreesLuUpdate >= Matrice->MxTermesCreesLuUpdate ) { 
	# if VERBOSE_LU
	  printf("LU update refusee car trop de termes crees, LU update numero %d MxTermesCreesLuUpdate %d\n",
            Matrice->NombreDeLuUpdates,Matrice->MxTermesCreesLuUpdate);   
	# endif	
	*SuccesUpdate = NON_LU; 
  return;
}

if ( Matrice->NombreDeLuUpdates >= Matrice->NombreDeVecteursHAlloues ) {
	# if VERBOSE_LU
    printf("LU update refusee car cycle atteint \n");
	# endif		
  *SuccesUpdate = NON_LU; 
  return;
}

/* Si on a cree trop de termes dans le triangle U sans pour autant avoir atteint le
   nombre max d'Updates, on refactorise */
if ( Matrice->IndexLibreDeUParColonne >= Matrice->LimiteUpdatePourRefactorisation ) {
  # if VERBOSE_LU
    printf("LU update refusee car le triangle U a trop de termes \n");
    printf("   nombre de termes dans le triangle U %d  NombreDeLuUpdates %d\n",Matrice->IndexLibreDeUParColonne,Matrice->NombreDeLuUpdates);
	# endif	
  *SuccesUpdate = NON_LU; 
  return;
}

OrdreUcolonne = Matrice->OrdreUcolonne;
InverseOrdreUcolonne = Matrice->InverseOrdreUcolonne;

/* Ordre d'elimination apres factorisation: KpColonneNativeConcernee */
KpColonneInitial = Matrice->InverseOrdreColonne[ColonneNativeConcernee];

/* Sur ce KpColonneNativeConcernee il y a une matrice de permutation de U que l'on met a jour au moment des LU updates :
	 OrdreUcolonne -> permutation des colonnes OrdreUcolonne[i] = i en fin de factorisation
*/

K1 = InverseOrdreUcolonne[KpColonneInitial]; 
KpLigneInitial = OrdreUcolonne[K1];

/* KpColonneInitial KpLigneInitial vont permettre d'utiliser le chainage de U */

*SuccesUpdate = OUI_LU;

/* Il faut maintenant recalculer dans resolution, K2 comme etant le plus grand InverseOrdreUcolonne */
IndicesLignesDuSpike   = Matrice->IndicesLignesDuSpike;
ValeurElmSpike         = Matrice->ValeurElmSpike;
NbTermesNonNulsDuSpike = Matrice->NbTermesNonNulsDuSpike;

CdebParColonneDeU           = Matrice->CdebParColonneDeU;
NbTermesParColonneDeU       = Matrice->NbTermesParColonneDeU;
IndiceLigneParColonneDeU    = Matrice->IndiceLigneParColonneDeU;
ElmDeUParColonne            = Matrice->ElmDeUParColonne;
LdebParLigneDeU             = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU         = Matrice->NbTermesParLigneDeU;
StockageColonneVersLigneDeU = Matrice->StockageColonneVersLigneDeU;
StockageLigneVersColonneDeU = Matrice->StockageLigneVersColonneDeU;
ElmDeU                      = Matrice->ElmDeU;
IndiceColonneDeU            = Matrice->IndiceColonneDeU;
CapaciteParColonneDeU       = Matrice->CapaciteParColonneDeU;
CapaciteParLigneDeU         = Matrice->CapaciteParLigneDeU;
OrdreColonne                = Matrice->OrdreColonne;
InverseOrdreColonne         = Matrice->InverseOrdreColonne;
Marqueur                    = Matrice->Marqueur;
W                           = Matrice->W;

NbTrm = 0;
/* On supprime les termes de la colonne KpColonneNativeConcernee */
ic    = CdebParColonneDeU[KpColonneInitial];
icMax = ic + NbTermesParColonneDeU[KpColonneInitial];
NbTrm-= NbTermesParColonneDeU[KpColonneInitial]; /* Termes supprimes */
while ( ic < icMax ) {
  KpInitial  = IndiceLigneParColonneDeU[ic];

  il1 = LdebParLigneDeU[KpInitial] + NbTermesParLigneDeU[KpInitial] - 1;
  il  = StockageColonneVersLigneDeU[ic];

  /* Pour retasser il suffit de mettre le dernier terme a la place de celui la et de decrementer le nombre de termes */
  ElmDeU          [il] = ElmDeU[il1];
  IndiceColonneDeU[il] = IndiceColonneDeU[il1];
  /* */

  i = StockageLigneVersColonneDeU[il1];
  StockageLigneVersColonneDeU[il] = i;
  StockageColonneVersLigneDeU[i]  = il;

  /* */
  NbTermesParLigneDeU[KpInitial]--;
  /* */
  ic++;
}

/* A ce stade, la colonne "ColonneConcernee" est vide */  
/* On place le spike dans la colonne KpColonneNativeConcernee du triangle U */
/* On doit placer le spike dans la colonne et il faut qu'il y ait assez de place */
if ( CapaciteParColonneDeU[KpColonneInitial] < NbTermesNonNulsDuSpike ) {
  /* La place liberee ne suffit pas. On range la colonne a la fin et s'il n'y a pas assez de place on 
     fait une reallocation de U par colonne */
  CapaciteDemandee = NbTermesNonNulsDuSpike;
  if ( Matrice->IndexLibreDeUParColonne + CapaciteDemandee > Matrice->DernierIndexLibreDeUParColonne ) { 
    CapaciteDemandee = NbTermesNonNulsDuSpike + MARGE;
    LU_AugmenterLaTailleDuTriangleUParColonne( Matrice , CapaciteDemandee );
    IndiceLigneParColonneDeU    = Matrice->IndiceLigneParColonneDeU;
    ElmDeUParColonne            = Matrice->ElmDeUParColonne;
    StockageColonneVersLigneDeU = Matrice->StockageColonneVersLigneDeU;    
  }
  /* On change la valeur de l'indice debut mais on ne recupere pas la place pour ne pas perdre de temps 
    a retasser tout le triangle */
  CdebParColonneDeU    [KpColonneInitial] = Matrice->IndexLibreDeUParColonne;
  CapaciteParColonneDeU[KpColonneInitial] = CapaciteDemandee;
  Matrice->IndexLibreDeUParColonne+= CapaciteDemandee;
}
/* Nouvelle valeur du nombre de termes */
NbTermesParColonneDeU[KpColonneInitial] = NbTermesNonNulsDuSpike;
NbTrm+= NbTermesNonNulsDuSpike; /* Nouveaux termes */

/* On place le spike dans la colonne KpColonneNativeConcernee du triangle U */
K2 = -1;
ic = CdebParColonneDeU[KpColonneInitial];
for ( i = 0 ; i < NbTermesNonNulsDuSpike ; i++ ) {
  
  KpInitial = IndicesLignesDuSpike[i];
	if ( InverseOrdreUcolonne[KpInitial] > K2 ) K2 = InverseOrdreUcolonne[KpInitial];
	
	X = ValeurElmSpike[i];
		
  /* On verifie s'il y a assez de place dans le chainage par ligne */
  NbTermes = NbTermesParLigneDeU[KpInitial]; 
  if ( CapaciteParLigneDeU[KpInitial] <= NbTermes ) {	
    /* On recopie la ligne a la fin avant de creer le terme */
    CapaciteDemandee = NbTermes + 1;
    if ( Matrice->IndexLibreDeU + CapaciteDemandee > Matrice->DernierIndexLibreDeU ) { 
      CapaciteDemandee = NbTermes + MARGE;
      LU_AugmenterLaTailleDuTriangleU( Matrice , CapaciteDemandee );
      ElmDeU           = Matrice->ElmDeU;
      IndiceColonneDeU = Matrice->IndiceColonneDeU;
      StockageLigneVersColonneDeU = Matrice->StockageLigneVersColonneDeU;      
    }
    CapaciteParLigneDeU[KpInitial] = CapaciteDemandee;
    ilU = Matrice->IndexLibreDeU;
    il  = LdebParLigneDeU[KpInitial]; 
    ilMax = il + NbTermes;
    LdebParLigneDeU[KpInitial] = ilU;
    memcpy( &(ElmDeU[ilU])            , &(ElmDeU[il])            , NbTermes * sizeof( double ) );
    memcpy( &(IndiceColonneDeU[ilU])  , &(IndiceColonneDeU[il])  , NbTermes * sizeof( int ) );
    while ( il < ilMax ) {
      iad = StockageLigneVersColonneDeU[il];
      StockageLigneVersColonneDeU[ilU] = iad;
      StockageColonneVersLigneDeU[iad] = ilU;
      il++;
      ilU++;
    }      
    Matrice->IndexLibreDeU+= CapaciteDemandee; 
  }
  il = LdebParLigneDeU[KpInitial] + NbTermes;
  ElmDeU            [il] = X;
  IndiceColonneDeU  [il] = ColonneNativeConcernee;
  /* */
  StockageLigneVersColonneDeU[il] = ic;
  /* */
  NbTermesParLigneDeU[KpInitial]++;
  /*  */
  /* On met a jour le stockage par colonne */
  IndiceLigneParColonneDeU[ic] = KpInitial;
	ElmDeUParColonne        [ic] = X;
  /* */
  StockageColonneVersLigneDeU[ic] = il;
  /* */
  ic++;
}

/* On permute les colonnes */
Kp0C = OrdreUcolonne[K1];
for ( Kp = K1 ; Kp < K2 ; Kp++ ) {
  KpInitial = OrdreUcolonne[Kp + 1];
  OrdreUcolonne[Kp] = KpInitial;
	InverseOrdreUcolonne[KpInitial] = Kp;
}
OrdreUcolonne[K2] = Kp0C;
InverseOrdreUcolonne[Kp0C] = K2;

/*****************************************************************************************/

/* Elimination partielle: en sortie on a dans W la ligne qu'il faut mettre
   en ligne K2 */
LU_UpdateLuEliminations( Matrice, KpLigneInitial, K1, K2, &NbTermesLigneK2, SuccesUpdate, &LigneModifiee, &LambdaMax );

/* Mise a jour avec le nombre de termes qui resultent de l'echange des colonnes */
Matrice->NbTermesCreesLuUpdate += NbTrm;

if ( *SuccesUpdate == NON_LU ) return;

/*****************************************************************************************/

/* A ce stade la, la ligne KpLigneInitial est stockee dans W */

if ( LigneModifiee == NON_LU && 0 ) { /* Inhibe car il se passe un truc bizarre sur certains jdd et */
  ilDiag = -1;                        /* pour l'instant je sais pas d'ou ca vient Ca se manifeste surtout */
	                                    /* dans le branch and bound */ 	
  il    = LdebParLigneDeU[KpLigneInitial];
  ilMax = il + NbTermesParLigneDeU[KpLigneInitial];
  while ( il < ilMax ) { 
    Colonne = IndiceColonneDeU[il];   
    W       [Colonne] = 0.0;
    Marqueur[Colonne] = 0;   		
		if ( InverseOrdreColonne[Colonne] == KpLigneInitial ) {
		  ilDiag = il;
			il++;
			break;
		}
    il++;
  }
  while ( il < ilMax ) { 
    W       [IndiceColonneDeU[il]] = 0.0;
    Marqueur[IndiceColonneDeU[il]] = 0;   		
    il++;
  }			
  if ( ilDiag == -1 ) {
    # if VERBOSE_LU
      printf("LU_Update: Terme diagonal pas trouve\n");
	  # endif		
    *SuccesUpdate = NON_LU;
		return;
	}
	
	/*SeuilPiv = 1.e-8;*/ /*  <- 1.e-8 trop faible de plus le test ci-dessous doit etre NbTermesParColonneDeU[KpLigneInitial] == 1*/
	/*if ( NbTermesParLigneDeU[KpLigneInitial] == 1 && NbTermesParColonneDeU[KpLigneInitial] ) SeuilPiv = 1.e-9;*/
	
  SeuilPiv = Matrice->ValeurDuPivotMin;
	
  if ( fabs( ElmDeU[ilDiag] ) < SeuilPiv /*Matrice->ValeurDuPivotMin*/ ) {
    # if VERBOSE_LU
      printf("Manque de precision dans la LU update numero %d\n",Matrice->NombreDeLuUpdates);
	  # endif		
    *SuccesUpdate = NON_LU;
		return;
  }	
	/* Placer le terme diagonale de KpLigneInitial */
  il = LdebParLigneDeU[KpLigneInitial];	
	if ( ilDiag == il ) ElmDeU[il] = 1. / ElmDeU[il];
  else {
    Colonne = IndiceColonneDeU[il];
    IndiceColonneDeU[il]     = IndiceColonneDeU[ilDiag];
    IndiceColonneDeU[ilDiag] = Colonne;
    X = ElmDeU[il];  
    ElmDeU[il] = 1. / ElmDeU[ilDiag];
 	  ElmDeU[ilDiag] = X;	
    i = StockageLigneVersColonneDeU[ilDiag];
    j = StockageLigneVersColonneDeU[il];
    StockageLigneVersColonneDeU[il] = i;
    StockageColonneVersLigneDeU[i]  = il; 
    StockageLigneVersColonneDeU[ilDiag] = j;
    StockageColonneVersLigneDeU[j]      = ilDiag;
  }	
  ic     = CdebParColonneDeU[KpLigneInitial];
	ilDiag = StockageLigneVersColonneDeU[il];
	if ( ilDiag == ic ) ElmDeUParColonne[ic] = ElmDeU[il] /* 1. / ElmDeUParColonne[ic] */;
	else {	
	  X = ElmDeUParColonne[ic];
	  j = IndiceLigneParColonneDeU[ic];		
    ElmDeUParColonne        [ic] = ElmDeU[il] /* 1. / ElmDeUParColonne[ilDiag] */;
    IndiceLigneParColonneDeU[ic] = IndiceLigneParColonneDeU[ilDiag];		
    ElmDeUParColonne        [ilDiag] = X;
    IndiceLigneParColonneDeU[ilDiag] = j;		
    i = StockageColonneVersLigneDeU[ilDiag];
    j = StockageColonneVersLigneDeU[ic];		
    StockageColonneVersLigneDeU[ic] = i;  
    StockageLigneVersColonneDeU[i]  = ic;
    StockageColonneVersLigneDeU[ilDiag] = j;
    StockageLigneVersColonneDeU[j]      = ilDiag;
  }
  return;
}

/* Cas ou la ligne est modifiee */

/* On enleve les termes de la ligne KpLigneInitial du stockage par colonne */
il    = LdebParLigneDeU[KpLigneInitial];
ilMax = il + NbTermesParLigneDeU[KpLigneInitial];
while ( il < ilMax ) {

  KpInitial  = InverseOrdreColonne[IndiceColonneDeU[il]];

  ic1 = CdebParColonneDeU[KpInitial] + NbTermesParColonneDeU[KpInitial] - 1;
  ic  = StockageLigneVersColonneDeU[il];

  /* Pour retasser il suffit de mettre le dernier terme a la place de celui la et de decrementer le nombre de termes */
  IndiceLigneParColonneDeU[ic] = IndiceLigneParColonneDeU[ic1];
	ElmDeUParColonne        [ic] = ElmDeUParColonne[ic1];
  /*  */

  i = StockageColonneVersLigneDeU[ic1];
  StockageColonneVersLigneDeU[ic] = i;
  StockageLigneVersColonneDeU[i]  = ic;

  /*  */
  NbTermesParColonneDeU[KpInitial]--;
  /* */  
  il++;
}

NbTermesParLigneDeU[KpLigneInitial] = 0;

if ( CapaciteParLigneDeU[KpLigneInitial] < NbTermesLigneK2 ) {
  /* La place liberee ne suffit pas. On range la colonne a la fin et s'il n'y a pas assez de place on 
     fait une reallocation de U par colonne */
  CapaciteDemandee = NbTermesLigneK2;
  if ( Matrice->IndexLibreDeU + CapaciteDemandee > Matrice->DernierIndexLibreDeU ) { 
    CapaciteDemandee = NbTermesLigneK2 + MARGE;		
    LU_AugmenterLaTailleDuTriangleU( Matrice , CapaciteDemandee );
    ElmDeU           = Matrice->ElmDeU;
    IndiceColonneDeU = Matrice->IndiceColonneDeU;
    StockageLigneVersColonneDeU = Matrice->StockageLigneVersColonneDeU;          
  }
  /* On change la valeur de l'indice debut mais on ne recupere pas la place pour ne pas perdre de temps 
    a retasser tout le triangle */
  LdebParLigneDeU    [KpLigneInitial] = Matrice->IndexLibreDeU;
  CapaciteParLigneDeU[KpLigneInitial] = CapaciteDemandee; 
  Matrice->IndexLibreDeU+= CapaciteDemandee;
}

/* On recopie les termes non nuls de la ligne W dans la ligne KpLigneInitial du triangle U et on met a jour le stockage par colonne */
/* Attention a mettre le terme diagonal en debut de ligne KpLigneInitial */
/* On regarde egalement si la condition de stabilite est verifiee sur la ligne KpLigneInitial */
il = LdebParLigneDeU[KpLigneInitial];
PlusGrandTermeDeLaLigne = -1;

/* Index du terme diagonal */
ilDiag = -1;
KpInitial = OrdreUcolonne[K2]; 
if ( Marqueur[OrdreColonne[KpInitial]] == 1 ) ilDiag = il;

for ( Kp = K2 ; Kp < Matrice->Rang ; Kp++ ) { 
  KpInitial = OrdreUcolonne[Kp];
	
	Colonne = OrdreColonne[KpInitial];
	
  if ( Marqueur[Colonne] == 1 ) {			
    X = W[Colonne];			
    if ( fabs( X ) > PlusGrandTermeDeLaLigne ) PlusGrandTermeDeLaLigne = fabs( X );
		
    ElmDeU          [il] = X;
    IndiceColonneDeU[il] = Colonne;
    NbTermesParLigneDeU[KpLigneInitial]++;
		
    /* On verifie s'il y a assez de place dans le chainage par colonne */
    NbTermes = NbTermesParColonneDeU[KpInitial]; 
    if ( CapaciteParColonneDeU[KpInitial] <= NbTermes ) {
      /* On recopie la ligne a la fin avant de creer le terme */
      CapaciteDemandee = NbTermes + 1;
      if ( Matrice->IndexLibreDeUParColonne + CapaciteDemandee > Matrice->DernierIndexLibreDeUParColonne ) { 
        CapaciteDemandee = NbTermes + MARGE;				
        LU_AugmenterLaTailleDuTriangleUParColonne( Matrice , CapaciteDemandee );
        IndiceLigneParColonneDeU    = Matrice->IndiceLigneParColonneDeU;
        ElmDeUParColonne            = Matrice->ElmDeUParColonne;
        StockageColonneVersLigneDeU = Matrice->StockageColonneVersLigneDeU;    	
      }
      CapaciteParColonneDeU[KpInitial] = CapaciteDemandee;
      icU = Matrice->IndexLibreDeUParColonne;
      ic  = CdebParColonneDeU[KpInitial];  
      icMax = ic + NbTermes;
      CdebParColonneDeU[KpInitial] = icU;
      memcpy( &(IndiceLigneParColonneDeU[icU]), &(IndiceLigneParColonneDeU[ic]), NbTermes * sizeof( int   ) );
      memcpy( &(ElmDeUParColonne[icU])        , &(ElmDeUParColonne[ic])        , NbTermes * sizeof( double ) );
			
      while ( ic < icMax ) {
        iad = StockageColonneVersLigneDeU[ic];
        StockageColonneVersLigneDeU[icU] = iad;
        StockageLigneVersColonneDeU[iad] = icU;
        ic++;
        icU++;
      } 
      
      Matrice->IndexLibreDeUParColonne+= CapaciteDemandee; 
    }
    ic = CdebParColonneDeU[KpInitial] + NbTermes;  
    IndiceLigneParColonneDeU[ic] = KpLigneInitial;
    ElmDeUParColonne[ic]         = X;		
    NbTermesParColonneDeU[KpInitial]++;		
    /* */
    StockageLigneVersColonneDeU[il] = ic;
    StockageColonneVersLigneDeU[ic] = il;   
    /* */		
    W       [Colonne] = 0.0;	
    Marqueur[Colonne] = 0;
    /* */
    il++;
  }
}
  
if ( ilDiag < 0 ) {
  # if VERBOSE_LU
    printf("LU_UpdateLu: pas de terme diagonal pour la colonne K2\n");
	# endif			
  *SuccesUpdate = NON_LU; 
  return;  
}

il = LdebParLigneDeU[KpLigneInitial];
if ( ilDiag != il ) {
  /* Normalement on ne passe jamais la dedans */
  Colonne = IndiceColonneDeU[il];
  IndiceColonneDeU[il]     = IndiceColonneDeU[ilDiag];
  IndiceColonneDeU[ilDiag] = Colonne;
  X = ElmDeU[il];  
  ElmDeU[il]     = ElmDeU[ilDiag];		
  ElmDeU[ilDiag] = X;	

  i = StockageLigneVersColonneDeU[ilDiag];
  j = StockageLigneVersColonneDeU[il];

  StockageLigneVersColonneDeU[il] = i;  
  StockageColonneVersLigneDeU[i]  = il; 

  StockageLigneVersColonneDeU[ilDiag] = j; 
  StockageColonneVersLigneDeU[j]      = ilDiag; 
}
/* */

/*SeuilPiv = 1.e-8;*/ /*  <- 1.e-8 trop faible de plus le test ci-dessous doit etre NbTermesParColonneDeU[KpLigneInitial] == 1*/
/*if ( NbTermesParLigneDeU[KpLigneInitial] == 1 && NbTermesParColonneDeU[KpLigneInitial] ) SeuilPiv = 1.e-9;*/
	
SeuilPiv = Matrice->ValeurDuPivotMin;
 
if ( fabs( ElmDeU[il] ) < SeuilPiv /*Matrice->ValeurDuPivotMin*/ ) {
  # if VERBOSE_LU
    printf("Manque de precision dans la LU update numero %d\n",Matrice->NombreDeLuUpdates);
    printf("  SeuilPiv %e PlusGrandTermeDeLaLigne %e Valeur Diagonale %e 0.01 * PlusGrandTermeDeLaLigne %e\n",
              SeuilPiv,PlusGrandTermeDeLaLigne,fabs( Matrice->ElmDeU[il] ),
							0.01*PlusGrandTermeDeLaLigne);							
	# endif	
  *SuccesUpdate = NON_LU;  
	return;
}

/* Test de stabilite */
# if TEST_DE_STABILITE == OUI_LU
  /* ElmDeU[il] est le pivot */	
  if ( LambdaMax * PlusGrandTermeDeLaLigne / fabs( ElmDeU[il] ) > SEUIL_DE_STABILITE ) {
    # if VERBOSE_LU
      printf("Manque de precision dans la LU update numero %d:\n",Matrice->NombreDeLuUpdates);
      printf("LambdaMax %e PlusGrandTermeDeLaLigne %e Valeur Diagonale %e LambdaMax * PlusGrandTermeDeLaLigne / Valeur Diagonale ) = %e > %e\n",
              LambdaMax,PlusGrandTermeDeLaLigne,fabs( Matrice->ElmDeU[il] ),LambdaMax * PlusGrandTermeDeLaLigne / fabs( ElmDeU[il] ),SEUIL_DE_STABILITE);
	  # endif		
    *SuccesUpdate = NON_LU;  
	  return;
	}
# endif

ElmDeU[il] = 1. / ElmDeU[il];

/* */
/* Pour le chainage par colonne, le terme diagonal vient d'etre place a la fin. On le place au debut */
ic     = CdebParColonneDeU[KpLigneInitial];  
ilDiag = ic + NbTermesParColonneDeU[KpLigneInitial] - 1;

if ( ic == ilDiag ) ElmDeUParColonne [ic] = 1. / ElmDeUParColonne[ic];
else {
  X = ElmDeUParColonne[ic];
  j = IndiceLigneParColonneDeU[ic];  

  ElmDeUParColonne        [ic] = 1. / ElmDeUParColonne[ilDiag];
  IndiceLigneParColonneDeU[ic] = IndiceLigneParColonneDeU[ilDiag];

  ElmDeUParColonne        [ilDiag] = X;
  IndiceLigneParColonneDeU[ilDiag] = j;

  i = StockageColonneVersLigneDeU[ilDiag];
  j = StockageColonneVersLigneDeU[ic]; 

  StockageColonneVersLigneDeU[ic] = i;  
  StockageLigneVersColonneDeU[i]  = ic;

  StockageColonneVersLigneDeU[ilDiag] = j;
  StockageLigneVersColonneDeU[j]      = ilDiag;
}

if ( LigneModifiee == OUI_LU ) Matrice->NombreDeLuUpdates++;

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* K1 est la ligne (dans la numerotation Kp) dont il faut annuler les termes   */
/* K2 est la ligne (dans la numerotation Kp) du dernier terme non nul du spike */
void LU_UpdateLuEliminations( MATRICE * Matrice, int KpLigneInitial, int K1, int K2,
                              int * NombreDeTermes, int * SuccesUpdate, char * LigneModifiee, double * LbdMax )
{
int il; int ilMax; int Colonne; int Kp; double Lambda;  int NbTermes; int * NbTermesParLigneDeU; int * LdebParLigneDeU;
int * IndiceColonneDeU; char * Marqueur; int * HDeb; int * HLigne; int * HNbTerm; int * HIndiceColonne; double * ElmDeU;
double * W; double * HValeur; int * OrdreUcolonne; int KpInitial; int IndexHLibre; int NombreDElementsHAlloues;
int NombreDeLuUpdates; int NbTermesCreesLuUpdate; char Flag; double LambdaMax;

/* */

*LbdMax = 1;
LambdaMax = -1;     

OrdreUcolonne = Matrice->OrdreUcolonne;

NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
LdebParLigneDeU     = Matrice->LdebParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;

W        = Matrice->W;
Marqueur = Matrice->Marqueur;

NombreDeLuUpdates     = Matrice->NombreDeLuUpdates;
NbTermesCreesLuUpdate = Matrice->NbTermesCreesLuUpdate;

HDeb    = Matrice->HDeb;
HLigne  = Matrice->HLigne;
HNbTerm = Matrice->HNbTerm;
HValeur = Matrice->HValeur;
HIndiceColonne = Matrice->HIndiceColonne;
IndexHLibre    = Matrice->IndexHLibre;
NombreDElementsHAlloues = Matrice->NombreDElementsHAlloues;

NbTermes = NbTermesParLigneDeU[KpLigneInitial];
il       = LdebParLigneDeU[KpLigneInitial];
ilMax    = il + NbTermes;
while ( il < ilMax ) { 
  Colonne = IndiceColonneDeU[il];   
  /*  */  
  W       [Colonne] = ElmDeU[il];
  Marqueur[Colonne] = 1;		
  /*  */
  il++;
}

/* */
HDeb   [NombreDeLuUpdates] = IndexHLibre;
HLigne [NombreDeLuUpdates] = KpLigneInitial;  
HNbTerm[NombreDeLuUpdates] = 0;
/* */

/* On annule la ligne */

Flag = NON_LU;

for ( Kp = K1 ; Kp < K2 ; Kp++ ) {
  /* Le premier terme de la ligne KpInitial est le terme "diagonal" */
	KpInitial = OrdreUcolonne[Kp];	
  il      = LdebParLigneDeU[KpInitial];
  Colonne = IndiceColonneDeU[il];
  /* S'il n'y a pas de terme dans la ligne K1, il n'y a rien a faire */
  if ( Marqueur[Colonne] == 0 ) continue;
	
	Flag = OUI_LU;	
  ilMax = il + NbTermesParLigneDeU[KpInitial];
  /* */
  Lambda = -W[Colonne] * ElmDeU[il]; /* Car le terme diagonal est deja inverse */

  # if TEST_DE_STABILITE == OUI_LU
	  if ( fabs( Lambda ) > LambdaMax ) LambdaMax = fabs( Lambda );
  # endif
	
  W       [Colonne] = 0.;
  Marqueur[Colonne] = 0;
  NbTermes--;
	
	NbTermesCreesLuUpdate--; /* Car on supprime un terme dans la triangualire superieure */

	/*if ( Lambda == 0.0 ) continue;*/
	
	if ( fabs( Lambda ) < VALEUR_NULLE_DE_LAMBDA_POUR_LU_UPDATE ) continue;	
	
  /* */							    
  il++;
  /* */
  HValeur       [IndexHLibre] = -Lambda;
  HIndiceColonne[IndexHLibre] = KpInitial;           
  HNbTerm[NombreDeLuUpdates]++;
  IndexHLibre++;
  if ( IndexHLibre >= NombreDElementsHAlloues ) { 
    /*printf("pas assez delements H alloues \n");*/
    Matrice->IndexHLibre = IndexHLibre;
    Matrice->NbTermesCreesLuUpdate = NbTermesCreesLuUpdate; 		
    *SuccesUpdate  = NON_LU;
		*LigneModifiee = Flag;
    return; 
  }
  /* */
  while ( il < ilMax ) { 
    Colonne = IndiceColonneDeU[il];    		
    W[Colonne] = W[Colonne] + ( Lambda * ElmDeU[il] );		
    if ( Marqueur[Colonne] != 1 ) { 
      NbTermesCreesLuUpdate++; 
      NbTermes++; 
    } 
    Marqueur[Colonne] = 1;
    il++;
  }
}
*LigneModifiee  = Flag;
*NombreDeTermes = NbTermes; 
Matrice->IndexHLibre = IndexHLibre;
Matrice->NbTermesCreesLuUpdate = NbTermesCreesLuUpdate;

if ( LambdaMax > 0 ) *LbdMax = LambdaMax;
  
return;
}
















































