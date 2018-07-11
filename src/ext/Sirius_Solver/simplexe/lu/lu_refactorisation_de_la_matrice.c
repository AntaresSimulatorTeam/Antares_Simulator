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

   FONCTION: Refactorisation de la matrice (l'ordre d'elimination
             a deja ete calcule). Cas d'une matrice non symetrique.
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/

void LU_EliminationDUneLigneRefactorisationNonSymetrique_B(   
                                                          int Kp,
                                                          int * LdebParLigneDeU,
																													int * NbTermesParLigneDeU,
																													int * IndiceColonneDeU,
																													double * ElmDeU,
																													int * CdebParColonneDeL,  
																													int * NbTermesParColonneDeL,
																													int * IndiceLigneDeL,
																													double * ElmDeL,
																													int * InverseOrdreLigne,
																													int * InverseOrdreColonne,
																													double * W,
																													int   Symetrie,
																													double PivotMin,
                                                          int * ProblemeDansLaRefactorisation
                                                        )
																																																								
{
int il; int il1; int il1Max; int ic; int ic1; int ic1Max; int KpU  ; int KpL  ; 
double UnSurValeurDuPivot     ; int LignePivot  ; int ColonnePivot   ; int ilMax;
double ValTermeColonnePivot   ; int icMax;

/* Transfert de la ligne dans un tableau de travail */

il1 = LdebParLigneDeU[Kp];
/* Le terme pivot est deja range au debut de la ligne */
ColonnePivot = IndiceColonneDeU[il1];
il1Max = il1 + NbTermesParLigneDeU[Kp];
il = il1;
while ( il < il1Max ) {
  W[IndiceColonneDeU[il]] = ElmDeU[il];
  il++;
}

/* Le terme pivot est deja range au debut de la ligne */
if ( fabs( W[ColonnePivot] ) < PivotMin ) {
  /* Pivot trop petit => on abandonne la refactorisation. Il vaut mieux relancer
     une factorisation complete pour trouver des pivots mieux calibres */
  *ProblemeDansLaRefactorisation = OUI_LU;
  return;     
}

UnSurValeurDuPivot = 1. / W[ColonnePivot];
W[ColonnePivot] = 0.0;

/* On met a jour les lignes du triangle U actif */
ic = CdebParColonneDeL[Kp];
/* Le terme pivot est deja range au debut de la colonne */
LignePivot = IndiceLigneDeL[ic];
ElmDeL[ic] = 1.;
icMax = ic + NbTermesParColonneDeL[Kp];
ic++;
while ( ic < icMax ) {
  /*Ligne = IndiceLigneDeL[ic];*/
  ValTermeColonnePivot = ElmDeL[ic] * UnSurValeurDuPivot;
  ElmDeL[ic] = ValTermeColonnePivot;  
  KpU = InverseOrdreLigne[/*Ligne*/IndiceLigneDeL[ic]]; 
  /* On balaye la ligne du triangle U et on la met a jour */
  il = LdebParLigneDeU[KpU];
  ilMax = il + NbTermesParLigneDeU[KpU];  
  while ( il < ilMax ) {
    /* Calcul du terme modifie */
    ElmDeU[il]-= W[IndiceColonneDeU[il]] * ValTermeColonnePivot;
    il++;
  }	
  ic++;
}

if ( Symetrie == NON_LU ) {
  /* Si la structure est symetrique, pas besoin de remettre a 0 */
  il = il1;
  while ( il < il1Max ) {
    W[IndiceColonneDeU[il]] = 0.0;
    il++;;
  }
}

/* On met a jour les lignes du triangle L actif */
ic1    = CdebParColonneDeL[Kp];
ic1Max = ic1 + NbTermesParColonneDeL[Kp];
ic = ic1;
while ( ic < ic1Max ) {
  W[IndiceLigneDeL[ic]] = ElmDeL[ic];
  ic++;;
}
W[LignePivot] = 0.0;

/* On met a jour les colonnes du triangle L actif */
il = LdebParLigneDeU[Kp];
ElmDeU[il] = UnSurValeurDuPivot; 	     
ilMax = il + NbTermesParLigneDeU[Kp];
il++;
while ( il < ilMax ) {
  /*Colonne = IndiceColonneDeU[il];*/
  ValTermeColonnePivot = ElmDeU[il];
  KpL = InverseOrdreColonne[/*Colonne*/IndiceColonneDeU[il]]; 
  ic = CdebParColonneDeL[KpL];
  icMax = ic + NbTermesParColonneDeL[KpL];  
  while ( ic < icMax ) {
    /* Calcul du terme modifie */
    ElmDeL[ic]-= W[IndiceLigneDeL[ic]] * ValTermeColonnePivot;
    ic++;
  }
  il++;
}

ic = ic1;
while ( ic < ic1Max ) {
  W[IndiceLigneDeL[ic]] = 0.0;
  ic++;;
}

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* On recopie la matrice d'entree dans les triangles L et U */

void LU_InitTrianglesRefactorisationNonSymetrique( MATRICE * Matrice,
                                                   double  * ValeurDesTermesDeLaMatrice, 
                                                   int    * IndicesDeLigne ,  
                                                   int    * Ideb,  
                                                   int    * NbTermesDesColonnes, 
						                                       int    * ProblemeDansLaRefactorisation
                                                 )
{
int Ligne; int Colonne; int Kp   ; int il   ; int ilMax     ; int ic1 ; int ic1Max;
int ic   ; int icSV   ; int icMax; double * W; int NbElements; int Rang; int il1   ; 

int * Ldeb     ; int * Lsui    ; int * Lcol; char Symetrie; char FaireScaling;
double * ScaleX ; double * ScaleB; double ScX ; double ScB   ; int k;

int * LdebParLigneDeU  ; int * NbTermesParLigneDeU  ; int * IndiceColonneDeU; double * ElmDeU;
int * CdebParColonneDeL; int * NbTermesParColonneDeL; int * IndiceLigneDeL  ; double * ElmDeL;

FaireScaling = Matrice->FaireScaling;
ScaleX       = Matrice->ScaleX;
ScaleB       = Matrice->ScaleB;

Symetrie = NON_LU;
if ( Matrice->LaMatriceEstSymetriqueEnStructure == OUI_LU ||
     Matrice->LaMatriceEstSymetrique            == OUI_LU ) {
  Symetrie = OUI_LU;     
}

Rang = Matrice->Rang;

NbElements = Matrice->NombreDeTermesDeLaMatriceAFactoriser;

/* Stockage par lignes de la matrice */
Ldeb = (int *) malloc( Rang       * sizeof( int ) );
Lsui = (int *) malloc( NbElements * sizeof( int ) );
Lcol = (int *) malloc( NbElements * sizeof( int ) );
if ( Ldeb == NULL || Lsui == NULL || Lcol == NULL ) {
  free( Ldeb ); free( Lsui ); free( Lcol );  
  printf("Refactorisation LU: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  *ProblemeDansLaRefactorisation = (int) OUI_LU;
  return;
}
for ( il = 0 ; il < Rang ; il++ ) Ldeb[il] = -1;

for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {
  ic    = Ideb[Colonne];
  icMax = ic + NbTermesDesColonnes[Colonne];  
  while ( ic < icMax ) {  
    Ligne = IndicesDeLigne[ic];
    icSV  = Ldeb[Ligne];
    Ldeb[Ligne] = ic;
    Lcol[ic]    = Colonne;
    Lsui[ic]    = icSV;   
    /* */    
    ic++;			 
  }
}

W = Matrice->SolutionIntermediaire;
memset( W , 0 , Rang * sizeof( double ) );

LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;

CdebParColonneDeL     = Matrice->CdebParColonneDeL;
NbTermesParColonneDeL = Matrice->NbTermesParColonneDeL;
IndiceLigneDeL        = Matrice->IndiceLigneDeL;
ElmDeL                = Matrice->ElmDeL;  

for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  /* 1- Triangle U */
  /* L'indice ligne du premier terme du triangle L nous donne la ligne a recopier
     dans U */
  Ligne = IndiceLigneDeL[CdebParColonneDeL[Kp]];
  il1 = Ldeb[Ligne]; 
  il  = il1;
  if ( FaireScaling != OUI_LU ) {
    while ( il >= 0 ) {
      W[Lcol[il]] = ValeurDesTermesDeLaMatrice[il];        
      il = Lsui[il];
    }
  }
  else {
    /* Cas du scaling */
    ScB = ScaleB[Ligne];  
    while ( il >= 0 ) {
      k = Lcol[il];
      W[k] = ValeurDesTermesDeLaMatrice[il] * ScaleX[k] * ScB;        
      il = Lsui[il];
    }    
  }  
  /* Initialisation du triangle U */
  il = LdebParLigneDeU[Kp];
  ilMax = il + NbTermesParLigneDeU[Kp];  
  while ( il < ilMax ) {
    ElmDeU[il] = W[IndiceColonneDeU[il]]; 
    il++;
  }
  /* Raz de W pour la suite */

  if ( Symetrie == NON_LU ) {
    /* Si la structure est symetrique, pas besoin de remettre a 0 */
    il = il1;
    while ( il >= 0 ) {
      W[Lcol[il]] = 0.0;
      il = Lsui[il];
    }
  }
  /* 2- Triangle L */
  /* L'indice colonne du premier terme du triangle U nous donne la colonne a recopier
     dans L */
  Colonne = IndiceColonneDeU[LdebParLigneDeU[Kp]];
  ic1     = Ideb[Colonne];
  ic1Max  = ic1 + NbTermesDesColonnes[Colonne];
  ic = ic1;
  if ( FaireScaling != OUI_LU ) { 
    while ( ic <ic1Max ) {  
      W[IndicesDeLigne[ic]] = ValeurDesTermesDeLaMatrice[ic];
      ic++;	 
    }
  }
  else {
    /* Cas du scaling */
    ScX = ScaleX[Colonne];
    while ( ic <ic1Max ) {
      k = IndicesDeLigne[ic];
      W[k] = ValeurDesTermesDeLaMatrice[ic] * ScX * ScaleB[k];
      ic++;	 
    }
  }  
  /* Initialisation du triangle L */
  ic = CdebParColonneDeL[Kp];
  icMax = ic + NbTermesParColonneDeL[Kp]; 
  while ( ic < icMax ) {
    ElmDeL[ic] = W[IndiceLigneDeL[ic]];
    ic++;
  }
  /* Raz de W pour la suite */
  ic = ic1;
  while ( ic <ic1Max ) {  
    W[IndicesDeLigne[ic]] = 0.0;
    ic++;	 
  }       
}

free( Ldeb );  
free( Lsui );  
free( Lcol );

return;
}

/*--------------------------------------------------------------------------------------------------*/
   
void LU_RefactorisationNonSymetrique( MATRICE * Matrice ,          /* Pointeur sur l'objet matrice qui a deja
                                                                      ete factorise (c'est le pointeur retourne
								                                                      par LU_Factorisation) */
                                      MATRICE_A_FACTORISER * Mat   /* Contient les nouvelles valeurs numeriques 
				                                                              de la matrice a refactoriser */
				                            )  	    
{
double * ValeurDesTermesDeLaMatrice; int * IndicesDeLigne; int * Ideb; int * NbTermesDesColonnes;
double * W; double PivotMin; double X;
int * LdebParLigneDeU; int * NbTermesParLigneDeU; int * IndiceColonneDeU; double * ElmDeU;
int * CdebParColonneDeL; int * NbTermesParColonneDeL; int * IndiceLigneDeL; double * ElmDeL;
int * InverseOrdreLigne; int * InverseOrdreColonne; 
/*                     */
int Kp; int ProblemeDansLaRefactorisation; int Symetrie;

/*----------------------------------------------------------------------------------------------------*/  
ValeurDesTermesDeLaMatrice = Mat->ValeurDesTermesDeLaMatrice;
IndicesDeLigne             = Mat->IndicesDeLigne;
Ideb                       = Mat->IndexDebutDesColonnes;
NbTermesDesColonnes        = Mat->NbTermesDesColonnes;
/*----------------------------------------------------------------------------------------------------*/

ProblemeDansLaRefactorisation = NON_LU;

X = (double) ( Matrice->IndexLibreDeL + Matrice->IndexLibreDeU ) / (double) ( Matrice->Rang * Matrice->Rang );
/* Attention. Maintenant dans le cas LU_GENERAL, on resout le triangle U en utilisant le stockage
   de U par colonne. Comme la refactorisation autre que RefactorisationLL ne met a jour que le stockage
	 de U par ligne, il n'est pas souhaitable de l'utiliser ou alors il faudrait tenir compte du type
	 de refactorisation disponible au moment de la resolution. Pour l'instant il semble preferable de
	 ne pas compliquer les choses. Le cas echeant on pourra prevoir un indicateur permettant de choisir
	 le type de stockage a untiliser lors des resolutions */
	 
if ( X < 0.03 /*&& Matrice->FaireDuPivotageDiagonal == OUI_LU*/ || 1 ) {
  LU_RefactorisationLL( Matrice, ValeurDesTermesDeLaMatrice, IndicesDeLigne ,   
                        Ideb, NbTermesDesColonnes, &ProblemeDansLaRefactorisation );
  Mat->ProblemeDeFactorisation = ProblemeDansLaRefactorisation;
  return;
}

LU_InitTrianglesRefactorisationNonSymetrique( Matrice, ValeurDesTermesDeLaMatrice, IndicesDeLigne ,  
                                              Ideb, NbTermesDesColonnes, &ProblemeDansLaRefactorisation );
if ( ProblemeDansLaRefactorisation == OUI_LU ) goto FinRefactorisation;

Symetrie = NON_LU;
if ( Matrice->LaMatriceEstSymetriqueEnStructure == OUI_LU ||
     Matrice->LaMatriceEstSymetrique            == OUI_LU ) {
  Symetrie = OUI_LU;     
}
PivotMin = Matrice->PivotMin;
W        = Matrice->SolutionIntermediaire;
LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;
CdebParColonneDeL   = Matrice->CdebParColonneDeL;
NbTermesParColonneDeL = Matrice->NbTermesParColonneDeL;
IndiceLigneDeL        = Matrice->IndiceLigneDeL;
ElmDeL                = Matrice->ElmDeL;
InverseOrdreLigne   = Matrice->InverseOrdreLigne;
InverseOrdreColonne = Matrice->InverseOrdreColonne;

for ( Kp = 0 ; Kp < Matrice->Rang ; Kp++ ) {
  LU_EliminationDUneLigneRefactorisationNonSymetrique_B( Kp, LdebParLigneDeU, NbTermesParLigneDeU, IndiceColonneDeU, ElmDeU,
																												 CdebParColonneDeL, NbTermesParColonneDeL, IndiceLigneDeL, ElmDeL,
																												 InverseOrdreLigne, InverseOrdreColonne, W, Symetrie, PivotMin,
                                                         &ProblemeDansLaRefactorisation );	
  if ( ProblemeDansLaRefactorisation == OUI_LU ) goto FinRefactorisation;
}

FinRefactorisation:
Mat->ProblemeDeFactorisation = ProblemeDansLaRefactorisation;

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* On fait pareil qu'en symetrique car ça fonctionne aussi */
   
void LU_RefactorisationSymetrique( MATRICE * Matrice ,          /* Pointeur sur l'objet matrice qui a deja
                                                                   ete factorise (c'est le pointeur retourne
							                                                     par LU_Factorisation */
                                   MATRICE_A_FACTORISER * Mat   /* Contient les nouvelles valeurs numeriques 
				                                                           de la matrice a refactoriser */
				                         )
{

LU_RefactorisationNonSymetrique( Matrice , Mat );

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_RefactorisationLL( MATRICE * Matrice,
                           double  * ValeurDesTermesDeLaMatrice, 
                           int    * IndicesDeLigne,  
                           int    * Ideb,  
                           int    * NbTermesDesColonnes, 
						               int    * ProblemeDansLaRefactorisation
                         )
{
int Ligne; int Colonne; int Kp; int il; int ilMax; int icDeb; int ic; int icMax; int Rang; double Pivot;
/*int Kpp;*/ double X; double PivotMin; double * SecondMembre;
int * CdebParColonneDeL; int * NbTermesParColonneDeL; int * IndiceLigneDeL  ; double * ElmDeL; 
int * CdebParColonneDeU; int * IndiceLigneParColonneDeU; int * NbTermesParColonneDeU;  double * ElmDeUParColonne;
/*int * InverseOrdreColonne;*/ int * OrdreColonne; int * InverseOrdreLigne; int * OrdreLigne;
char FaireScaling; double * ScaleX; double * ScaleB; double ScX; 

FaireScaling = Matrice->FaireScaling;
ScaleX       = Matrice->ScaleX;
ScaleB       = Matrice->ScaleB;

Rang = Matrice->Rang;
PivotMin = Matrice->PivotMin;

CdebParColonneDeL     = Matrice->CdebParColonneDeL;
NbTermesParColonneDeL = Matrice->NbTermesParColonneDeL;
IndiceLigneDeL        = Matrice->IndiceLigneDeL;
ElmDeL                = Matrice->ElmDeL;

CdebParColonneDeU        = Matrice->CdebParColonneDeU;
IndiceLigneParColonneDeU = Matrice->IndiceLigneParColonneDeU;
NbTermesParColonneDeU    = Matrice->NbTermesParColonneDeU;
ElmDeUParColonne         = Matrice->ElmDeUParColonne;

/* On resout le triangle L avec les colonnes de la matrice d'entree */
SecondMembre = Matrice->SolutionIntermediaire;

/*InverseOrdreColonne = Matrice->InverseOrdreColonne;*/
OrdreColonne        = Matrice->OrdreColonne;

InverseOrdreLigne = Matrice->InverseOrdreLigne;
OrdreLigne        = Matrice->OrdreLigne;

for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  /* Colonne de la matrice d'entree a utiliser */
  Colonne = OrdreColonne[Kp];
	if ( FaireScaling != OUI_LU ) {
	  ic = Ideb[Colonne];
	  icMax = ic + NbTermesDesColonnes[Colonne];
    while ( ic < icMax ) {
	    SecondMembre[IndicesDeLigne[ic]] = ValeurDesTermesDeLaMatrice[ic];
	    ic++;
    }
	}
	else {
	  ScX = ScaleX[Colonne];
	  ic = Ideb[Colonne];
	  icMax = ic + NbTermesDesColonnes[Colonne];
    while ( ic < icMax ) {
	    Ligne = IndicesDeLigne[ic];
	    SecondMembre[Ligne] = ValeurDesTermesDeLaMatrice[ic] * ScaleB[Ligne] * ScX;
	    ic++;
    }
	}
	
  /* On met les colonnes du triangle U: le premier terme est le terme diagonal, les termes du triangle U sont
	   stockes dans l'ordre decroissant de Kp */
  icDeb = CdebParColonneDeU[Kp];
	ic = icDeb + NbTermesParColonneDeU[Kp] - 1;
  while ( ic > icDeb ) {
    Ligne = /*OrdreColonne*/OrdreLigne[IndiceLigneParColonneDeU[ic]];
    /* Le pivot vaut 1 */   	
    X = SecondMembre[Ligne];
		/* A ce stade on peut ranger X dans le triangle U */
		SecondMembre[Ligne] = 0.0;
		ElmDeUParColonne[ic] = X; 
	  if ( X != 0.0 ) {
		  /*
		  Kpp = InverseOrdreColonne[Ligne];
      il = CdebParColonneDeL[Kpp];		 
      ilMax = il + NbTermesParColonneDeL[Kpp];
			*/
      il = CdebParColonneDeL[/*InverseOrdreColonne*/InverseOrdreLigne[Ligne]];		 
      ilMax = il + NbTermesParColonneDeL[/*InverseOrdreColonne*/InverseOrdreLigne[Ligne]];
      il++;
      while ( il < ilMax ) {
        SecondMembre[IndiceLigneDeL[il]] -= ElmDeL[il] * X;	
        il++;
      }
		}
    ic--;
  }
  /* En ce qui concerne le triangle L on n'a rien a faire car le resultat se trouve deja dans SecondMembre */		 	 	
	/* On divise la colonne par le pivot */
	ic = CdebParColonneDeL[Kp];
	Ligne = IndiceLigneDeL[ic /*CdebParColonneDeL[Kp]*/ ];
	Pivot = SecondMembre[Ligne];
	SecondMembre[Ligne] = 0.0;
  if ( fabs( Pivot ) < PivotMin ) {
	  printf("Pivot nul dans la refactorisation: Pivot %e PivotMin %e\n",Pivot,PivotMin);
    *ProblemeDansLaRefactorisation = (int) OUI_LU;
		return;
	}	
	Pivot = 1. / Pivot;
  ElmDeUParColonne[icDeb] = Pivot;
 
	/* On met la colonne dans le triangle */
  /* ic = CdebParColonneDeL[Kp]; */
  icMax = ic + NbTermesParColonneDeL[Kp];
  /*ElmDeL[ic] = 1.;*/
	ic++;	
  while ( ic < icMax ) {
	  /*
	  Ligne = IndiceLigneDeL[ic];
    ElmDeL[ic] = SecondMembre[Ligne] * Pivot; 
	  SecondMembre[Ligne] = 0.0;
		*/
    ElmDeL[ic] = SecondMembre[IndiceLigneDeL[ic]] * Pivot;
	  SecondMembre[IndiceLigneDeL[ic]] = 0.0;		
    ic++;
  }	
}

return;
}































