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

   FONCTION: Reorganisation des chainages en fin de factorisation.
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/
/*                           Chainage par ligne du triangle U                                       */
/*              Le triangle U se trouve stocke par ligne dans la matrice active                     */
/*              On se contente d'un chainage dans l'ordre d'elimination                             */
/*--------------------------------------------------------------------------------------------------*/
void LU_ChainageParLigneDeULUGeneral( MATRICE * Matrice )
{
int Ligne;int Kp; int * LdebParLigneDeU; int * NbTermesParLigneDeU; int * IndiceColonneDeU; double * ElmDeU;
int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm; int * OrdreLigne;

Matrice->DernierIndexLibreDeU = Matrice->IndexLibreDeU;

Matrice->NbTermesParLigneDeU = Matrice->LignePrecedente;
Matrice->LdebParLigneDeU     = Matrice->LigneSuivante;
	
Matrice->ElmDeU              = Matrice->Elm;
Matrice->IndiceColonneDeU    = Matrice->LIndiceColonne;

LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
ElmDeU              = Matrice->ElmDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
Elm            = Matrice->Elm;
LIndiceColonne = Matrice->LIndiceColonne; 

OrdreLigne = Matrice->OrdreLigne;

for ( Kp = 0 ; Kp < Matrice->Rang ; Kp++ ) {
  Ligne = OrdreLigne[Kp];
	LdebParLigneDeU    [Kp] = Ldeb[Ligne];
	NbTermesParLigneDeU[Kp] = LNbTerm[Ligne];
}	 

return;
}
/*--------------------------------------------------------------------------------------------------*/
/*                           Chainage par ligne du triangle U                                     */
/*              Le triangle U se trouve stocke par ligne dans la matrice active                     */
/*--------------------------------------------------------------------------------------------------*/

void LU_ChainageParLigneDeU( MATRICE * Matrice )
{
int NbTrm_U; int Ligne; int il; int Nb; int ilNew; int Kp;
int * LdebParLigneDeU; int * NbTermesParLigneDeU; int * IndiceColonneDeU; double * ElmDeU;
int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;
int * OrdreLigne;

NbTrm_U = Matrice->IndexLibreDeU + 10;
Matrice->DernierIndexLibreDeU = NbTrm_U;

Matrice->NbTermesParLigneDeU = Matrice->LignePrecedente;
Matrice->LdebParLigneDeU     = Matrice->LigneSuivante;
    
Matrice->ElmDeU            = (double *) malloc( NbTrm_U * sizeof( double ) );
Matrice->IndiceColonneDeU  = (int *)    malloc( NbTrm_U * sizeof( int    ) );

if ( Matrice->ElmDeU == NULL || Matrice->IndiceColonneDeU == NULL ) {
  printf("Factorisation LU, sous-programme LU_ChainageParLigneDeU: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}

LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
ElmDeU              = Matrice->ElmDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
Elm            = Matrice->Elm;
LIndiceColonne = Matrice->LIndiceColonne; 

OrdreLigne = Matrice->OrdreLigne; 

ilNew = 0;
for ( Kp = 0 ; Kp < Matrice->Rang ; Kp++ ) {
  Ligne = OrdreLigne[Kp];
  il = Ldeb[Ligne];	
	Nb = LNbTerm[Ligne];
	LdebParLigneDeU    [Kp] = ilNew;
	NbTermesParLigneDeU[Kp] = Nb;
  memcpy( (char *) &(IndiceColonneDeU[ilNew]), (char *) &(LIndiceColonne[il]), Nb * sizeof( int    ) );
  memcpy( (char *) &(ElmDeU[ilNew])          , (char *) &(Elm[il])           , Nb * sizeof( double ) );
  ilNew += Nb;
}	 
Matrice->IndexLibreDeU = ilNew;
Matrice->DernierIndexLibreDeU = ilNew;

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*    Classement des lignes de U et des colonnes de U dans l'ordre croissant de l'elimination       */
/*    ordonnee. Le premier terme de chaque ligne ou colonne sera automatiquement le terme pivot.    */

void LU_StockageDeUDansLOrdreCroissantDesIndices( MATRICE * Matrice )
{
int ilDeb; int ilMax; int il; int IndiceColonne; double X; char OnInverse; int Kp; int Rang; 
int * LdebParLigneDeU; int * NbTermesParLigneDeU; int * IndiceColonneDeU   ; int * IndiceLigneDeL;
double * ElmDeU       ; double * ElmDeL           ; int * InverseOrdreColonne; int * IndexKpDeUouL ;
int NbTermesDeU      ; int iKp;

NbTermesDeU = Matrice->IndexLibreDeU;
Matrice->IndexKpDeUouL = (int *) malloc( NbTermesDeU * sizeof( int ) );
if ( Matrice->IndexKpDeUouL == NULL ) {
  printf("Factorisation LU, sous-programme LU_StockageDeUDansLOrdreCroissantDesIndices: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}

Rang = Matrice->Rang;

LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;
IndiceLigneDeL      = Matrice->IndiceLigneDeL;
ElmDeL              = Matrice->ElmDeL;
IndexKpDeUouL       = Matrice->IndexKpDeUouL;

InverseOrdreColonne = Matrice->InverseOrdreColonne;

for ( il = 0 ; il < NbTermesDeU ; il++ ) {
  IndexKpDeUouL[il] = InverseOrdreColonne[IndiceColonneDeU[il]];
}

/* Classement des lignes de U et des colonnes de U dans l'ordre croissant de indices */
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  /* Triangle U */
  ilDeb = LdebParLigneDeU[Kp]; 
  ilMax = ilDeb + NbTermesParLigneDeU[Kp];
  ilMax--;
  ilDeb++;
  OnInverse = OUI_LU;
  while ( OnInverse == OUI_LU ) {
    OnInverse = NON_LU;
    il = ilDeb;
    while ( il < ilMax ) {
      if ( IndexKpDeUouL[il] > IndexKpDeUouL[il+1] ) {
        OnInverse   = OUI_LU;
        IndiceColonne = IndiceColonneDeU[il+1];
				iKp           = IndexKpDeUouL[il+1];
        X             = ElmDeU[il+1];				
        IndiceColonneDeU[il+1] = IndiceColonneDeU[il];
				IndexKpDeUouL   [il+1] = IndexKpDeUouL[il];
        ElmDeU          [il+1] = ElmDeU[il];				
        IndiceColonneDeU[il]   = IndiceColonne;
        IndexKpDeUouL   [il]   = iKp;
        ElmDeU          [il]   = X;
      }
      il++;
    }
  }  
}
/* Recopie du triangle U dans le triangle L (car matrice symetrique ) */
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  il    = LdebParLigneDeU[Kp];
  ilMax = il + NbTermesParLigneDeU[Kp];
  ElmDeL[il] = 1.0;
  X = ElmDeU[il];
  il++;
  while ( il < ilMax ) { 
    /* Calcul du terme modifie */
    ElmDeL[il] = ElmDeU[il] * X;
    IndiceLigneDeL[il] = IndiceColonneDeU[il];
    il++;
  }
}

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                           Chainage par colonne du triangle U                                     */
/* C'est en particulier utilise par la LU update                                                    */

void LU_ChainageParColonneDeU( MATRICE * Matrice )
{
int i; int il; int ilMax; int ilC; int Rang; int Kp; int Nb; double * ElmDeU;
int * OrdreUcolonne              ; int KpColonne;  int * NbTermesParLigneDeU  ;
int * StockageColonneVersLigneDeU; int * LdebParLigneDeU      ; double * ElmDeUParColonne   ;
int * StockageLigneVersColonneDeU; int * CapaciteParColonneDeU; int * IndiceColonneDeU     ;
int * InverseOrdreColonne        ; int * CdebParColonneDeU    ; int * NbTermesParColonneDeU;
int * IndiceLigneParColonneDeU   ; int * CapaciteParLigneDeU  ; char ContexteDeLaFactorisation;

/* Cette routine peut etre appelee par l'estimation d'etat (Sylvain), qui veut aussi avoir un
   un chainage par colonne de U. Cependant, suite a des modifs d'architecture de la LU, ce
	 chainage de U par colonne est deja disponible */
if ( Matrice->ContexteDeLaFactorisation == LU_GENERAL ) {
 return;
}

ContexteDeLaFactorisation = Matrice->ContexteDeLaFactorisation;
Rang = Matrice->Rang;
			 
/* Pour avoir une marge disponible pour l'update de la LU */
i = Matrice->IndexLibreDeU + ( 2 * INCREMENT_DALLOCATION_DE_U );
Matrice->DernierIndexLibreDeUParColonne = i - 1; 

/* Limite au nombre d'Update avant refactorisation */
/*Matrice->LimiteUpdatePourRefactorisation = Matrice->DernierIndexLibreDeUParColonne << 2;*/
Matrice->LimiteUpdatePourRefactorisation = Matrice->DernierIndexLibreDeUParColonne * 10;  
Matrice->CdebParColonneDeU     = Matrice->ColonnePrecedente;
Matrice->NbTermesParColonneDeU = Matrice->ColonneSuivante;

i = Matrice->DernierIndexLibreDeUParColonne + 1;
Matrice->IndiceLigneParColonneDeU    = (int *) malloc( i * sizeof( int ) );
Matrice->StockageColonneVersLigneDeU = (int *) malloc( i * sizeof( int ) );
Matrice->ElmDeUParColonne            = (double *) malloc( i * sizeof( double ) );

i = Matrice->DernierIndexLibreDeU + 1;
Matrice->StockageLigneVersColonneDeU = (int *) malloc( i * sizeof( int ) );

Matrice->CapaciteParLigneDeU   = (int *) malloc( Rang * sizeof( int ) );
Matrice->CapaciteParColonneDeU = (int *) malloc( Rang * sizeof( int ) );

Matrice->OrdreUcolonne        = (int *) malloc( Rang * sizeof( int ) );
Matrice->InverseOrdreUcolonne = (int *) malloc( Rang * sizeof( int ) );

if ( Matrice->IndiceLigneParColonneDeU    == NULL ||
		 Matrice->StockageColonneVersLigneDeU == NULL || Matrice->ElmDeUParColonne    == NULL ||		 
     Matrice->StockageLigneVersColonneDeU == NULL || Matrice->CapaciteParLigneDeU == NULL ||
     Matrice->CapaciteParColonneDeU       == NULL || Matrice->OrdreUcolonne       == NULL ||
		 Matrice->InverseOrdreUcolonne        == NULL ) {

  printf("Factorisation LU, sous-programme LU_ChainageParColonneDeU: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");

  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}

OrdreUcolonne = Matrice->OrdreUcolonne;
for ( i = 0 ; i < Rang ; i++ ) OrdreUcolonne[i] = i;
memcpy( (char *) Matrice->InverseOrdreUcolonne, (char *) OrdreUcolonne, Rang * sizeof( int ) );

NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
CapaciteParLigneDeU = Matrice->CapaciteParLigneDeU;
LdebParLigneDeU     = Matrice->LdebParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;
InverseOrdreColonne = Matrice->InverseOrdreColonne;

CdebParColonneDeU        = Matrice->CdebParColonneDeU;
NbTermesParColonneDeU    = Matrice->NbTermesParColonneDeU;
IndiceLigneParColonneDeU = Matrice->IndiceLigneParColonneDeU;
ElmDeUParColonne         = Matrice->ElmDeUParColonne;

StockageColonneVersLigneDeU = Matrice->StockageColonneVersLigneDeU;
StockageLigneVersColonneDeU = Matrice->StockageLigneVersColonneDeU;
CapaciteParColonneDeU       = Matrice->CapaciteParColonneDeU;

memset( (char *) NbTermesParColonneDeU, 0, Rang * sizeof( int ) );
memset( (char *) CapaciteParColonneDeU, 0, Rang * sizeof( int ) );

/* On balaye les lignes de U pour compter les termes dans la colonne */
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  Nb =  NbTermesParLigneDeU[Kp];
  CapaciteParLigneDeU[Kp] = Nb;
  il    = LdebParLigneDeU[Kp];  
  ilMax = il + Nb;
  while ( il < ilMax ) {     
		NbTermesParColonneDeU[InverseOrdreColonne[IndiceColonneDeU[il]]]++;		            
    il++;
  }
}
/* Initialisation de l'index debut des colonnes */
ilC = 0;
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  CdebParColonneDeU[Kp] = ilC;
  ilC += NbTermesParColonneDeU[Kp];
	NbTermesParColonneDeU[Kp] = 0;
}
Matrice->IndexLibreDeUParColonne = ilC;

/* En balayant Kp a l'envers, le terme pivot de chaque colonne se retrouve automatiquement au debut */
for ( Kp = Rang - 1 ; Kp >= 0 ; Kp-- ) {
  il    = LdebParLigneDeU[Kp];  
  ilMax = il + NbTermesParLigneDeU[Kp];
  while ( il < ilMax ) {     
    KpColonne = InverseOrdreColonne[IndiceColonneDeU[il]];
		/* Automatiquement range dans l'ordre decroissant de Kp */
		ilC = CdebParColonneDeU[KpColonne] + NbTermesParColonneDeU[KpColonne];
		IndiceLigneParColonneDeU[ilC] = Kp;
		
    StockageColonneVersLigneDeU[ilC] = il;
    StockageLigneVersColonneDeU[il]  = ilC;
		
		ElmDeUParColonne[ilC] = ElmDeU[il];

		NbTermesParColonneDeU[KpColonne]++;		            
    CapaciteParColonneDeU[KpColonne]++;
    il++;
  }	
}

return;  
}

/*--------------------------------------------------------------------------------------------------*/
/*                           Chainage par colonne du triangle U hors simplexe                       */
/*                           (le stockage definitif de U par ligne est suppose disponible)          */

void LU_ChainageParColonneDeUHorsSimplexe( MATRICE * Matrice )
{
int il; int ilMax; int ilC; int Rang; int Kp; int KpColonne; int NbTrm_U;
int * LdebParLigneDeU; int * NbTermesParLigneDeU; double * ElmDeU; int * IndiceColonneDeU;
int * CdebParColonneDeU; int * NbTermesParColonneDeU; double * ElmDeUParColonne; int * IndiceLigneParColonneDeU;
int * InverseOrdreColonne;

/*if ( Matrice->ContexteDeLaFactorisation != LU_SIMPLEXE ) return;*/

Rang = Matrice->Rang;

NbTrm_U = Matrice->IndexLibreDeU + 10;
Matrice->DernierIndexLibreDeU = NbTrm_U;

Matrice->CdebParColonneDeU     = Matrice->ColonnePrecedente;
Matrice->NbTermesParColonneDeU = Matrice->ColonneSuivante;
    
Matrice->ElmDeUParColonne         = (double *) malloc( NbTrm_U * sizeof( double ) );
Matrice->IndiceLigneParColonneDeU = (int *)    malloc( NbTrm_U * sizeof( int    ) );

if ( Matrice->ElmDeUParColonne == NULL || Matrice->IndiceLigneParColonneDeU == NULL ) {
  printf("Factorisation LU, sous-programme LU_ChainageParColonneDeUHorsSimplexe: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}
 
CdebParColonneDeU        = Matrice->CdebParColonneDeU;
NbTermesParColonneDeU    = Matrice->NbTermesParColonneDeU;
ElmDeUParColonne         = Matrice->ElmDeUParColonne;
IndiceLigneParColonneDeU = Matrice->IndiceLigneParColonneDeU;

NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
LdebParLigneDeU     = Matrice->LdebParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;
InverseOrdreColonne = Matrice->InverseOrdreColonne;   

memset( (char *) NbTermesParColonneDeU, 0, Rang * sizeof( int ) );

/* On balaye les lignes de U pour compter les termes dans la colonne */
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  il    = LdebParLigneDeU[Kp];  
  ilMax = il + NbTermesParLigneDeU[Kp];
  while ( il < ilMax ) {     
		NbTermesParColonneDeU[InverseOrdreColonne[IndiceColonneDeU[il]]]++;		            
    il++;
  }
}
/* Initialisation de l'index debut des colonnes */
ilC = 0;
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  CdebParColonneDeU[Kp] = ilC;
  ilC += NbTermesParColonneDeU[Kp];
	NbTermesParColonneDeU[Kp] = 0;
}

/* En balayant Kp a l'envers, le terme pivot de chaque colonne se retrouve automatiquement au debut */
for ( Kp = Rang - 1 ; Kp >= 0 ; Kp-- ) {
  il    = LdebParLigneDeU[Kp];  
  ilMax = il + NbTermesParLigneDeU[Kp];
  while ( il < ilMax ) {     
    KpColonne = InverseOrdreColonne[IndiceColonneDeU[il]];
		/* Automatiquement range dans l'ordre decroissant de Kp */
		ilC = CdebParColonneDeU[KpColonne] + NbTermesParColonneDeU[KpColonne];
		IndiceLigneParColonneDeU[ilC] = Kp;				
		ElmDeUParColonne[ilC] = ElmDeU[il];
		NbTermesParColonneDeU[KpColonne]++;		            
    il++;
  }	
}

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* Chainage de L par ligne pour la resolution transpose */

void LU_ChainageParLigneDeL( MATRICE * Matrice, char * AllocOk )
{
int i; int il; int ilMax; int ilL; int Rang; int KpLigne; int Kp; 
int * InverseOrdreLigne; int * LdebParLigneDeL   ; int * NbTermesParLigneDeL;
double * ElmDeLParLigne; int * CdebParColonneDeL ; int * IndiceColonneParLigneDeL;
int * NbTermesParColonneDeL ; int * IndiceLigneDeL; double * ElmDeL;

/*if ( Matrice->ContexteDeLaFactorisation != LU_SIMPLEXE ) return;*/

*AllocOk = OUI_LU;

Rang = Matrice->Rang;
 
/* Pour avoir une marge disponible pour l'update de la LU */
i = Matrice->IndexLibreDeL + 1;

Matrice->LdebParLigneDeL          = (int *)   malloc( Rang * sizeof( int ) );
Matrice->NbTermesParLigneDeL      = (int *)   malloc( Rang * sizeof( int ) );
Matrice->IndiceColonneParLigneDeL = (int *)   malloc( i * sizeof( int   ) );
Matrice->ElmDeLParLigne           = (double *) malloc( i * sizeof( double ) );

if ( Matrice->LdebParLigneDeL          == NULL || Matrice->NbTermesParLigneDeL == NULL ||
     Matrice->IndiceColonneParLigneDeL == NULL || Matrice->ElmDeLParLigne      == NULL 
	 ) {
  printf("Factorisation LU, sous-programme LU_ChainageParColonneDeU: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
	if ( Matrice->LdebParLigneDeL != NULL ) free( Matrice->LdebParLigneDeL );
	if ( Matrice->NbTermesParLigneDeL != NULL ) free( Matrice->NbTermesParLigneDeL );
	if ( Matrice->IndiceColonneParLigneDeL != NULL ) free( Matrice->IndiceColonneParLigneDeL );
	if ( Matrice->ElmDeLParLigne != NULL ) free( Matrice->ElmDeLParLigne );
	*AllocOk = NON_LU;
	return;
	/*
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee );
	*/
}

CdebParColonneDeL     = Matrice->CdebParColonneDeL;
NbTermesParColonneDeL = Matrice->NbTermesParColonneDeL;
IndiceLigneDeL        = Matrice->IndiceLigneDeL;

InverseOrdreLigne = Matrice->InverseOrdreLigne;

ElmDeLParLigne = Matrice->ElmDeLParLigne;
ElmDeL         = Matrice->ElmDeL;

LdebParLigneDeL          = Matrice->LdebParLigneDeL;
NbTermesParLigneDeL      = Matrice->NbTermesParLigneDeL;
IndiceColonneParLigneDeL = Matrice->IndiceColonneParLigneDeL ;
ElmDeLParLigne           = Matrice->ElmDeLParLigne ;

memset( (char *) NbTermesParLigneDeL, 0, Rang * sizeof( int ) );

for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  il = CdebParColonneDeL[Kp];  
  ilMax = il + NbTermesParColonneDeL[Kp]; 
  while ( il < ilMax ) {     
    NbTermesParLigneDeL[InverseOrdreLigne[IndiceLigneDeL[il]]]++;		
    il++;
  }
}
/* Initialisation de l'index debut des lignes */
ilL = 0;
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  LdebParLigneDeL[Kp] = ilL;
  ilL += NbTermesParLigneDeL[Kp];
	NbTermesParLigneDeL[Kp] = 0;
}

/* En balayant Kp a l'envers, le terme pivot de chaque colonne se retrouve automatiquement au debut */
for ( Kp = Rang - 1 ; Kp >= 0 ; Kp-- ) {
  il = CdebParColonneDeL[Kp];  
  ilMax = il + NbTermesParColonneDeL[Kp]; 
  while ( il < ilMax ) {
    KpLigne = InverseOrdreLigne[IndiceLigneDeL[il]];
		ilL = LdebParLigneDeL[KpLigne] + NbTermesParLigneDeL[KpLigne];
		IndiceColonneParLigneDeL[ilL] = Kp;
	  ElmDeLParLigne          [ilL] = ElmDeL[il];
    NbTermesParLigneDeL[KpLigne]++;			
    il++;
	}
}

return;
}

























































