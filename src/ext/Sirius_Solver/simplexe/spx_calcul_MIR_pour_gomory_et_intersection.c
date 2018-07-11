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

   FONCTION: Calcul d'une mir sur les donnees preparees pour le calcul
	           des Gomory et des coupes d'intersection 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_define.h"
# include "pne_fonctions.h"

# include "lu_define.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

# define AJUSTER_SECOND_MEMBRE OUI_SPX /* Correction du second membre au cas ou on ecrete un coefficient */
# define NOMBRE_MAX_DE_VARIABLES_DECART_DE_COUPES 10 /* Seuil du nombre de variables d'ecart de coupes au dela duquel on
                                                        ne prend pas en compte la coupe qu'on est en train de calculer */

/*----------------------------------------------------------------------------*/

void SPX_CalculMIRPourCoupeDeGomoryOuIntersection(
       PROBLEME_SPX * Spx,
			    
       double RapportMaxDesCoeffs,
			 double ZeroPourCoeffVariablesDEcart,
			 double ZeroPourCoeffVariablesNatives,
			 double RelaxRhsAbs,
			 double RelaxRhsRel,

			 double AlphaI0, 
			 double * B,
			 /* Pour calculs intermediaires */
			 char * T,
			 double * Coeff,
			 char * LaVariableSpxEstEntiere,
			 
       /* En retour, la coupe */
       int   * NombreDeTermes       , /* Nombre de coefficients non nuls dans la coupe */
       double * Coefficient          , /* Coefficients de la contrainte */
       int   * IndiceDeLaVariable   , /* Indices des variables qui interviennent dans la coupe */
       double * SecondMembre,         /* Remarque: la coupe est toujours dans le sens <= SecondMembre */
       char * OnAEcrete )
{
int VarSpx ; int Var2Spx ; double F0; double F0SurUnMoinsF0; double Fj; double NBarreR; int Cnt;
int il; int ilMax; double Co; double BGomory; double ValeurDuZero; double PlusPetitTerme;
double PlusGrandTerme; double SeuilGrandesBornes; double CoSpx; int * Cdeb; double * ACol;
int * CNbTerm; int * NumeroDeContrainte; char * OrigineDeLaVariable; int NumCoupe; double BG0;
char * PositionDeLaVariable; double * ScaleX; double * XminEntree; double * XmaxEntree; double * Xmax;
double * Xmin; int * Mdeb; int * NbTerm;int * Indcol; double * A; double ScaleSpx;
int * CorrespondanceVarSimplexeVarEntree; char * TypeDeVariable; int NbVarEcartDeCoupes;
int * CorrespondanceCntSimplexeCntEntree; int NombreDeContraintesDuProblemeSansCoupes;
int NbCorrectionsImpossibles; double CumulCorrections; int NN; double PlusPetitTermeDeLaMatrice;
double PlusGrandTermeDeLaMatrice; double RapportCoupe; int i;

PROBLEME_PNE * Pne;

*OnAEcrete = NON_SPX;
*NombreDeTermes = 0;

Pne = NULL;
Pne = (PROBLEME_PNE *) Spx->ProblemePneDeSpx;

ValeurDuZero = ZERO_TERMES_DU_TABLEAU_POUR_GOMORY;

NbVarEcartDeCoupes = 0;

SeuilGrandesBornes = RapportMaxDesCoeffs;

ACol               = Spx->ACol; 
Cdeb               = Spx->Cdeb;
CNbTerm            = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;

OrigineDeLaVariable  = Spx->OrigineDeLaVariable;
PositionDeLaVariable = Spx->PositionDeLaVariable;
TypeDeVariable = Spx->TypeDeVariable;
ScaleX         = Spx->ScaleX;
XminEntree     = Spx->XminEntree;
XmaxEntree     = Spx->XmaxEntree;
Xmax           = Spx->Xmax;
Xmin           = Spx->Xmin;

NombreDeContraintesDuProblemeSansCoupes = Spx->NombreDeContraintesDuProblemeSansCoupes;
CorrespondanceCntSimplexeCntEntree = Spx->CorrespondanceCntSimplexeCntEntree;
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A      = Spx->A;
TypeDeVariable = Spx->TypeDeVariable;
CorrespondanceVarSimplexeVarEntree = Spx->CorrespondanceVarSimplexeVarEntree;

F0 = AlphaI0 - floor( AlphaI0 );
BGomory = F0;

/* Test de division par 0 */
if ( fabs( 1. - F0 ) > ZERO_GOMORY_1_F0 ) {  
  F0SurUnMoinsF0 = F0 / ( 1. - F0 );
}
else return;

/* On applique les regles d'arrondi */
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  if ( T[VarSpx] == 0 ) continue;
  NBarreR = Coeff[VarSpx];		
  if ( OrigineDeLaVariable[VarSpx] == ECART ) {
    /* C'est une variable d'ecart non connue de l'appelant, on considere qu'elle est reelle. */
    if ( NBarreR > 0. ) Co = NBarreR;
    else Co = -NBarreR * F0SurUnMoinsF0;
    /* Si le coeff de la variable d'ecart est trop petit, on n'en tient pas compte */
    if ( fabs( Co ) < ZeroPourCoeffVariablesDEcart && 0 ) {						
      T[VarSpx] = 0;			
			continue;
		}	
  }
  else {		
    /* C'est une variable connue de l'appelant */
    if ( LaVariableSpxEstEntiere[VarSpx] == OUI_SPX ) {		
      Fj = NBarreR - floor( NBarreR );								
      if ( Fj <= F0 ) Co = Fj;
      else            Co = F0SurUnMoinsF0 * ( 1. - Fj );
    }
    else { /* Cas d'une variable reelle */
      if ( NBarreR > 0. ) Co = NBarreR;
      else  Co = -NBarreR * F0SurUnMoinsF0;
    }    
  }
  /* Si le coeff de la variable native est trop petit, on n'en tient pas compte */
  /* Pour faire ce test on se replace dans le contexte du simplexe apres scaling quand on utilisera a coupe */
	CoSpx = fabs( Co ) * ScaleX[VarSpx];
  if ( CoSpx < ZeroPourCoeffVariablesNatives && 0 ) {		
		if ( CoSpx > 0.1 * ZeroPourCoeffVariablesNatives ) *OnAEcrete = OUI_SPX;		
    T[VarSpx] = 0;
    # if AJUSTER_SECOND_MEMBRE == OUI_SPX 
		  /* Il faut ajuster le second membre */		
		  if ( TypeDeVariable[VarSpx] == BORNEE ) {			
        if ( Xmax[VarSpx] - Xmin[VarSpx] < SeuilGrandesBornes ) {				
          if ( LaVariableSpxEstEntiere[VarSpx] == OUI_SPX ) { 
            /* La variable est entiere. Si elle a ete instanciee a 0, l'information HORS_BASE_SUR_BORNE_INF ou 
               HORS_BASE_SUR_BORNE_SUP ne joue pas. Si elle a ete instanciee a 1 alors c'est comme si elle 
               etait HORS_BASE_SUR_BORNE_SUP */
            if ( XminEntree[VarSpx] == XmaxEntree[VarSpx] ) {
              if ( XminEntree[VarSpx] != 0. ) Co = -Co;
            }
					}					
				  if ( Co > 0.0 ) BGomory -= Co * ScaleX[VarSpx] * Xmax[VarSpx];
				  else  BGomory -= Co * ScaleX[VarSpx] * Xmin[VarSpx];
        }
      }
		# endif		
		continue;
	}
  /* Le coeff est pris en compte */
  Coeff[VarSpx] = Co;
  if ( LaVariableSpxEstEntiere[VarSpx] == OUI_SPX ) { 
    /* La variable est entiere. Si elle a ete instanciee a 0, l'information HORS_BASE_SUR_BORNE_INF ou 
       HORS_BASE_SUR_BORNE_SUP ne joue pas. Si elle a ete instanciee a 1 alors c'est comme si elle 
       etait HORS_BASE_SUR_BORNE_SUP */
    if ( XminEntree[VarSpx] == XmaxEntree[VarSpx] ) {
      /* C'est une variable instanciee */
      if ( XminEntree[VarSpx] == 0. ) continue;
      BGomory -= Coeff[VarSpx] * Xmax[VarSpx] * ScaleX[VarSpx];				
      Coeff[VarSpx] = -Coeff[VarSpx];      
      continue;
    }      
  }    
  if ( PositionDeLaVariable[VarSpx] == HORS_BASE_SUR_BORNE_SUP ) {
    /* Remarque: une variable d'ecart ne peut pas etre HORS_BASE_SUR_BORNE_SUP */
    /* On refait les changements de variables necessaires Xtilde = Xmax - X */			
    BGomory -= Coeff[VarSpx] * Xmax[VarSpx] * ScaleX[VarSpx]; 	
    Coeff[VarSpx] = -Coeff[VarSpx];      
  }	
}

/* Cas ou le probleme Pne n'est pas sous la forme standard: on trazouille la contrainte pour que si une variable d'ecart etait 
   dans la contrainte, on la remplace par des variables connues du niveau "pne" */
   
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  if ( T[VarSpx] == 0 ) continue;
  if ( OrigineDeLaVariable[VarSpx] != ECART ) continue;

  /* Var est une variable non native et elle fait partie de la coupe de Gomory */
  T[VarSpx] = 0;
  Co = Coeff[VarSpx];
  ScaleSpx = ScaleX[VarSpx];
	if ( ScaleSpx != 1. ) {
	  *NombreDeTermes = 0;
		return;
	}
  /* Contrainte a laquelle appartient la variable d'ecart */
  Cnt = NumeroDeContrainte[Cdeb[VarSpx]];
  /* C'est une coupe ? */	
  if ( Cnt >= NombreDeContraintesDuProblemeSansCoupes && Pne != NULL ) {
    NumCoupe = CorrespondanceCntSimplexeCntEntree[Cnt] - NombreDeContraintesDuProblemeSansCoupes;		
    if ( Pne->Coupes.NbTerm[NumCoupe] > 0 ) {		
      if ( Pne->Coupes.TypeDeCoupe[NumCoupe] == 'G' || Pne->Coupes.TypeDeCoupe[NumCoupe] == 'I' ) {  
        /* Si c'est une Gomory ou derivee de Gomory, on incremente le compteur */   	
        NbVarEcartDeCoupes++;
		    if ( NbVarEcartDeCoupes > NOMBRE_MAX_DE_VARIABLES_DECART_DE_COUPES ) {
	        *NombreDeTermes = 0;
		      return;
				}
			}
		}
	}	
  /* Remplacement de la variable d'ecart: la variable d'ecart est toujours la derniere variable */	
  il    = Mdeb[Cnt]; 
  ilMax = il + NbTerm[Cnt];
	ilMax--;		
  while ( il < ilMax ) {
    Var2Spx = Indcol[il];		
    /* Ici il n'y a pas lieu de tenir compte du fait que la variable serait HORS_BASE_SUR_BORNE_SUP */ 
    T[Var2Spx] = 1;
    Coeff[Var2Spx] -= Co * A[il] * ScaleSpx / ScaleX[Var2Spx];			
    il++;
  }
	/* B est avant translation (sauf pour les variables entieres ou on a remis toujours xmin=0) et apres scaling */
	BGomory -= Co * B[Cnt] * ScaleSpx;	
}

/* Il faut maintenant tenir compte de la translation des bornes */

for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  if ( T[VarSpx] == 0 ) continue;	
  /* Translation des bornes */	
  if ( TypeDeVariable[VarSpx] == BORNEE || TypeDeVariable[VarSpx] == BORNEE_INFERIEUREMENT ) {
    /* Translation des bornes */  
    if ( LaVariableSpxEstEntiere[VarSpx] == OUI_SPX ) continue; 
    BGomory += Coeff[VarSpx] * XminEntree[VarSpx]; 
  }	
}

/* Decompte du nombre de termes, attribution des numeros de variables connus de la partie pne 
   et transformation de la contrainte en une contrainte de type < */
	 
PlusPetitTerme = LINFINI_SPX;
PlusGrandTerme = -1.;  
*NombreDeTermes = 0;

CumulCorrections = 0.0;
NbCorrectionsImpossibles = 0;
BG0 = BGomory;
NN = 0;

for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {	
  if ( T[VarSpx] == 0 ) continue;
  T[VarSpx] = 0;
	NN++;
  /* Si le coeff de la variable native est trop petit, on n'en tient pas compte */
  /* Pour faire ces test on se replace dans le contexte du simplexe apres scaling quand on utilisera a coupe */		
	CoSpx = fabs( Coeff[VarSpx] ) * ScaleX[VarSpx];
	if ( CoSpx < ZeroPourCoeffVariablesNatives ) {
		if ( CoSpx > 0.1 * ZeroPourCoeffVariablesNatives ) *OnAEcrete = OUI_SPX;		
    # if AJUSTER_SECOND_MEMBRE == OUI_SPX 	
		  /* Il faut ajuster le second membre */
		  if ( TypeDeVariable[VarSpx] == BORNEE ) {
        if ( Xmax[VarSpx] - Xmin[VarSpx] < SeuilGrandesBornes ) {				  
				  if ( Coeff[VarSpx] > 0.0 ) {
					  Co = Coeff[VarSpx] * ScaleX[VarSpx] * Xmax[VarSpx];
						/* Il n'est pas clair s'il est interessant ou non de faire une correction */
					  BGomory -= Co;
						CumulCorrections += fabs( Co );
					}
				  else  {
					  Co = Coeff[VarSpx] * ScaleX[VarSpx] * Xmin[VarSpx];
						/* Il n'est pas clair s'il est interessant ou non de faire une correction */
					  BGomory -= Co;
						CumulCorrections += fabs( Co );
					}        					
        }
				else {
				  if ( CoSpx != 0.0 ) NbCorrectionsImpossibles++;
				}
      }
		# else
		  if ( TypeDeVariable[VarSpx] == BORNEE ) {
        if ( Xmax[VarSpx] - Xmin[VarSpx] < SeuilGrandesBornes ) {				  
				  if ( Coeff[VarSpx] > 0.0 ) {
					  Co = Coeff[VarSpx] * ScaleX[VarSpx] * Xmax[VarSpx];
						CumulCorrections += fabs( Co );
					}
				  else  {
					  Co = Coeff[VarSpx] * ScaleX[VarSpx] * Xmin[VarSpx];
						CumulCorrections += fabs( Co );
					}        					
        }
				else {
				  if ( CoSpx != 0.0 ) NbCorrectionsImpossibles++;
				}
      }		
		# endif
	  continue;
	}	
  if ( CoSpx < PlusPetitTerme ) PlusPetitTerme = CoSpx;
	if ( CoSpx > PlusGrandTerme ) PlusGrandTerme = CoSpx;  
  Coefficient[*NombreDeTermes] = -Coeff[VarSpx];  /* Pour la transformation en une contrainte de type < */
  IndiceDeLaVariable[*NombreDeTermes] = CorrespondanceVarSimplexeVarEntree[VarSpx];  
  *NombreDeTermes = *NombreDeTermes + 1;	
}

if ( fabs( BGomory ) > ZERO_TERMES_DU_TABLEAU_POUR_GOMORY ) {
  if ( fabs( CumulCorrections / BG0 ) > 1.e-8 ) {
    *NombreDeTermes = 0;
  }
}
if ( CumulCorrections > 1.e-8 ) {
  *NombreDeTermes = 0;
}
if ( NbCorrectionsImpossibles > 0.1 * NN ) {
  /* Trop de corrections impossibles */
  *NombreDeTermes = 0;
}

if ( *NombreDeTermes <= 0 ) return;

/* Modification du RapportMaxDesCoeffs en fonction du scaling */
if ( RapportMaxDesCoeffs < Spx->RapportDeScaling ) RapportMaxDesCoeffs = Spx->RapportDeScaling;

PlusPetitTermeDeLaMatrice = Spx->PlusPetitTermeDeLaMatrice;
PlusGrandTermeDeLaMatrice = Spx->PlusGrandTermeDeLaMatrice;	

if ( PlusGrandTerme/PlusPetitTerme > RapportMaxDesCoeffs ) {
	/*
	printf("Refus calcul de G car Rapport %e et RapportMaxDesCoeffs = %e  NombreDeTermes = %d\n",PlusGrandTerme/PlusPetitTerme,
	        RapportMaxDesCoeffs,*NombreDeTermes);
	*/
  *NombreDeTermes = 0;
  return;
}

if ( *OnAEcrete == OUI_SPX ) {
  if ( PlusGrandTerme < RapportMaxDesCoeffs * ZeroPourCoeffVariablesNatives ) {
    /*
		printf("Refus calcul de G car PlusGrandTerme %e OnAEcrete\n",PlusGrandTerme);
		*/
	  *NombreDeTermes = 0;
		return;
	}
}

/* On recale la contrainte sur le plus petit et plus grand terme de la matrice */
RapportCoupe = 1.;
# ifdef ON_COMPILE
if ( PlusGrandTerme > PlusGrandTermeDeLaMatrice ) {
  /* On fait une homothetie vers PlusGrandTermeDeLaMatrice */
  RapportCoupe = PlusGrandTermeDeLaMatrice / PlusGrandTerme;
}
else if ( PlusPetitTerme < PlusPetitTermeDeLaMatrice ) {
  /* On fait une homothetie vers PlusPetitTermeDeLaMatrice */
  RapportCoupe = PlusPetitTermeDeLaMatrice / PlusPetitTerme;
}
# endif

if ( PlusGrandTerme > PlusGrandTermeDeLaMatrice ) {
  RapportCoupe = PlusGrandTermeDeLaMatrice / PlusGrandTerme;
}

if ( RapportCoupe != 1. ) {
  /*SPX_ArrondiEnPuissanceDe2( &RapportCoupe );*/				
  for ( i = 0 ; i < *NombreDeTermes ; i++ ) Coefficient[i] *= RapportCoupe;				
  BGomory *= RapportCoupe;				
}

/* Pour la transformation en une contrainte de type < on prend -BGomory */
Co = fabs( BGomory ) * RelaxRhsRel;
if ( Co < RelaxRhsAbs ) {
  *SecondMembre = -BGomory + RelaxRhsAbs;
}
else {
  *SecondMembre = -BGomory + Co;
}
 		
return;
}
     

