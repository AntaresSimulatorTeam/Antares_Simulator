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

   FONCTION: Resolution du systeme. Cas hyper creux.
 
   AUTEUR: R. GONZALEZ

************************************************************************/
   
# include "lu_define.h"
# include "lu_fonctions.h"

# define SEUIL_RECURSION    0.75/*0.8*/
# define SEUIL_RECURSION_T  0.75/*0.8*/

/*--------------------------------------------------------------------------------------------------*/

void LU_AllocEtInitHyperCreux( MATRICE * Matrice, char * AllocOk )
{
int i; int Rang; char * NoeudDansLaliste;
*AllocOk = OUI_LU;
Rang    = Matrice->Rang;
Matrice->RatioMoyen = 0.0;
Matrice->NombreDeRatios = 0;
Matrice->RatioMoyenTransposee = 0.0;
Matrice->NombreDeRatiosTransposee = 0;	
Matrice->TailleAlloueeDePseudoPile = Rang;
Matrice->PseudoPile     = Matrice->Ldeb;
Matrice->ListeDesNoeuds = Matrice->LNbTerm;
Matrice->NoeudDansLaliste = (char *) Matrice->LDernierPossible;
/*
Matrice->NoeudDansLaliste = (char *) malloc( Rang * sizeof( char ) ); 
if ( Matrice->NoeudDansLaliste == NULL ) {
  *AllocOk = NON_LU;
	return;
}
*/
NoeudDansLaliste = Matrice->NoeudDansLaliste;
for ( i = 0 ; i < Rang ; i++ ) NoeudDansLaliste[i] = NOEUD_HORS_LISTE;
return;
}
/*--------------------------------------------------------------------------------------------------*/

char LU_RecusionSurTriangle_1( MATRICE * Matrice, int * IndexTermesNonNuls, int NbTermesNonNuls,
                               int * InverseOrdre, int * CdebParColonne, int * IndiceLigne,
			       int * NbTermesParColonne, int * NombreDeNoeudsDansLaListe )
{
int i; int Kp; int IndexDePile; int NbNoeudsDansLaListe; int il; int ilMax; int Col;
char * NoeudDansLaliste; int * ListeDesNoeuds; int * PseudoPile; int IndexMaxDePile;

NbNoeudsDansLaListe = 0;
NoeudDansLaliste = Matrice->NoeudDansLaliste;
ListeDesNoeuds   = Matrice->ListeDesNoeuds;
PseudoPile       = Matrice->PseudoPile;

IndexMaxDePile = Matrice->TailleAlloueeDePseudoPile - 3;

for ( i = 0 ; i < NbTermesNonNuls ; i++ ) {
  Col = IndexTermesNonNuls[i];	
  if ( NoeudDansLaliste[Col] != NOEUD_HORS_LISTE ) continue;	
	
	/* Recursion avec simulation de pile */
  IndexDePile = 0;
	Kp = InverseOrdre[Col];
  NouveauNoeud:
	if ( NbTermesParColonne[Kp] == 1 ) goto InclureLeNoeudDansLaListe;
  il = CdebParColonne[Kp] + 1;
  ilMax = il + NbTermesParColonne[Kp] - 1;
  ParcoursNoeud:
  while ( il < ilMax ) {
    if ( NoeudDansLaliste[IndiceLigne[il]] == NOEUD_HORS_LISTE ) {

	  	if ( IndexDePile > IndexMaxDePile ) {
			  # if VERBOSE_LU
			    printf("Taille de la pseudo pile depasse\n");
				# endif
        *NombreDeNoeudsDansLaListe = NbNoeudsDansLaListe;
        return( RECURSION_NOT_OK );
			}

			if ( il+1 < ilMax ) {
		    PseudoPile[IndexDePile] = ilMax;
		    IndexDePile++;			
		    PseudoPile[IndexDePile] = il+1;
		    IndexDePile++;				
			}
			else {
		    IndexDePile++;			
		    PseudoPile[IndexDePile] = -1;
		    IndexDePile++;			
			}
	    PseudoPile[IndexDePile] = Col;
		  IndexDePile++;
			Col = IndiceLigne[il];
		  Kp  = InverseOrdre[Col];			
      goto NouveauNoeud;
	  }   
	  il++;
  }
  /* On a explore tous les fils de Kp => On remonte */
  /* Au lieu de placer le noeud au sommet de la liste, on le place a la fin.
     Il faudra donc lire la liste a l'envers */		 
  InclureLeNoeudDansLaListe:
  ListeDesNoeuds[NbNoeudsDansLaListe] = Col;	
  NoeudDansLaliste[Col] = NOEUD_DANS_LISTE;
  NbNoeudsDansLaListe++;
	
  /* On remonte dans la pile */	
  IndexDePile--;
  if ( IndexDePile >= 2 ) {
    Col	  = PseudoPile[IndexDePile];
    IndexDePile--;		
    il = PseudoPile[IndexDePile];
    IndexDePile--;
		if ( il < 0 ) goto InclureLeNoeudDansLaListe;
    ilMax	= PseudoPile[IndexDePile];		
    goto ParcoursNoeud;
  }
}

*NombreDeNoeudsDansLaListe = NbNoeudsDansLaListe;  

return( RECURSION_OK );
}

/*--------------------------------------------------------------------------------------------------*/

char LU_RecusionSurTriangle_2( MATRICE * Matrice, int * IndexTermesNonNuls, int NbTermesNonNuls,
                               int * CdebParColonne, int * IndiceLigne, int * NbTermesParColonne,
			                         int * NombreDeNoeudsDansLaListe )
{
int i; int Kp; int IndexDePile; int NbNoeudsDansLaListe; int il; int ilMax; char * NoeudDansLaliste;
int * ListeDesNoeuds; int * PseudoPile; int IndexMaxDePile;

NbNoeudsDansLaListe = 0;
NoeudDansLaliste = Matrice->NoeudDansLaliste;
ListeDesNoeuds   = Matrice->ListeDesNoeuds;
PseudoPile       = Matrice->PseudoPile;

IndexMaxDePile = Matrice->TailleAlloueeDePseudoPile - 3;

for ( i = 0 ; i < NbTermesNonNuls ; i++ ) {	
  if ( NoeudDansLaliste[IndexTermesNonNuls[i]] != NOEUD_HORS_LISTE ) continue;
	Kp = IndexTermesNonNuls[i];
	
	/* Recursion avec simulation de pile */
  IndexDePile = 0;
  NouveauNoeud:
	if ( NbTermesParColonne[Kp] == 1 ) goto InclureLeNoeudDansLaListe;
  il = CdebParColonne[Kp] + 1;
  ilMax = il + NbTermesParColonne[Kp] - 1;
  ParcoursNoeud:
  while ( il < ilMax ) {
    if ( NoeudDansLaliste[IndiceLigne[il]] == NOEUD_HORS_LISTE ) {
      
	  	if ( IndexDePile > IndexMaxDePile ) {
			  # if VERBOSE_LU
			    printf("Taille de la pseudo pile depasse\n");
				# endif				
        *NombreDeNoeudsDansLaListe = NbNoeudsDansLaListe;
        return( RECURSION_NOT_OK );				
			}
		  
			if ( il+1 < ilMax ) {
		    PseudoPile[IndexDePile] = ilMax;
		    IndexDePile++;			
		    PseudoPile[IndexDePile] = il+1;
		    IndexDePile++;				
			}
			else {
		    IndexDePile++;			
		    PseudoPile[IndexDePile] = -1;
		    IndexDePile++;			
			}
	    PseudoPile[IndexDePile] = Kp;
		  IndexDePile++;
		  Kp = IndiceLigne[il];		
		  goto NouveauNoeud;
	  }   
	  il++;
  }
  /* On a explore tous les fils de Kp => On remonte */
  /* Au lieu de placer le noeud au sommet de la liste, on le place a la fin.
     Il faudra donc lire la liste a l'envers */
	InclureLeNoeudDansLaListe:
  ListeDesNoeuds[NbNoeudsDansLaListe] = Kp;
  NoeudDansLaliste[Kp] = NOEUD_DANS_LISTE;
  NbNoeudsDansLaListe++;
			
  /* On remonte dans la pile */
  IndexDePile--; 
  if ( IndexDePile >= 2 ) {
    Kp = PseudoPile[IndexDePile];	
    IndexDePile--;		
    il = PseudoPile[IndexDePile];
    IndexDePile--;		
		if ( il < 0 ) goto InclureLeNoeudDansLaListe;
    ilMax	= PseudoPile[IndexDePile];
    goto ParcoursNoeud;
  }			
}

*NombreDeNoeudsDansLaListe = NbNoeudsDansLaListe;

return( RECURSION_OK );
}

/*--------------------------------------------------------------------------------------------------*/ 
	 
char LU_LuSolvTriangleLSecondMembreHyperCreux( MATRICE * Matrice, int * NbTermesNonNuls,
                                               int * IndexTermesNonNuls, double * SecondMembre )
{
int Kp; int il  ; int ilMax; double X ; int j; int i; int NombreDeNoeudsDansLaListe;
int Ligne; double * SolutionIntermediaire; double * ElmDeL; int * OrdreLigne;
char * NoeudDansLaliste; int * InverseOrdreLigne; int * CdebParColonneDeL; char CodeRet;
int * NbTermesParColonneDeL; int * IndiceLigneDeL; int * ListeDesNoeuds;

NombreDeNoeudsDansLaListe = 0;

InverseOrdreLigne = Matrice->InverseOrdreLigne;
CdebParColonneDeL = Matrice->CdebParColonneDeL;
IndiceLigneDeL    = Matrice->IndiceLigneDeL;
NbTermesParColonneDeL = Matrice->NbTermesParColonneDeL;

/* Recusion sur le graphe */
CodeRet = LU_RecusionSurTriangle_1 ( Matrice, IndexTermesNonNuls, *NbTermesNonNuls, InverseOrdreLigne,
                                     CdebParColonneDeL, IndiceLigneDeL, NbTermesParColonneDeL,
									                   &NombreDeNoeudsDansLaListe );

ListeDesNoeuds   = Matrice->ListeDesNoeuds;
NoeudDansLaliste = Matrice->NoeudDansLaliste;

if ( CodeRet == RECURSION_NOT_OK ) {
  for ( i = 0 ; i < NombreDeNoeudsDansLaListe; i++ ) NoeudDansLaliste[ListeDesNoeuds[i]] = NOEUD_HORS_LISTE;
	return( CodeRet );
}
 
SolutionIntermediaire = Matrice->SolutionIntermediaire;
ElmDeL                = Matrice->ElmDeL;
OrdreLigne            = Matrice->OrdreLigne;

/* Resolution  */
j = 0;
for ( i = NombreDeNoeudsDansLaListe - 1 ; i >= 0 ; i-- ) {
  Ligne = ListeDesNoeuds[i];
	NoeudDansLaliste[Ligne] = NOEUD_HORS_LISTE;

  /* Le pivot vaut 1 */    	
  X = SecondMembre[Ligne];	
	if ( X == 0.0 ) continue;
	SecondMembre[Ligne] = 0.0;
	
	Kp = InverseOrdreLigne[Ligne];	
  SolutionIntermediaire[Kp] = X;	
	IndexTermesNonNuls[j] = Kp;
	j++;
		
  il = CdebParColonneDeL[Kp];		 
  ilMax = il + NbTermesParColonneDeL[Kp];      
  il++;
  while ( il < ilMax ) {
    SecondMembre[IndiceLigneDeL[il]] -= ElmDeL[il] * X;	
    il++;
  }					         
}

/* Prediction du nombre de termes */
if ( j * Matrice->RatioMoyen > ceil( SEUIL_RECURSION * Matrice->Rang ) ) {					
  for ( i = 0 ; i < NombreDeNoeudsDansLaListe; i++ ) NoeudDansLaliste[ListeDesNoeuds[i]] = NOEUD_HORS_LISTE;
	return( ARRET_RESOLUTION_HYPER_CREUX );
}

Matrice->NbTermesAuDepart = NombreDeNoeudsDansLaListe;

/* En sortie on recupere SolutionIntermediaire sous forme creux et IndexTermesNonNuls qui donne
   les index de SolutionIntermediaire non nulles */
for ( i = 0 ; i < j ; i++ ) NoeudDansLaliste[IndexTermesNonNuls[i]] = NOEUD_DANS_LISTE;
*NbTermesNonNuls = j;

return( POURSUITE_RESOLUTION_HYPER_CREUX );
}
/*--------------------------------------------------------------------------------------------------*/ 
void LU_LuSolveHSecondMembreHyperCreux( MATRICE * Matrice, int * NbTermesNonNuls,
                                        int * IndexTermesNonNuls )
{
int k ; int il; int ilMax; double S; int Kp; int Rang; int NbTermesNonNulsDuSpike;
double * HValeur; double * SolutionIntermediaire; double * ValeurElmSpike;
int * HDeb; int * HNbTerm; int * HIndiceColonne; int * HLigne; int * IndicesLignesDuSpike;
int i; char Flag; char * NoeudDansLaliste; 

SolutionIntermediaire = Matrice->SolutionIntermediaire;

if ( Matrice->NombreDeLuUpdates <= 0 ) goto SVspike;

NoeudDansLaliste = Matrice->NoeudDansLaliste;

HDeb           = Matrice->HDeb;
HNbTerm        = Matrice->HNbTerm;
HValeur        = Matrice->HValeur;
HIndiceColonne = Matrice->HIndiceColonne;
HLigne         = Matrice->HLigne;

i = *NbTermesNonNuls;
for ( k = 0 ; k < Matrice->NombreDeLuUpdates ; k++ ) {
  il    = HDeb[k];
  ilMax = il + HNbTerm[k];
	Flag = 0;
  S = 0;
  while ( il < ilMax ) {	
	  if ( NoeudDansLaliste[HIndiceColonne[il]] != NOEUD_HORS_LISTE ) {
      S+= HValeur[il] * SolutionIntermediaire[HIndiceColonne[il]];
			Flag = 1;
		}
    il++;
  }
  /* Colonne egal aussi contrainte pour le terme diagonal */
	if ( S != 0.0  ) {
	  if ( Flag == 1 ) {
	    Kp = HLigne[k];
      SolutionIntermediaire[Kp] -= S;						
		  if ( NoeudDansLaliste[Kp] == NOEUD_HORS_LISTE ) {
			  NoeudDansLaliste[Kp] = NOEUD_DANS_LISTE;
		    IndexTermesNonNuls[i] = Kp;
		    i++;
		  }
		}
	}
}

*NbTermesNonNuls = i;

SVspike:

if ( Matrice->SauvegardeDuResultatIntermediaire == OUI_LU ) {

  ValeurElmSpike       = Matrice->ValeurElmSpike;
  IndicesLignesDuSpike = Matrice->IndicesLignesDuSpike;
  Rang                 = Matrice->Rang;	
  NbTermesNonNulsDuSpike = 0;

  for ( i = 0 ; i < *NbTermesNonNuls ; i++ ) {
	  Kp = IndexTermesNonNuls[i];	
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
char LU_LuSolvTriangleUSecondMembreHyperCreux( MATRICE * Matrice, double * Solution,
                                               int * NbTermesNonNuls, int * IndexTermesNonNuls,
																							 char TypeDeSortie ) 
{ 
double X; double * SolutionIntermediaire; int KpInitial; int ic; int icMax; 
int * CdebParColonneDeU; int * NbTermesParColonneDeU; double * ElmDeUParColonne;
int * IndiceLigneParColonneDeU; int * OrdreColonne; int i; int j; int NombreDeNoeudsDansLaListe; 
char * NoeudDansLaliste; int * ListeDesNoeuds; char CodeRet; 

/* Recusion sur le graphe */
NombreDeNoeudsDansLaListe = 0;
NoeudDansLaliste = Matrice->NoeudDansLaliste;   
for ( i = 0 ; i < *NbTermesNonNuls ; i++ ) NoeudDansLaliste[IndexTermesNonNuls[i]] = NOEUD_HORS_LISTE;
 
CdebParColonneDeU        = Matrice->CdebParColonneDeU;
IndiceLigneParColonneDeU = Matrice->IndiceLigneParColonneDeU;
NbTermesParColonneDeU    = Matrice->NbTermesParColonneDeU;

CodeRet = LU_RecusionSurTriangle_2( Matrice, IndexTermesNonNuls, *NbTermesNonNuls, CdebParColonneDeU,
                                    IndiceLigneParColonneDeU, NbTermesParColonneDeU, &NombreDeNoeudsDansLaListe );
																													 
ListeDesNoeuds = Matrice->ListeDesNoeuds;
SolutionIntermediaire = Matrice->SolutionIntermediaire;

if ( CodeRet == RECURSION_NOT_OK ) {
  for ( i = 0 ; i < NombreDeNoeudsDansLaListe ; i++ ) NoeudDansLaliste[ListeDesNoeuds[i]] = NOEUD_HORS_LISTE;
	return( RECURSION_NOT_OK ); 		   
}

OrdreColonne          = Matrice->OrdreColonne;
ElmDeUParColonne      = Matrice->ElmDeUParColonne;

X = Matrice->NombreDeRatios * Matrice->RatioMoyen;
X+= (double) NombreDeNoeudsDansLaListe / (double) Matrice->NbTermesAuDepart;
Matrice->NombreDeRatios++;
Matrice->RatioMoyen = X / Matrice->NombreDeRatios;

j = 0;
for ( i = NombreDeNoeudsDansLaListe - 1 ; i >= 0 ; i-- ) {

  KpInitial = ListeDesNoeuds[i];	
  NoeudDansLaliste[KpInitial] = NOEUD_HORS_LISTE;
  if ( SolutionIntermediaire[KpInitial] == 0.0 ) continue;	
  ic = CdebParColonneDeU[KpInitial];
	
  /* Par convention le premier terme est le pivot et il est deja inverse */
  X = ElmDeUParColonne[ic] * SolutionIntermediaire[KpInitial];	
  SolutionIntermediaire[KpInitial] = 0.0;

	/*if ( X == 0.0 ) continue;*/
	
  /* */			
  icMax = ic + NbTermesParColonneDeU[KpInitial];
  ic++;  
  while ( ic < icMax ) {
    SolutionIntermediaire[IndiceLigneParColonneDeU[ic]]-= ElmDeUParColonne[ic] * X;
    ic++;
  } 

	if ( fabs( X ) < ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION ) continue;
	
	if ( TypeDeSortie == COMPACT_LU ) Solution[j] = X;
	else                              Solution[OrdreColonne[KpInitial]] = X;
  IndexTermesNonNuls[j] = OrdreColonne[KpInitial];
  j++;	
}
*NbTermesNonNuls = j;

/*printf("Solution : NbTermesNonNuls %d\n",j);*/

X = Matrice->NombreDeRatios * Matrice->RatioMoyen;
X+= (double) j / (double) Matrice->NbTermesAuDepart;
Matrice->NombreDeRatios++;
Matrice->RatioMoyen = X / Matrice->NombreDeRatios;

return( RECURSION_OK ); 
}

/*--------------------------------------------------------------------------------------------------*/
/* SecondMembre est toujours sous forme expand */

char LU_LuSolvTriangleUTransposeeSecondMembreHyperCreux( MATRICE * Matrice, int * NbTermesNonNuls,
                                                         int * IndexTermesNonNuls, double * SecondMembre )
{
int NombreDeNoeudsDansLaListe; int i; int il; int ilMax; double X; int Col; int j; int KpInitial; 
int * ListeDesNoeuds; int * NbTermesParLigneDeU; double * ElmDeU; int * LdebParLigneDeU;
int * IndiceColonneDeU; double * SolutionIntermediaire;  char * NoeudDansLaliste; int * InverseOrdreColonne;
int * OrdreColonne; char CodeRet;

/* Recusion sur le graphe */
NombreDeNoeudsDansLaListe = 0;

InverseOrdreColonne   = Matrice->InverseOrdreColonne;
LdebParLigneDeU     = Matrice->LdebParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;

CodeRet = LU_RecusionSurTriangle_1( Matrice, IndexTermesNonNuls, *NbTermesNonNuls, InverseOrdreColonne, LdebParLigneDeU,
                                    IndiceColonneDeU, NbTermesParLigneDeU, &NombreDeNoeudsDansLaListe );
							 											 
ListeDesNoeuds        = Matrice->ListeDesNoeuds;
NoeudDansLaliste      = Matrice->NoeudDansLaliste;

if ( CodeRet == RECURSION_NOT_OK ) {
  for ( i = 0 ; i < NombreDeNoeudsDansLaListe; i++ ) NoeudDansLaliste[ListeDesNoeuds[i]] = NOEUD_HORS_LISTE;
	return( CodeRet );
}

SolutionIntermediaire = Matrice->SolutionIntermediaire;
ElmDeU                = Matrice->ElmDeU;													
OrdreColonne          = Matrice->OrdreColonne;

j = 0;
for ( i = NombreDeNoeudsDansLaListe - 1 ; i >= 0 ; i-- ) {  
	Col = ListeDesNoeuds[i];   
	NoeudDansLaliste[Col] = NOEUD_HORS_LISTE;
	
	KpInitial = InverseOrdreColonne[Col];
  il = LdebParLigneDeU[KpInitial];
	
  /* Par convention le premier terme est le pivot et est deja inverse */
	
  X = SecondMembre[Col] * ElmDeU[il];
	
	if ( X == 0.0 ) continue;
	
	SecondMembre[Col] = 0.0;
	
  SolutionIntermediaire[KpInitial] = X;
	IndexTermesNonNuls   [j]         = KpInitial;
	j++;
	
  ilMax = il + NbTermesParLigneDeU[KpInitial];
  il++;     
  while ( il < ilMax ) {
		SecondMembre[IndiceColonneDeU[il]] -= ElmDeU[il] * X;		
    il++;
  }
}

/* Prediction du nombre de termes */
if ( /*NombreDeNoeudsDansLaListe*/ j * Matrice->RatioMoyenTransposee > ceil( SEUIL_RECURSION_T * Matrice->Rang ) ) {
  for ( i = 0 ; i < NombreDeNoeudsDansLaListe; i++ ) NoeudDansLaliste[ListeDesNoeuds[i]] = NOEUD_HORS_LISTE;
	return( ARRET_RESOLUTION_HYPER_CREUX );
}

Matrice->NbTermesAuDepartTransposee = NombreDeNoeudsDansLaListe;

for ( i = 0 ; i < j ; i++ ) NoeudDansLaliste[IndexTermesNonNuls[i]] = NOEUD_DANS_LISTE;
*NbTermesNonNuls = j;

return( POURSUITE_RESOLUTION_HYPER_CREUX );
}
/*--------------------------------------------------------------------------------------------------*/
void LU_LuSolveHTransposeeSecondMembreHyperCreux( MATRICE * Matrice, int * NbTermesNonNuls,
                                                  int * IndexTermesNonNuls )

{
int k; int il; int ilMax; double S; int * HDeb; int * HNbTerm; int * HLigne; int * HIndiceColonne;
double * SolutionIntermediaire; double * HValeur; int i; int Kp; char * NoeudDansLaliste;

if ( Matrice->NombreDeLuUpdates == 0 ) return;

NoeudDansLaliste = Matrice->NoeudDansLaliste;

HDeb                  = Matrice->HDeb;
HNbTerm               = Matrice->HNbTerm;
SolutionIntermediaire = Matrice->SolutionIntermediaire;
HLigne                = Matrice->HLigne;
HIndiceColonne        = Matrice->HIndiceColonne;
HValeur               = Matrice->HValeur;

i = *NbTermesNonNuls;
k = Matrice->NombreDeLuUpdates - 1;
while ( k >= 0 ) {
  il    = HDeb[k];
  ilMax = il + HNbTerm[k];
  if ( NoeudDansLaliste[HLigne[k]] != NOEUD_HORS_LISTE ) {
    S = SolutionIntermediaire[HLigne[k]];
    if ( S != 0.0 ) {
      while ( il < ilMax ) {
			  Kp = HIndiceColonne[il];
        SolutionIntermediaire[Kp]-= HValeur[il] * S;
		    if ( NoeudDansLaliste[Kp] == NOEUD_HORS_LISTE ) {
			    NoeudDansLaliste[Kp] = NOEUD_DANS_LISTE;
		      IndexTermesNonNuls[i] = Kp;
		      i++;
		    }				
        il++;
      }
    }
	}
  k--;
}
*NbTermesNonNuls = i;

return;
}
/*--------------------------------------------------------------------------------------------------*/
char LU_LuSolvTriangleLTransposeeSecondMembreHyperCreux( MATRICE * Matrice, double * Solution,
                                                         int * NbTermesNonNuls, int * IndexTermesNonNuls )
{
int Kp ;int il; int ilMax; double X; int i; int NombreDeNoeudsDansLaListe; double * SolutionIntermediaire;
int * LdebParLigneDeL; double * ElmDeLParLigne; int * ListeDesNoeuds; int * OrdreLigne; int j; 
int * IndiceColonneParLigneDeL; int *  NbTermesParLigneDeL; char * NoeudDansLaliste; char CodeRet;
char AllocOk; 

/* Recusion sur le graphe */
NombreDeNoeudsDansLaListe = 0;
NoeudDansLaliste = Matrice->NoeudDansLaliste;
for ( i = 0 ; i < *NbTermesNonNuls ; i++ ) NoeudDansLaliste[IndexTermesNonNuls[i]] = NOEUD_HORS_LISTE;

if ( Matrice->LdebParLigneDeL == NULL ) {
  /* Si le stockage par ligne de L n'existe pas on le cree */
  LU_ChainageParLigneDeL( Matrice, &AllocOk );
	/* En cas de probleme on repart sur la methode traditionnelles. Dans le methode traditionnelle on va
	   alors reessayer d'allouer et si ca passe pas, on utilisera uniquement le chainage pas colonne de L*/
	if ( AllocOk != OUI_LU ) 	return( RECURSION_NOT_OK ); 		   
}

LdebParLigneDeL          = Matrice->LdebParLigneDeL;  
IndiceColonneParLigneDeL = Matrice->IndiceColonneParLigneDeL;
NbTermesParLigneDeL      = Matrice->NbTermesParLigneDeL; 

CodeRet = LU_RecusionSurTriangle_2( Matrice, IndexTermesNonNuls, *NbTermesNonNuls, LdebParLigneDeL,
                                    IndiceColonneParLigneDeL, NbTermesParLigneDeL, &NombreDeNoeudsDansLaListe );
																		
ListeDesNoeuds = Matrice->ListeDesNoeuds;														
SolutionIntermediaire = Matrice->SolutionIntermediaire;

if ( CodeRet == RECURSION_NOT_OK ) {
  for ( i = 0 ; i < NombreDeNoeudsDansLaListe ; i++ ) NoeudDansLaliste[ListeDesNoeuds[i]] = NOEUD_HORS_LISTE;
	return( RECURSION_NOT_OK ); 		   
}

OrdreLigne     = Matrice->OrdreLigne;
ElmDeLParLigne = Matrice->ElmDeLParLigne;

j = 0;
for ( i = NombreDeNoeudsDansLaListe - 1 ; i >= 0 ; i-- ) {
  Kp = ListeDesNoeuds[i];
  NoeudDansLaliste[Kp] = NOEUD_HORS_LISTE;  
	if ( SolutionIntermediaire[Kp] == 0.0 ) continue;
  X = SolutionIntermediaire[Kp];
  SolutionIntermediaire[Kp] = 0.0;	
	/*if ( X == 0.0 ) continue;*/
	
	/*  */
  il    = LdebParLigneDeL[Kp];
  ilMax = il + NbTermesParLigneDeL[Kp];		
  il++;
  while ( il < ilMax ) {
    SolutionIntermediaire[IndiceColonneParLigneDeL[il]] -= ElmDeLParLigne[il] * X;			
    il++;
  }
	
  if ( fabs( X ) > ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION_TRANSPOSEE ) {
	  Solution[j] = X;
    IndexTermesNonNuls[j] = OrdreLigne[Kp];
    j++;		
	}
	
}
*NbTermesNonNuls = j;

X = Matrice->NombreDeRatiosTransposee * Matrice->RatioMoyenTransposee;
X+= (double) j / (double) Matrice->NbTermesAuDepartTransposee;
Matrice->NombreDeRatiosTransposee++;
Matrice->RatioMoyenTransposee = X / Matrice->NombreDeRatiosTransposee;

return( RECURSION_OK );
}

/*--------------------------------------------------------------------------------------------------*/
/* On a interet a ce que le type d'entree ne soit pas COMPACT_LU mais plutot ETENDUE_LU */

void LU_LuSolvSecondMembreHyperCreux( MATRICE * Matrice, double * SecondMembreEtSolution,
                                      int * IndexTermesNonNuls, int * NbTermesNonNuls,
																			char TypeDEntree, char Save, char * TypeDeSortie )
{
double * SolEtSec; int i; char CodeRet; int CodeRetour; char AllocOk; 

Matrice->SauvegardeDuResultatIntermediaire = Save;

/* Si SecondMembreEtSolution est donne sous forme comptacte, on fait un expand */
if ( TypeDEntree == COMPACT_LU ) {
  SolEtSec = Matrice->W;
  for ( i = 0 ; i < *NbTermesNonNuls ; i++ ) {
	  SolEtSec[IndexTermesNonNuls[i]] = SecondMembreEtSolution[i];
  }
}
else SolEtSec = SecondMembreEtSolution;

/* Si l'init n'a pas ete faite on la fait */
if ( Matrice->PseudoPile == NULL ) {
  LU_AllocEtInitHyperCreux( Matrice, &AllocOk );
  if ( AllocOk != OUI_LU ) {
    /* On passe en mode normal */
    LU_LuSolvLuUpdate( Matrice, SolEtSec, &CodeRetour, Save, OUI_LU, NULL, 0, 0.0 );
    goto RestaurationSolution;
	}
}

/* Resolution de Ly = Pb avec P matrice de permutation des lignes et b 
   le second membre */
CodeRet = LU_LuSolvTriangleLSecondMembreHyperCreux( Matrice, NbTermesNonNuls, IndexTermesNonNuls, SolEtSec );
if ( CodeRet != POURSUITE_RESOLUTION_HYPER_CREUX ) {  
  /* On passe en mode normal car il y a trop de termes */
	if ( CodeRet == RECURSION_NOT_OK ) {
    LU_LuSolvLuUpdate( Matrice, SolEtSec, &CodeRetour, Save, OUI_LU, NULL, 0, 0.0 );
	}
	else {
    LU_LuSolveH( Matrice ); 
    LU_LuSolvTriangleU( Matrice , SolEtSec );
  }
  goto RestaurationSolution;	
 		
	/* Il faut mettre la solution dans SecondMembreEtSolution */
	/*
  LU_MettreSolutionSousFormeHyperCreux( Matrice, SecondMembreEtSolution, IndexTermesNonNuls, NbTermesNonNuls,
																				SolEtSec, TypeDEntree, *TypeDeSortie );
	*/
  Matrice->SauvegardeDuResultatIntermediaire = NON_LU;
  return;
}
																							 
LU_LuSolveHSecondMembreHyperCreux( Matrice , NbTermesNonNuls, IndexTermesNonNuls );
																				
/* Resolution de U Q^t x = y avec Q matrice de permutation des colonnes */
CodeRet = LU_LuSolvTriangleUSecondMembreHyperCreux( Matrice, SecondMembreEtSolution, NbTermesNonNuls,
                                                    IndexTermesNonNuls , *TypeDeSortie);
																										
if ( CodeRet == RECURSION_NOT_OK ) {
	LU_LuSolvTriangleU( Matrice , SolEtSec );
  goto RestaurationSolution;	
	
	/* Il faut mettre la solution dans SecondMembreEtSolution */
	/*
  LU_MettreSolutionSousFormeHyperCreux( Matrice, SecondMembreEtSolution, IndexTermesNonNuls, NbTermesNonNuls,
																				SolEtSec, TypeDEntree, *TypeDeSortie );	
  */
}

Matrice->SauvegardeDuResultatIntermediaire = NON_LU;

return;

RestaurationSolution:
*TypeDeSortie = VECTEUR_LU; 
if ( TypeDEntree == COMPACT_LU ) {
  memcpy( (char *) SecondMembreEtSolution, (char *) SolEtSec, Matrice->Rang * sizeof(double) );
	memset( (char *) SolEtSec, 0, Matrice->Rang * sizeof(double) );
}

Matrice->SauvegardeDuResultatIntermediaire = NON_LU;

return;
}

/*--------------------------------------------------------------------------------------------------*/
void LU_LuSolvTransposeeSecondMembreHyperCreux( MATRICE * Matrice, double * SecondMembreEtSolution,
                                                int * IndexTermesNonNuls, int * NbTermesNonNuls,
																								char TypeDEntree, char * TypeDeSortie )
{
double * SolEtSec; int i; char CodeRet; int CodeRetour; char AllocOk; 

/* Si SecondMembreEtSolution est donne sous forme comptacte, on fait un expand */

if ( TypeDEntree == COMPACT_LU ) {
  SolEtSec = Matrice->W;
  for ( i = 0 ; i < *NbTermesNonNuls ; i++ ) {
	  SolEtSec[IndexTermesNonNuls[i]] = SecondMembreEtSolution[i];
  }   
}
else SolEtSec = SecondMembreEtSolution;

/* Si l'init n'a pas ete faite on la fait */
if ( Matrice->PseudoPile == NULL ) {
  LU_AllocEtInitHyperCreux( Matrice, &AllocOk );
  if ( AllocOk != OUI_LU ) {
    /* On passe en mode normal */
    LU_LuSolvTransposeeLuUpdate( Matrice, SolEtSec, &CodeRetour, OUI_LU, NULL, 0, 0.0 );
    goto RestaurationSolutionTransposee;
	}
}

CodeRet = LU_LuSolvTriangleUTransposeeSecondMembreHyperCreux( Matrice, NbTermesNonNuls, IndexTermesNonNuls, SolEtSec );
if ( CodeRet != POURSUITE_RESOLUTION_HYPER_CREUX ) {
  /* On passe en mode normal car il y a trop de termes */
	if ( CodeRet == RECURSION_NOT_OK ) {	
    LU_LuSolvTransposeeLuUpdate( Matrice, SolEtSec, &CodeRetour, OUI_LU, NULL, 0, 0.0 );
	}
	else {
    /*printf("On fait le calcul normal TRANSPOSEE car trop de termes dans la recursion \n");*/
    LU_LuSolveHTransposee( Matrice );
    LU_LuSolvTriangleLTransposee( Matrice , SolEtSec );
	}
  goto RestaurationSolutionTransposee;
	
	/* On met la solution dans SecondMembreEtSolution et en plus toujours sous forme compacte */
	/*
  LU_MettreSolutionSousFormeHyperCreux( Matrice, SecondMembreEtSolution, IndexTermesNonNuls, NbTermesNonNuls,
																				SolEtSec, TypeDEntree, TypeDeSortie );
  */
  return;
}
									
LU_LuSolveHTransposeeSecondMembreHyperCreux( Matrice , NbTermesNonNuls, IndexTermesNonNuls );

/* Resolution de U Q^t x = y avec Q matrice de permutation des colonnes */
CodeRet = LU_LuSolvTriangleLTransposeeSecondMembreHyperCreux( Matrice, SecondMembreEtSolution, NbTermesNonNuls, IndexTermesNonNuls );
if ( CodeRet == RECURSION_NOT_OK ) {
  LU_LuSolvTriangleLTransposee( Matrice , SolEtSec );
  goto RestaurationSolutionTransposee;	
	
	/* On met la solution dans SecondMembreEtSolution et en plus toujours sous forme compacte */
  /*
  TypeSor = COMPACT_LU;
	LU_MettreSolutionSousFormeHyperCreux( Matrice, SecondMembreEtSolution, IndexTermesNonNuls, NbTermesNonNuls,
																				SolEtSec, TypeDEntree, TypeSor );	
  */
}
return;
RestaurationSolutionTransposee:
*TypeDeSortie = VECTEUR_LU; 
if ( TypeDEntree == COMPACT_LU ) {
  memcpy( (char *) SecondMembreEtSolution, (char *) SolEtSec, Matrice->Rang * sizeof(double) );
	memset( (char *) SolEtSec, 0, Matrice->Rang * sizeof(double) );
}

return;
}   

/*--------------------------------------------------------------------------------------------------*/

void LU_MettreSolutionSousFormeHyperCreux( MATRICE * Matrice, double * SecondMembreEtSolution,
                                           int * IndexTermesNonNuls, int * NbTermesNonNuls,
																					 double * SolEtSec, char TypeDEntree, char TypeDeSortie )
{
double * W; int i; int j;

if ( TypeDeSortie == COMPACT_LU ) {
  if ( TypeDEntree == ADRESSAGE_INDIRECT_LU ) {
	  /* Verification pour debogage */
    if ( SecondMembreEtSolution != SolEtSec ) {
      printf("BUG dans LU_MettreSolutionSousFormeHyperCreux\n");
			exit(0);
		}		
		W = Matrice->W; /* Car Matrice->W n'a pas ete utilise */
	  for ( j = 0 , i = 0 ; i < Matrice->Rang ; i++ ) {
      if ( SolEtSec[i] != 0.0 ) {
		    W[j] = SolEtSec[i];
			  IndexTermesNonNuls[j] = i;
			  j++;
		  }
	  }
		/* On recopie W dans SecondMembreEtSolution et on remet W a 0 */
	  for ( i = 0 ; i < j ; i++ ) {
			SecondMembreEtSolution[i] = W[i];
			W[i] = 0.0;
		}			
  }	
	else { /* TypeDEntree = COMPACT_LU */
	  /* Verification pour debogage */
    if ( SecondMembreEtSolution == SolEtSec ) {
      printf("BUG dans LU_MettreSolutionSousFormeHyperCreux\n");
			exit(0);
		}			
	  for ( j = 0 , i = 0 ; i < Matrice->Rang ; i++ ) {
      if ( SolEtSec[i] != 0.0 ) {
		    SecondMembreEtSolution[j] = SolEtSec[i];
			  SolEtSec[i] = 0.0; 
			  IndexTermesNonNuls[j] = i;
			  j++;
		  }
	  }
	}
}
else { /* TypeDeSortie = ADRESSAGE_INDIRECT_LU */
	if ( TypeDEntree == ADRESSAGE_INDIRECT_LU ) {
    if ( SecondMembreEtSolution != SolEtSec ) {
      printf("BUG dans LU_MettreSolutionSousFormeHyperCreux\n");
			exit(0);
		}			
		/* Dans ce cas on avait deja fait SolEtSec = SecondMembreEtSolution */
	  for ( j = 0 , i = 0 ; i < Matrice->Rang ; i++ ) {
      if ( SecondMembreEtSolution[i] != 0.0 ) {
			  IndexTermesNonNuls[j] = i;
			  j++;
		  }
		}
	}
	else { /* TypeDEntree = COMPACT_LU */
    if ( SecondMembreEtSolution == SolEtSec ) {
      printf("BUG dans LU_MettreSolutionSousFormeHyperCreux\n");
			exit(0);
		}		
	  for ( j = 0 , i = 0 ; i < Matrice->Rang ; i++ ) {
		  SecondMembreEtSolution[i] = SolEtSec[i];
      if ( SolEtSec[i] != 0.0 ) {
			  SolEtSec[i] = 0.0;
			  IndexTermesNonNuls[j] = i;
			  j++;
		  }
		}
	} 
}

*NbTermesNonNuls = j;

return;
}


