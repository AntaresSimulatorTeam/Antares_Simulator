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

   FONCTION: Calcul de A_BARRE_S = B-1 * AS
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h" 

# include "spx_fonctions.h"
# include "spx_define.h"   

# include "lu_define.h"

# define DEBUG NON_SPX

/*----------------------------------------------------------------------------*/

void SPX_CalculerABarreSAvecBaseReduite( PROBLEME_SPX * Spx, char * HyperCreuxInitial,
                                         char * CalculEnHyperCreux, char * TypeDEntree,
																				 char * TypeDeSortie )
{
int il; int ilMax; char Save; char SecondMembreCreux; double * ABarreS; int * CdebProblemeReduit;
int * CNbTermProblemeReduit; int * IndicesDeLigneDesTermesDuProblemeReduit; int i; 
double * ValeurDesTermesDesColonnesDuProblemeReduit; int NbTermesNonNuls; int * CntDeABarreSNonNuls;
int RangDeLaMatriceFactorisee; 

ABarreS = Spx->ABarreS;
CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	

CdebProblemeReduit = Spx->CdebProblemeReduit;
CNbTermProblemeReduit = Spx->CNbTermProblemeReduit;
IndicesDeLigneDesTermesDuProblemeReduit = Spx->IndicesDeLigneDesTermesDuProblemeReduit;
ValeurDesTermesDesColonnesDuProblemeReduit = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;

*CalculEnHyperCreux = NON_SPX;
*HyperCreuxInitial  = NON_SPX;
if ( Spx->TypeDeCreuxDeLaBase == BASE_HYPER_CREUSE && Spx->CalculABarreSEnHyperCreux == OUI_SPX &&
     Spx->FaireDuRaffinementIteratif <= 0 ) {
  if ( CNbTermProblemeReduit[Spx->VariableEntrante] < TAUX_DE_REMPLISSAGE_POUR_VECTEUR_HYPER_CREUX * RangDeLaMatriceFactorisee ) {
    *CalculEnHyperCreux = OUI_SPX;
    *HyperCreuxInitial  = OUI_SPX;		
  }
}

NbTermesNonNuls = 0;

if ( *CalculEnHyperCreux == OUI_SPX ) {	
  il = CdebProblemeReduit[Spx->VariableEntrante];		   
  ilMax = il + CNbTermProblemeReduit[Spx->VariableEntrante];	
  while ( il < ilMax ) {
		ABarreS[NbTermesNonNuls] = ValeurDesTermesDesColonnesDuProblemeReduit[il]; /* On evite ainsi la raz qui serait necessaire de tout le vecteur ABarreS */						
		CntDeABarreSNonNuls[NbTermesNonNuls] = IndicesDeLigneDesTermesDuProblemeReduit[il];   
    NbTermesNonNuls++;			
    il++;
	}	
	*TypeDEntree  = COMPACT_LU;
	*TypeDeSortie = COMPACT_LU;			
}
else {
	for ( i = 0 ; i < RangDeLaMatriceFactorisee ; i++ ) ABarreS[i] = 0;
	
  il = CdebProblemeReduit[Spx->VariableEntrante];
  ilMax = il + CNbTermProblemeReduit[Spx->VariableEntrante];
  while ( il < ilMax ) {
	  ABarreS[IndicesDeLigneDesTermesDuProblemeReduit[il]] = ValeurDesTermesDesColonnesDuProblemeReduit[il];				  
    il++;
  }
  *TypeDEntree  = VECTEUR_LU;
  *TypeDeSortie = VECTEUR_LU;		  
}
  
/* Resolution du systeme */
Save = OUI_LU; /* Mis a NON_LU si pas de LU update */
SecondMembreCreux = OUI_LU;

SPX_ResolutionDeSysteme( Spx, *TypeDEntree, ABarreS, CntDeABarreSNonNuls, &NbTermesNonNuls,
												 TypeDeSortie, *CalculEnHyperCreux, Save, SecondMembreCreux );							 

												 
/*
printf("---------------- CalculerABarreS  Spx->NombreDeChangementsDeBase %d -------------\n",Spx->NombreDeChangementsDeBase);
if ( *TypeDEntree == VECTEUR_LU ) printf("apres resolution TypeDEntree = VECTEUR_LU\n");
if ( *TypeDEntree == COMPACT_LU ) printf("apres resolution TypeDEntree = COMPACT_LU\n");
if ( *TypeDEntree == ADRESSAGE_INDIRECT_LU ) printf("apres resolution TypeDEntree = ADRESSAGE_INDIRECT_LU\n");
if ( *TypeDeSortie == VECTEUR_LU ) printf("apres resolution TypeDeSortie = VECTEUR_LU\n");
if ( *TypeDeSortie == COMPACT_LU ) printf("apres resolution TypeDeSortie = COMPACT_LU\n");		
*/
												 
/* Eventuellement forme produit de l'inverse */
if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
  printf("CalculerBBarre AppliquerLesEtaVecteurs pas operationnel \n");
	exit(0);
  SPX_AppliquerLesEtaVecteurs( Spx, ABarreS, CntDeABarreSNonNuls, &NbTermesNonNuls, *CalculEnHyperCreux, *TypeDeSortie );
}

Spx->NbABarreSNonNuls = NbTermesNonNuls;

# if VERIFICATION_ABARRES == OUI_SPX
printf("---------------- CalculerABarreS  Spx->NombreDeChangementsDeBase %d -------------\n",Spx->NombreDeChangementsDeBase);
if ( *TypeDEntree == VECTEUR_LU ) printf("apres resolution TypeDEntree = VECTEUR_LU\n");
if ( *TypeDEntree == COMPACT_LU ) printf("apres resolution TypeDEntree = COMPACT_LU\n");
if ( *TypeDeSortie == VECTEUR_LU ) printf("apres resolution TypeDeSortie = VECTEUR_LU\n");
if ( *TypeDeSortie == COMPACT_LU ) printf("apres resolution TypeDeSortie = COMPACT_LU\n");					
{
double * Buff; int i; int Var; int ic; int icMx; double * Sortie; char Arret;
int * VariableEnBaseDeLaContrainte; int rr;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
Buff = (double *) malloc( RangDeLaMatriceFactorisee * sizeof( double ) );
Sortie = (double *) malloc( RangDeLaMatriceFactorisee * sizeof( double ) );
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) Sortie[r]= 0;
if ( *TypeDeSortie == COMPACT_LU ) {
  for ( i = 0 ; i < NbTermesNonNuls ; i++ ) {
	  Sortie[CntDeABarreSNonNuls[i]] = ABarreS[i];
	}
}
else {
  for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) Sortie[r] = ABarreS[r];	
}
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) Buff[r]= 0;
Var = Spx->VariableEntrante;
ic = Cdeb[Var];
icMx = ic + CNbTerm[Var];
while ( ic < icMx ) {
  Cnt = NumeroDeContrainte[ic];
	r = OrdreLigneDeLaBaseFactorisee[Cnt];
	if ( r < RangDeLaMatriceFactorisee ) {
	  Buff[r] = ACol[ic];
	}			
  ic++;
}

for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {
  if ( Sortie[r] == 0 ) continue;
  Var = Spx->VariableEnBaseDeLaContrainte[Spx->ColonneDeLaBaseFactorisee[r]];
	ic = Spx->Cdeb[Var];
	icMx = ic + Spx->CNbTerm[Var];
	while ( ic < icMx ) {
    Cnt = NumeroDeContrainte[ic];
	  rr = OrdreLigneDeLaBaseFactorisee[Cnt];
	  if ( rr < RangDeLaMatriceFactorisee ) {	
	    Buff[rr] -= ACol[ic] * Sortie[r];
		}
	  ic++;
	}
}
Arret = NON_SPX;
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {
	if ( fabs( Buff[r] ) > 1.e-7 ) {
	  printf("r = %d   ecart %e\n",r,Buff[r]);
    Var = Spx->VariableEnBaseDeLaContrainte[Spx->ColonneDeLaBaseFactorisee[r]];
		if ( Spx->OrigineDeLaVariable[Var] != NATIVE ) printf(" variable non native\n");
		else printf(" variable native\n");		
		Arret = OUI_SPX;
	}
}
if ( Arret == OUI_SPX ) {
 printf("Verif ABarreS  not OK\n");
 exit(0);
}
printf("Fin verif ABarreS  OK\n");
free( Buff );
free( Sortie );

}

SPX_VerifierLesVecteursDeTravail( Spx );

# endif

return;
}

/*----------------------------------------------------------------------------*/


