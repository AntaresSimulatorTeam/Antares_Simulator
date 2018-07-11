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

   FONCTION: Resolution du systeme
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

/*--------------------------------------------------------------------------------------------------*/
 
void LU_LuSolv( MATRICE * Matrice,
                double  * SecondMembreEtSolution, /* Le vecteur du second membre et la solution */
                int    * CodeRetour,             /* Le code retour ( 0 si tout s'est bien passe ) */
	            	MATRICE_A_FACTORISER * MatriceEntree, /* Peut etre NULL, dans ce cas on ne fait pas de raffinement */
	              int NombreMaxIterationsDeRaffinement,
		            double ValeurMaxDesResidus        /* En norme L infini i.e. le plus grand */
              )
{
double * SecondMembre; double * Solution  ; double * SecondMembreSV       ; double * SolutionSV  ; int NbResolutions ;
int Rang; int Var  ; int il; int ilMax; double X; int * IdebColEntree; int * NbElmColEntree; int * IndLigEntree;
double * ValElmEntree;

*CodeRetour = 0;

if ( MatriceEntree == NULL ) NombreMaxIterationsDeRaffinement = 0;
if ( NombreMaxIterationsDeRaffinement < 0 ) NombreMaxIterationsDeRaffinement = 0;

if ( NombreMaxIterationsDeRaffinement > NOMBRE_MAX_ITER_DE_RAFFINEMENT  ) {
  NombreMaxIterationsDeRaffinement = NOMBRE_MAX_ITER_DE_RAFFINEMENT;
}

Rang = Matrice->Rang;

if ( NombreMaxIterationsDeRaffinement > 0 ) {
  SecondMembreSV = Matrice->SecondMembreSV;
  if ( Matrice->SolutionSV == NULL ) Matrice->SolutionSV = (double *) malloc( Rang * sizeof( double ) );
  if ( Matrice->SolutionSV != NULL ) {
    SolutionSV = Matrice->SolutionSV;  
    memcpy( (char *) SecondMembreSV , (char *) SecondMembreEtSolution , Rang * sizeof( double ) );    
    memset( (char *) SolutionSV     , 0                               , Rang * sizeof( double ) );
    NbResolutions  = 0;
    /* La matrice d'entree */
    IdebColEntree  = MatriceEntree->IndexDebutDesColonnes;
    NbElmColEntree = MatriceEntree->NbTermesDesColonnes;
    IndLigEntree   = MatriceEntree->IndicesDeLigne;
    ValElmEntree   = MatriceEntree->ValeurDesTermesDeLaMatrice;
	}
	else NombreMaxIterationsDeRaffinement = 0;
}

ResolutionLuSolv:

if ( Matrice->FaireScaling == OUI_LU ) LU_ScalingSecondMembre( Matrice , SecondMembreEtSolution );

/* Resolution de Ly = Pb avec P matrice de permutation des lignes et b 
   le second membre */
SecondMembre = SecondMembreEtSolution;
LU_LuSolvTriangleL( Matrice , SecondMembre );

LU_LuSolveH( Matrice );

/* Resolution de U Q^t x = y avec Q matrice de permutation des colonnes */
Solution = SecondMembreEtSolution;
LU_LuSolvTriangleU( Matrice , Solution );

if ( Matrice->FaireScaling == OUI_LU ) LU_UnScaling( Matrice , SecondMembreEtSolution );

if ( NombreMaxIterationsDeRaffinement > 0 ) {	
  NbResolutions++;  
  if ( NbResolutions <= NombreMaxIterationsDeRaffinement ) {
    for ( Var = 0 ; Var < Rang ; Var++ ) SolutionSV[Var]+= SecondMembreEtSolution[Var];
    memcpy( (char *) SecondMembreEtSolution , (char *) SecondMembreSV , Rang * sizeof( double ) );  
    for ( Var = 0 ; Var < Rang ; Var++ ) {
      X  = SolutionSV[Var];
      il = IdebColEntree[Var];
      ilMax = il + NbElmColEntree[Var];
      while ( il < ilMax ) {
        SecondMembreEtSolution[IndLigEntree[il]]-= ValElmEntree[il] * X;
        il++;
      }
    }
    for ( Var = 0 ; Var < Rang ; Var++ ) {
      if ( fabs( SecondMembreEtSolution[Var] ) > ValeurMaxDesResidus ) {      
        /*
	      printf("LU_LuSolv: iteration de raffinement necessaire ecart %e sur %d\n",fabs( SecondMembreEtSolution[Var]),Var);
				*/
        goto ResolutionLuSolv;
      }
    }
    memcpy( SecondMembreEtSolution , SolutionSV , Rang * sizeof( double ) ); 
  }
  else {
    /*
    printf("LU_LuSolv: le nombre max d'iterations de raffinement ne suffit pas pour atteindre la precision demandee \n");
    */
    *CodeRetour = PRECISION_DE_RESOLUTION_NON_ATTEINTE;    
    for ( Var = 0 ; Var < Rang ; Var++ ) SecondMembreEtSolution[Var]+= SolutionSV[Var];    
  }
}

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_LuSolvTriangleL( MATRICE * Matrice, double * SecondMembre )
{
int Kp; int il; int ilMax; double X; int * IndiceLigneDeL; int * NbTermesParColonneDeL; 
double * SolutionIntermediaire; double * ElmDeL; int * OrdreLigne; 

SolutionIntermediaire = Matrice->SolutionIntermediaire;
IndiceLigneDeL        = Matrice->IndiceLigneDeL;
NbTermesParColonneDeL = Matrice->NbTermesParColonneDeL;
ElmDeL                = Matrice->ElmDeL;

/* Resolution de Ly = Pb avec P matrice de permutation des lignes et b 
   le second membre */
if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE && 0 ) goto LPourSimplexe;
il = 0;
for ( Kp = 0 ; Kp < Matrice->Rang ; Kp++ ) {  
  /* Le pivot vaut 1 */	
  if ( NbTermesParColonneDeL[Kp] == 1 ) {
    SolutionIntermediaire[Kp] =	SecondMembre[IndiceLigneDeL[il]];	
    il++;
		continue;
	}
  X = SecondMembre[IndiceLigneDeL[il]];	
  if ( X != 0.0 ) {
    SolutionIntermediaire[Kp] = X;		
    ilMax = il + NbTermesParColonneDeL[Kp];      
    il++;
    while ( il < ilMax ) {
      SecondMembre[IndiceLigneDeL[il]]-= ElmDeL[il] * X; 
      il++;
    }		
  }
  else {
    il+= NbTermesParColonneDeL[Kp];
  }
}
return;

LPourSimplexe:
OrdreLigne = Matrice->OrdreLigne;
il = 0;
for ( Kp = 0 ; Kp < Matrice->Rang ; Kp++ ) {  
  /* Le pivot vaut 1 */			
  SolutionIntermediaire[Kp] =	SecondMembre[OrdreLigne[Kp]] - SolutionIntermediaire[Kp];	
	if ( NbTermesParColonneDeL[Kp] == 1 ) {
    il++;
		continue;
	}		
  if ( SolutionIntermediaire[Kp] != 0.0 ) {
    X = SolutionIntermediaire[Kp];		
    ilMax = il + NbTermesParColonneDeL[Kp];      
    il++;
    while ( il < ilMax ) {			
      SolutionIntermediaire[IndiceLigneDeL[il]]+= ElmDeL[il] * X; 
      il++;
    }		
  }
  else {
    il+= NbTermesParColonneDeL[Kp];
  }
}
return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_LuSolveH( MATRICE * Matrice )
{
int k ; int il; int ilMax; double S; int Kp; int Rang; int NbTermesNonNulsDuSpike;
double * HValeur; double * SolutionIntermediaire; double * ValeurElmSpike;
int * HDeb; int * HNbTerm; int * HIndiceColonne; int * HLigne; int * IndicesLignesDuSpike;

HDeb                  = Matrice->HDeb;
HNbTerm               = Matrice->HNbTerm;
HValeur               = Matrice->HValeur;
HIndiceColonne        = Matrice->HIndiceColonne;
SolutionIntermediaire = Matrice->SolutionIntermediaire;
HLigne                = Matrice->HLigne;

k = 0;
while ( k < Matrice->NombreDeLuUpdates ) {
  il    = HDeb[k];
  ilMax = il + HNbTerm[k]; 
  S = 0.;
  while ( il < ilMax ) {
    S+= HValeur[il] * SolutionIntermediaire[HIndiceColonne[il]];
    il++;
  }
  /* Colonne egal aussi contrainte pour le terme diagonal */
  SolutionIntermediaire[HLigne[k]]-= S;
  k++;
}

if ( Matrice->SauvegardeDuResultatIntermediaire == OUI_LU ) {
  ValeurElmSpike       = Matrice->ValeurElmSpike;
  IndicesLignesDuSpike = Matrice->IndicesLignesDuSpike;
  Rang                   = Matrice->Rang;	
  NbTermesNonNulsDuSpike = 0;
	
  for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
    /*if ( SolutionIntermediaire[Kp] != 0.0 ) {*/
    if ( fabs(SolutionIntermediaire[Kp]) > ZERO_POUR_SPIKE ) {		
      ValeurElmSpike[NbTermesNonNulsDuSpike] = SolutionIntermediaire[Kp];
      IndicesLignesDuSpike[NbTermesNonNulsDuSpike] =  Kp; 
      NbTermesNonNulsDuSpike++;
    }		
  }    
  Matrice->NbTermesNonNulsDuSpike = NbTermesNonNulsDuSpike;

}
return;
}

/*--------------------------------------------------------------------------------------------------*/
void LU_LuSolvTriangleU( MATRICE * Matrice , double * Solution )
{
int Kp ; double X; double * SolutionIntermediaire;
int * OrdreUcolonne; int KpInitial; int ic; int icMax; int * CdebParColonneDeU;
int * NbTermesParColonneDeU; double * ElmDeUParColonne; int * IndiceLigneParColonneDeU; 
int * OrdreColonne; 
int * LdebParLigneDeU; int * NbTermesParLigneDeU; int * IndiceColonneDeU; double * ElmDeU;
int il; int ilMax; int Inconnue; double Y;

SolutionIntermediaire = Matrice->SolutionIntermediaire;

if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) goto UPourSimplexe;
if ( Matrice->ContexteDeLaFactorisation == LU_POINT_INTERIEUR ) goto UPourPointInterieur;

/* Attention, cette partie de code ne fonctionne pas avec la LU Update */
OrdreColonne             = Matrice->OrdreColonne;
CdebParColonneDeU        = Matrice->CdebParColonneDeU;
NbTermesParColonneDeU    = Matrice->NbTermesParColonneDeU;
ElmDeUParColonne         = Matrice->ElmDeUParColonne;
IndiceLigneParColonneDeU = Matrice->IndiceLigneParColonneDeU;

for ( Kp = Matrice->Rang - 1 ; Kp >= 0 ; Kp-- ) {		
  ic = CdebParColonneDeU[Kp];
  icMax = ic + NbTermesParColonneDeU[Kp];		
  /* Par convention le premier terme est le pivot et il est deja inverse */
  if ( SolutionIntermediaire[Kp] != 0.0 ) {
    X = ElmDeUParColonne[ic] * SolutionIntermediaire[Kp];		
    /* Pendant que la donnee est dans le cache: Raz pour une prochaine utilisation */
    SolutionIntermediaire[Kp] = 0.0;
    /* */		
    ic++;  
    while ( ic < icMax ) {			
      SolutionIntermediaire[IndiceLigneParColonneDeU[ic]]-= ElmDeUParColonne[ic] * X;
      ic++;
    }		
    Solution[OrdreColonne[Kp]] = X;				
  }
  else {
    Solution[OrdreColonne[Kp]] = 0.0;
  }
}
return;

UPourPointInterieur:
LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;
for ( Kp = Matrice->Rang - 1 ; Kp >= 0 ; Kp-- ) {  
  il = LdebParLigneDeU[Kp];
  ilMax = il + NbTermesParLigneDeU[Kp];
  /* Par convention le premier terme est le pivot et il est deja inverse */  
  X = SolutionIntermediaire[Kp];
	/* Pendant que la donnee est dans le cache: Raz pour une prochaine utilisation */
	SolutionIntermediaire[Kp] = 0.0;
	/* */
  Y = ElmDeU[il];  
  Inconnue = IndiceColonneDeU[il];  
  il++;  
  while ( il < ilMax ) {
    X-= ElmDeU[il] * Solution[IndiceColonneDeU[il]];        
    il++;
  }    
  /* Par convention le pivot est deja inverse */
  Solution[Inconnue] = X * Y; 
}
return;

UPourSimplexe:
/* On utilise des matrices de permutations */
OrdreColonne             = Matrice->OrdreColonne;
OrdreUcolonne            = Matrice->OrdreUcolonne;
CdebParColonneDeU        = Matrice->CdebParColonneDeU;
NbTermesParColonneDeU    = Matrice->NbTermesParColonneDeU;
ElmDeUParColonne         = Matrice->ElmDeUParColonne;
IndiceLigneParColonneDeU = Matrice->IndiceLigneParColonneDeU;

for ( Kp = Matrice->Rang - 1 ; Kp >= 0 ; Kp-- ) {	
  KpInitial = OrdreUcolonne[Kp];
	if ( SolutionIntermediaire[KpInitial] == 0.0 ) {
    Solution[OrdreColonne[KpInitial]] = 0.0;		
    continue;	
	}
  if ( NbTermesParColonneDeU[KpInitial] == 1 ) {
    X = ElmDeUParColonne[CdebParColonneDeU[KpInitial]] * SolutionIntermediaire[KpInitial];		
    if ( fabs( X ) > ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION ) Solution[OrdreColonne[KpInitial]] = X;
    else Solution[OrdreColonne[KpInitial]] = 0.0;				
    /* Pendant que la donnee a toutes les chances d'etre dans le cache: Raz pour une prochaine utilisation */
    SolutionIntermediaire[KpInitial] = 0.0;
    /* */				
    continue;
  }
  ic = CdebParColonneDeU[KpInitial];
  icMax = ic + NbTermesParColonneDeU[KpInitial];		
  /* Par convention le premier terme est le pivot et il est deja inverse */
  if ( SolutionIntermediaire[KpInitial] != 0.0 ) {
    X = ElmDeUParColonne[ic] * SolutionIntermediaire[KpInitial];		
    /* Pendant que la donnee est dans le cache: Raz pour une prochaine utilisation */
    SolutionIntermediaire[KpInitial] = 0.0;
    /* */		
    ic++;  
    while ( ic < icMax ) {			
      SolutionIntermediaire[IndiceLigneParColonneDeU[ic]]-= ElmDeUParColonne[ic] * X;
      ic++;
    }		
    if ( fabs( X ) < ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION ) Solution[OrdreColonne[KpInitial]] = 0.0;
    else Solution[OrdreColonne[KpInitial]] = X;				
  }
  else {
    Solution[OrdreColonne[KpInitial]] = 0.0;		
  }
}

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_LuSolvTransposee(
                          MATRICE * Matrice,
                          double  * SecondMembreEtSolution, /* Le vecteur du second membre et la solution */
                          int    * CodeRetour,             /* Le code retour ( 0 si tout s'est bien passe ) */
	      	                MATRICE_A_FACTORISER * MatriceEntree, /* Peut etre NULL, dans ce cas on ne fait pas de raffinement */
		                      int NombreMaxIterationsDeRaffinement,
			                    double ValeurMaxDesResidus	    /* En norme L infini i.e. le plus grand */	  
                        )
{
double * SecondMembre; double * Solution  ; double * SecondMembreSV       ; double * SolutionSV  ; int NbResolutions ;
int Rang; int Var  ; int il; int ilMax; double X; int * IdebColEntree; int * NbElmColEntree; int * IndLigEntree;
double * ValElmEntree; int i ;

*CodeRetour = 0;

if ( MatriceEntree == NULL ) NombreMaxIterationsDeRaffinement = 0;
if ( NombreMaxIterationsDeRaffinement < 0 ) NombreMaxIterationsDeRaffinement = 0;
if ( NombreMaxIterationsDeRaffinement > NOMBRE_MAX_ITER_DE_RAFFINEMENT  ) {
  NombreMaxIterationsDeRaffinement = NOMBRE_MAX_ITER_DE_RAFFINEMENT;
}

Rang = Matrice->Rang;
	
if ( NombreMaxIterationsDeRaffinement > 0 ) {
  SecondMembreSV = Matrice->SecondMembreSV;
  if ( Matrice->SolutionSV == NULL ) Matrice->SolutionSV = (double *) malloc( Rang * sizeof( double ) );
  if ( Matrice->SolutionSV != NULL ) {	
    SolutionSV = Matrice->SolutionSV;
    memcpy( (char *) SecondMembreSV , (char *) SecondMembreEtSolution , Rang * sizeof( double ) );
    memset( (char *) SolutionSV     , 0                               , Rang * sizeof( double ) );
    NbResolutions  = 0;
    /* La matrice d'entree */
    IdebColEntree  = MatriceEntree->IndexDebutDesColonnes;
    NbElmColEntree = MatriceEntree->NbTermesDesColonnes;
    IndLigEntree   = MatriceEntree->IndicesDeLigne;
    ValElmEntree   = MatriceEntree->ValeurDesTermesDeLaMatrice;
	}
	else NombreMaxIterationsDeRaffinement = 0;
}

ResolutionLuSolvTransposee:

if ( Matrice->FaireScaling == OUI_LU ) LU_ScalingSecondMembreTranspose( Matrice , SecondMembreEtSolution );

/* Resolution de U^t y = Q^t b avec Q matrice de permutation des colonnes */
SecondMembre = SecondMembreEtSolution;
LU_LuSolvTriangleUTransposee( Matrice , SecondMembre );

LU_LuSolveHTransposee( Matrice );

/* Resolution de L^t x = y */
Solution = SecondMembreEtSolution;
LU_LuSolvTriangleLTransposee( Matrice , Solution );

if ( Matrice->FaireScaling == OUI_LU ) LU_UnScalingTranspose( Matrice , SecondMembreEtSolution );

if ( NombreMaxIterationsDeRaffinement > 0 ) {
  NbResolutions++;  
  if ( NbResolutions <= NombreMaxIterationsDeRaffinement ) {
    for ( Var = 0 ; Var < Rang ; Var++ ) SolutionSV[Var]+= SecondMembreEtSolution[Var];
    memcpy( (char *) SecondMembreEtSolution , (char *) SecondMembreSV , Rang * sizeof( double ) );  
    for ( i = 0 ; i < Rang ; i++ ) {
      X = SecondMembreEtSolution[i];      
      il = IdebColEntree[i];
      ilMax = il + NbElmColEntree[i];
      while ( il < ilMax ) {
        /* Dans ce cas, IndLigEntree[il] va nous donner le numero de la variable */
	      X-= ValElmEntree[il] * SolutionSV[IndLigEntree[il]];
        il++;
      }
      SecondMembreEtSolution[i] = X;
    }		
    for ( i = 0 ; i < Rang ; i++ ) {
      if ( fabs( SecondMembreEtSolution[i] ) > ValeurMaxDesResidus ) {
			  /*
	      printf("LU_LuSolvTransposee: iteration de raffinement necessaire ecart %e sur %d\n",fabs( SecondMembreEtSolution[i]),i);
        */
        goto ResolutionLuSolvTransposee;
      }
    }		
    memcpy( SecondMembreEtSolution , SolutionSV , Rang * sizeof( double ) ); 
  }
  else {
    /*
    printf("LU_LuSolvTransposee: le nombre max d'iterations de raffinement ne suffit pas pour atteindre la precision demandee \n");
    */
    *CodeRetour = PRECISION_DE_RESOLUTION_NON_ATTEINTE;    
    for ( Var = 0 ; Var < Rang ; Var++ ) SecondMembreEtSolution[Var]+= SolutionSV[Var];
  }
}

return;
}
/*--------------------------------------------------------------------------------------------------*/
void LU_LuSolvTriangleUTransposee( MATRICE * Matrice , double * SecondMembre )
{
int Kp; int il; int ilMax; double X; double Y; int Rang; int KpInitial; int * LdebParLigneDeU;
int * IndiceColonneDeU; int * NbTermesParLigneDeU; double * SolutionIntermediaire;
double * ElmDeU; int * OrdreUcolonne;
int * CdebParColonneDeU; int * NbTermesParColonneDeU; int * IndiceLigneParColonneDeU; double * ElmDeUParColonne;
int * OrdreColonne;

SolutionIntermediaire = Matrice->SolutionIntermediaire;
Rang                  = Matrice->Rang;

/* Resolution de U^t y = Q^t b avec Q matrice de permutation des colonnes */
if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) goto UTPourSimplexe;
if ( Matrice->ContexteDeLaFactorisation == LU_POINT_INTERIEUR ) goto UTPourPointInterieur;

/* Hors simplexe on utilise encore le chainage par colonne car c'est le seul
   qui est maintenu dans le cas de la refactorisation */
CdebParColonneDeU        = Matrice->CdebParColonneDeU;
NbTermesParColonneDeU    = Matrice->NbTermesParColonneDeU;
ElmDeUParColonne         = Matrice->ElmDeUParColonne;
IndiceLigneParColonneDeU = Matrice->IndiceLigneParColonneDeU;
OrdreColonne             = Matrice->OrdreColonne;
	 
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  il = CdebParColonneDeU[Kp];
  ilMax = il + NbTermesParColonneDeU[Kp];
  /* Par convention le premier terme est le pivot et il est deja inverse */  
  X = SecondMembre[OrdreColonne[Kp]];
	/* */
  Y = ElmDeUParColonne[il];  
  il++;  
  while ( il < ilMax ) {
	  /* IndiceLigneParColonneDeU est donne dans la numerotation Kp */
    X-= ElmDeUParColonne[il] * SolutionIntermediaire[IndiceLigneParColonneDeU[il]];        
    il++;
  }    
  /* Par convention le pivot est deja inverse */
  SolutionIntermediaire[Kp] = X * Y; 
}
return;

UTPourPointInterieur:
LdebParLigneDeU       = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU   = Matrice->NbTermesParLigneDeU;
ElmDeU                = Matrice->ElmDeU;
IndiceColonneDeU      = Matrice->IndiceColonneDeU;

for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  il = LdebParLigneDeU[Kp];				
  /* Par convention le premier terme est le pivot et est deja inverse */
  X = SecondMembre[IndiceColonneDeU[il]];
  /* On fait le test sur SecondMembre car si SecondMembre est nul alors X est nul */
  if ( X != 0.0 ) {
    X = X * ElmDeU[il];  
    SolutionIntermediaire[Kp] = X;      
    ilMax = il + NbTermesParLigneDeU[Kp];
    il++;     
    while ( il < ilMax ) {
      SecondMembre[IndiceColonneDeU[il]]-= ElmDeU[il] * X;
      il++;
    }
  }
}
return;

UTPourSimplexe:

LdebParLigneDeU       = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU   = Matrice->NbTermesParLigneDeU;
ElmDeU                = Matrice->ElmDeU;
IndiceColonneDeU      = Matrice->IndiceColonneDeU;
OrdreUcolonne = Matrice->OrdreUcolonne;

for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  KpInitial = OrdreUcolonne[Kp];	
  il = LdebParLigneDeU[KpInitial];				
  /* Par convention le premier terme est le pivot et est deja inverse */
  X = SecondMembre[IndiceColonneDeU[il]];
  /* On fait le test sur SecondMembre car si SecondMembre est nul alors X est nul */
  if ( X != 0.0 ) {
    X = X * ElmDeU[il];  
    SolutionIntermediaire[KpInitial] = X;      
    ilMax = il + NbTermesParLigneDeU[KpInitial];
    il++;     
    while ( il < ilMax ) {
      SecondMembre[IndiceColonneDeU[il]]-= ElmDeU[il] * X;
      il++;
    }
  }
}
return;

}
/*--------------------------------------------------------------------------------------------------*/
void LU_LuSolveHTransposee( MATRICE * Matrice )
{
int k; int il; int ilMax; double S; int * HDeb; int * HNbTerm; int * HLigne;
int * HIndiceColonne; double * SolutionIntermediaire; double * HValeur;

HDeb                  = Matrice->HDeb;
HNbTerm               = Matrice->HNbTerm;
SolutionIntermediaire = Matrice->SolutionIntermediaire;
HLigne                = Matrice->HLigne;
HIndiceColonne        = Matrice->HIndiceColonne;
HValeur               = Matrice->HValeur;

k = Matrice->NombreDeLuUpdates - 1;
while ( k >= 0 ) {
  il    = HDeb[k];
  ilMax = il + HNbTerm[k];
  S = SolutionIntermediaire[HLigne[k]];
  if ( S != 0.0 ) {
    while ( il < ilMax ) {
      SolutionIntermediaire[HIndiceColonne[il]]-= HValeur[il] * S;
      il++;
    }
  }
  k--;
}
return;
}
/*--------------------------------------------------------------------------------------------------*/
void LU_LuSolvTriangleLTransposee( MATRICE * Matrice , double * Solution )
{
int Kp; int il; int ilMax; double X; int Inconnue; int * CdebParColonneDeL; int * NbTermesParColonneDeL;
double * ElmDeL; int * IndiceLigneDeL; double * SolutionIntermediaire; int * LdebParLigneDeL;
int * NbTermesParLigneDeL; int * IndiceColonneParLigneDeL; double * ElmDeLParLigne; int * OrdreLigne;
char AllocOk;

SolutionIntermediaire = Matrice->SolutionIntermediaire;

CdebParColonneDeL     = Matrice->CdebParColonneDeL;
NbTermesParColonneDeL = Matrice->NbTermesParColonneDeL;
ElmDeL                = Matrice->ElmDeL;
IndiceLigneDeL        = Matrice->IndiceLigneDeL;

/* Resolution de L^t x = y */
if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) goto LTPourSimplexe;

LTAutresContextes:
for ( Kp = Matrice->Rang - 1 ; Kp >= 0 ; Kp-- ) {
  X = SolutionIntermediaire[Kp];
	/* RAZ pendant que la donnee est dans le cache */
  SolutionIntermediaire[Kp] = 0.0;
	/* */
  il    = CdebParColonneDeL[Kp];
  ilMax = il + NbTermesParColonneDeL[Kp]; 
  Inconnue = IndiceLigneDeL[il];
  il++;
  while ( il < ilMax ) {
    X-= ElmDeL[il] * Solution[IndiceLigneDeL[il]]; 
    il++;
  }
  Solution[Inconnue] = X;
}
return;

LTPourSimplexe:

if ( Matrice->LdebParLigneDeL == NULL ) {
  /* Si le stockage par ligne de L n'existe pas on le cree */
  LU_ChainageParLigneDeL( Matrice, &AllocOk );  
	if ( AllocOk != OUI_LU ) goto LTAutresContextes;	
}

OrdreLigne               = Matrice->OrdreLigne;
LdebParLigneDeL          = Matrice->LdebParLigneDeL;
NbTermesParLigneDeL      = Matrice->NbTermesParLigneDeL;
IndiceColonneParLigneDeL = Matrice->IndiceColonneParLigneDeL;
ElmDeLParLigne           = Matrice->ElmDeLParLigne;

for ( Kp = Matrice->Rang - 1 ; Kp >= 0 ; Kp-- ) {
  X = SolutionIntermediaire[Kp];	
  if ( X != 0 ) {
	  /* RAZ pendant que la donnee est dans le cache */
    SolutionIntermediaire[Kp] = 0.0;
	  /*  */
		if ( NbTermesParLigneDeL[Kp] == 1 ) {
      if ( fabs( X ) < ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION_TRANSPOSEE ) Solution[OrdreLigne[Kp]] = 0.0;			
      else Solution[OrdreLigne[Kp]] = X;			
			continue;
		}
	  /* */
    il    = LdebParLigneDeL[Kp];
    ilMax = il + NbTermesParLigneDeL[Kp];	
    il++;
    while ( il < ilMax ) {			
      SolutionIntermediaire[IndiceColonneParLigneDeL[il]]-= ElmDeLParLigne[il] * X;			
      il++;
    }				
    if ( fabs( X ) < ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION_TRANSPOSEE ) Solution[OrdreLigne[Kp]] = 0.0;		
		else Solution[OrdreLigne[Kp]] = X;		
	}
	else Solution[OrdreLigne[Kp]] = 0.0;			
}

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_LuSolvLuUpdate( MATRICE * Matrice, double * SecondMembreEtSolution, int * CodeRetour,
                        char Save, char SecondMembreCreux, MATRICE_A_FACTORISER * MatriceEntree, 
		        int NombreMaxIterationsDeRaffinement, double ValeurMaxDesResidus        
                      )			
{

Matrice->SauvegardeDuResultatIntermediaire = Save;
Matrice->SecondMembreCreux                 = SecondMembreCreux;

LU_LuSolv( Matrice, SecondMembreEtSolution, CodeRetour, MatriceEntree, NombreMaxIterationsDeRaffinement,
           ValeurMaxDesResidus );
						 
Matrice->SauvegardeDuResultatIntermediaire = NON_LU;
Matrice->SecondMembreCreux = NON_LU;  
return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_LuSolvTransposeeLuUpdate( MATRICE * Matrice, double * SecondMembreEtSolution, int * CodeRetour ,
                                  char SecondMembreCreux, MATRICE_A_FACTORISER * MatriceEntree, 
		                  int NombreMaxIterationsDeRaffinement, double ValeurMaxDesResidus        
                                )
{

Matrice->SecondMembreCreux = SecondMembreCreux;

LU_LuSolvTransposee( Matrice, SecondMembreEtSolution, CodeRetour, MatriceEntree, NombreMaxIterationsDeRaffinement,
                     ValeurMaxDesResidus );
											 
Matrice->SecondMembreCreux = NON_LU;  
return;
}
		      








