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

   FONCTION: Prise en compte des contraintes de borne variable dans le
	           presolve simplifie.
                 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"
 
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TRACES NON_PNE

# define ZERFIX 1.e-8
# define MARGE  1.e-5

# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE

/*----------------------------------------------------------------------------*/

void PNE_PresolveSimplifieContraintesDeBornesVariables( PROBLEME_PNE *  Pne, int * Faisabilite,
                                                        char * RefaireUnCycle ) 
{
int Cnt; int * First; int ilbin; int ilcont; double B; int Varcont; int * Colonne;
double * SecondMembre; double * Coefficient; int Varbin; CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable;
int NombreDeContraintesDeBorne; int * TypeDeVariable; char * BorneSupConnue; char * BorneInfConnue;
double XmaxVarcont; double * ValeurDeBorneSup; double * ValeurDeBorneInf; double XminVarcont;
char BrnInfConnue; PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; double S0; double S1;
char UneVariableAEteFixee; char BorneMiseAJour; double NouvelleValeur;

*RefaireUnCycle = NON_PNE;
if ( *Faisabilite == NON_PNE ) return;

# if UTILISER_LE_GRAPHE_DE_CONFLITS == OUI_PNE
  # if PROBING_JUSTE_APRES_LE_PRESOLVE == NON_PNE  
    return;
  # endif
# endif

if ( Pne->ContraintesDeBorneVariable == NULL ) return;

if ( Pne->ProbingOuNodePresolve == NULL ) return;

ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;

TypeDeVariable = Pne->TypeDeVariableTrav;

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;
BorneSupConnue = ProbingOuNodePresolve->BorneSupConnue;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;

First = ContraintesDeBorneVariable->First;
SecondMembre = ContraintesDeBorneVariable->SecondMembre;
Colonne = ContraintesDeBorneVariable->Colonne;
Coefficient = ContraintesDeBorneVariable->Coefficient;

NombreDeContraintesDeBorne = ContraintesDeBorneVariable->NombreDeContraintesDeBorne;

/* La variable continue est toujours placee en premier */

for ( Cnt = 0 ; Cnt < NombreDeContraintesDeBorne ; Cnt++ ) {
  if ( First[Cnt] < 0 ) continue;	
  ilcont = First[Cnt];
	ilbin = ilcont + 1;
	B = SecondMembre[Cnt];

	Varcont = Colonne[ilcont];
	Varbin = Colonne[ilbin];

  UneVariableAEteFixee = NON_PNE;
  BorneMiseAJour = NON_PNE;
		
  BrnInfConnue = BorneInfConnue[Varbin];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) {
    /* La variable binaire est fixee */
    if ( Coefficient[ilcont] > 0 ) {
		  /* La contrainte est une contrainte de borne sup */
			XmaxVarcont = ( B - ( Coefficient[ilbin] * ValeurDeBorneInf[Varbin] ) ) / Coefficient[ilcont];		
			if ( BorneSupConnue[Varcont] == OUI_PNE || 1 ) {
			  if ( XmaxVarcont < ValeurDeBorneSup[Varcont] - MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d on peut abaisser a borne sup de %e a %e\n",Varcont,ValeurDeBorneSup[Varcont],XmaxVarcont);
					# endif
          NouvelleValeur = XmaxVarcont;
          BorneMiseAJour = MODIF_BORNE_SUP;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varcont, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}
				}
			}
			if ( BorneInfConnue[Varcont] == OUI_PNE ) {
			  if ( ValeurDeBorneInf[Varcont] > XmaxVarcont + MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d pas de solution car XmaxVarcont = %e et ValeurDeBorneInf = %e\n",Varcont,XmaxVarcont,ValeurDeBorneInf[Varcont]);
					# endif
          *Faisabilite = NON_PNE;
		      return;		      				
				}
				else if ( fabs( ValeurDeBorneInf[Varcont] - XmaxVarcont ) < ZERFIX ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d fixee a ValeurDeBorneInf = %e\n",Varcont,ValeurDeBorneInf[Varcont]);
					# endif
					/* A completer */
          NouvelleValeur = ValeurDeBorneInf[Varcont];
          UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varcont, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}					
				}
			}			
		}
		else {
		  /* La contrainte est une contrainte de borne inf */
			XminVarcont = ( -B + ( Coefficient[ilbin] * ValeurDeBorneInf[Varbin] ) ) / fabs( Coefficient[ilcont] );			
			if ( BorneInfConnue[Varcont] == OUI_PNE || 1 ) {
			  if ( XminVarcont > ValeurDeBorneInf[Varcont] + MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d on peut remonter a borne inf de %e a %e\n",Varcont,ValeurDeBorneInf[Varcont],XminVarcont);
					# endif						
          NouvelleValeur = XminVarcont;
          BorneMiseAJour = MODIF_BORNE_INF;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varcont, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}			 				
        }
			}
			if ( BorneSupConnue[Varcont] == OUI_PNE ) {
			  if ( ValeurDeBorneSup[Varcont] < XminVarcont - MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d pas de solution car XminVarcont = %e et ValeurDeBorneSup = %e\n",Varcont,XminVarcont,ValeurDeBorneSup[Varcont]);
					# endif						
          *Faisabilite = NON_PNE;
		      return;		      								
				}
				else if ( fabs( XminVarcont - ValeurDeBorneSup[Varcont] ) < ZERFIX ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d fixee a ValeurDeBorneInf = %e\n",Varcont,ValeurDeBorneSup[Varcont]);
					# endif												
          NouvelleValeur = ValeurDeBorneSup[Varcont];
          UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varcont, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}								
				}				
			}
		}
	}
	else {
    /* La variable binaire n'est pas fixee */
    if ( Coefficient[ilcont] > 0 ) {
		  /* La contrainte est une contrainte de borne sup */
			/* Fixation a 0 */
			S0 = ( B - ( Coefficient[ilbin] * ValeurDeBorneInf[Varbin] ) ) / Coefficient[ilcont];
			XmaxVarcont = S0;
			if ( BorneInfConnue[Varcont] == OUI_PNE ) {
			  if ( ValeurDeBorneInf[Varcont] > XmaxVarcont + MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable entiere %d valeur 0 interdite\n",Varbin);
					# endif												
          NouvelleValeur = ValeurDeBorneSup[Varbin];
          UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varbin, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}							
				}
			}			
			/* Fixation a 1 */
		  S1 = ( B - ( Coefficient[ilbin] * ValeurDeBorneSup[Varbin] ) ) / Coefficient[ilcont];
			XmaxVarcont = S1;
			if ( BorneInfConnue[Varcont] == OUI_PNE ) {
			  if ( ValeurDeBorneInf[Varcont] > XmaxVarcont + MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable entiere %d valeur 1 interdite\n",Varbin);
					# endif																		
          NouvelleValeur = ValeurDeBorneInf[Varbin];
          UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varbin, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}						
				}
			}			
      if ( S0 < S1 ) XmaxVarcont = S1;
			else XmaxVarcont = S0;			
			if ( BorneSupConnue[Varcont] == OUI_PNE || 1 ) {
			  if ( XmaxVarcont < ValeurDeBorneSup[Varcont] - MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d on peut abaisser a borne sup de %e a %e\n",Varcont,ValeurDeBorneSup[Varcont],XmaxVarcont);
					# endif																		
          NouvelleValeur = XmaxVarcont;
          BorneMiseAJour = MODIF_BORNE_SUP;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varcont, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}					
				}
			}	
		}
		else {
		  /* La contrainte est une contrainte de borne inf */
			S0 = ( -B + ( Coefficient[ilbin] * ValeurDeBorneInf[Varbin] ) ) / fabs( Coefficient[ilcont] );
      XminVarcont = S0;
			if ( BorneSupConnue[Varcont] == OUI_PNE ) {
			  if ( ValeurDeBorneSup[Varcont] < XminVarcont - MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable entiere %d valeur 0 interdite\n",Varbin);
					# endif																		
          NouvelleValeur = ValeurDeBorneSup[Varbin];
          UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varbin, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}									
				}
			}			
			S1 = ( -B + ( Coefficient[ilbin] * ValeurDeBorneSup[Varbin] ) ) / fabs( Coefficient[ilcont] );
      XminVarcont = S1;			
			if ( BorneSupConnue[Varcont] == OUI_PNE ) {
			  if ( ValeurDeBorneSup[Varcont] < XminVarcont - MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable entiere %d valeur 1 interdite\n",Varbin);
					# endif																		
          NouvelleValeur = ValeurDeBorneInf[Varbin];
          UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varbin, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}								
				}
			}
      if ( S0 < S1 ) XminVarcont = S0;
			else XminVarcont = S1;			
			if ( BorneInfConnue[Varcont] == OUI_PNE || 1 ) {
			  if ( XminVarcont > ValeurDeBorneInf[Varcont] + MARGE ) {
				  # if TRACES == OUI_PNE 
				    printf("Contrainte de borne variable: variable %d on peut remonter a borne inf de %e a %e\n",Varcont,ValeurDeBorneInf[Varcont],XminVarcont);
					# endif																		
          NouvelleValeur = XminVarcont;
          BorneMiseAJour = MODIF_BORNE_INF;
					*RefaireUnCycle = OUI_PNE;
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Varcont, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
          if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
					  *Faisabilite = NON_PNE;
		        return;		        
					}							
        }
			}			
		}		
	}			 	
}

return;
}

/*----------------------------------------------------------------------------*/

# endif
