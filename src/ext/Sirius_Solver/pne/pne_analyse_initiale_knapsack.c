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

   FONCTION: Analyse initiale des contraintes de sac a dos.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif     
  
/*----------------------------------------------------------------------------*/

void PNE_AnalyseInitialeDesKnapsack( PROBLEME_PNE * Pne )
{
int Cnt; int il; int ilMax; int Var; char TypeBorne; char Inf; char Sup; int NbBin;
int * MdebTrav; int * NbTermTrav; int * NuvarTrav; int * TypeDeVariableTrav; double * ATrav; 
char * ContrainteKnapsack; char * SensContrainteTrav; int * TypeDeBorneTrav;
char QueDesUn; 

if ( Pne->ContrainteKnapsack == NULL ) {   
  Pne->ContrainteKnapsack  = (char *) malloc( Pne->NombreDeContraintesAllouees * sizeof( char ) );
  if ( Pne->ContrainteKnapsack == NULL ) {
    printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_AnalyseInitialeDesKnapsak \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }	
}

SensContrainteTrav = Pne->SensContrainteTrav;
MdebTrav   = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
NuvarTrav  = Pne->NuvarTrav;
ATrav      = Pne->ATrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;

ContrainteKnapsack = Pne->ContrainteKnapsack;
for ( Cnt = 0 ; Cnt <  Pne->NombreDeContraintesTrav ; Cnt++ ) {
  ContrainteKnapsack[Cnt] = IMPOSSIBLE;
}

/*
printf(" sortie des contraintes dans PNE_AnalyseInitialeDesKnapsack\n");
printf("Nombre de contraintes: %d\n",Pne->NombreDeContraintesTrav);
for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  printf("Contrainte %d\n",Cnt);
  il    = MdebTrav[Cnt];
  ilMax = il + NbTermTrav[Cnt];	
  while ( il < ilMax ) {	
    Var = NuvarTrav[il];		
		TypeBorne = TypeDeBorneTrav[Var];
		if ( TypeBorne != VARIABLE_FIXE ) {
		  if ( TypeDeVariableTrav[Var] == ENTIER ) {
		    printf("%e (i_%d) ",ATrav[il],Var);				
		  }
		  else {
		    if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES ) printf("%e (cb2_%d) ",ATrav[il],Var);
			  else if ( TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) printf("%e (cbs_%d) ",ATrav[il],Var);
			  else if ( TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) printf("%e (cbi_%d) ",ATrav[il],Var);
			  else if ( TypeBorne == VARIABLE_NON_BORNEE ) printf("%e (cnb_%d) ",ATrav[il],Var);
		  }
		}
		il++;
	}
	printf(" %c %e\n",SensContrainteTrav[Cnt],Pne->BTrav[Cnt]);
}
exit(0);
*/

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  /* Limite sur le nombre de variables */
	QueDesUn = OUI_PNE;
	Inf = INF_POSSIBLE;
	Sup = SUP_POSSIBLE;
	NbBin = 0;
	# if KNAPSACK_SUR_CONTRAINTES_DEGALITE == NON_PNE
    if ( SensContrainteTrav[Cnt] == '=' ) {
	    Inf = IMPOSSIBLE;
	    Sup = IMPOSSIBLE;
		  continue;
    }
	# endif
  if ( SensContrainteTrav[Cnt] == '<' ) Sup = IMPOSSIBLE;	
  il    = MdebTrav[Cnt];
  ilMax = il + NbTermTrav[Cnt];	
  while ( il < ilMax ) {	
    Var = NuvarTrav[il];		
		TypeBorne = TypeDeBorneTrav[Var];		
    if ( TypeBorne == VARIABLE_FIXE ) goto NextIl;
		if ( fabs( ATrav[il] ) != 1.0 && ATrav[il] != 0.0 ) QueDesUn = NON_PNE; 
    if ( TypeDeVariableTrav[Var] == ENTIER ) {
		  NbBin++;
		  goto NextIl;
		}
		/* La variable n'est pas entiere */				
		if ( ATrav[il] < 0.0 ) {
		  if ( Inf != IMPOSSIBLE ) {
		    /* Il faut monter la variable au max */								
			  if ( TypeBorne != VARIABLE_BORNEE_DES_DEUX_COTES && TypeBorne != VARIABLE_BORNEE_SUPERIEUREMENT ) {										
				  Inf = IMPOSSIBLE;					
        }				
			}
			if ( Sup != IMPOSSIBLE ) {
		    /* Il faut baisser la variable au min */
			  if ( TypeBorne != VARIABLE_BORNEE_DES_DEUX_COTES && TypeBorne != VARIABLE_BORNEE_INFERIEUREMENT ) {										
					Sup = IMPOSSIBLE;											
				}
			}
		}
		else if ( ATrav[il] > 0.0 ) {
		  if ( Inf != IMPOSSIBLE ) {
			  /* Il faut baisser la variable au min */				
			  if ( TypeBorne != VARIABLE_BORNEE_DES_DEUX_COTES && TypeBorne != VARIABLE_BORNEE_INFERIEUREMENT ) {										
					Inf = IMPOSSIBLE;					
        }								
			}
			if ( Sup != IMPOSSIBLE ) {
		    /* Il faut monter la variable au max */
			  if ( TypeBorne != VARIABLE_BORNEE_DES_DEUX_COTES && TypeBorne != VARIABLE_BORNEE_SUPERIEUREMENT ) {
					Sup = IMPOSSIBLE;					 
        }								
			}			
		}
    
		NextIl:
		if ( Inf == IMPOSSIBLE && Sup == IMPOSSIBLE ) break;
    il++;
  }
		
	/* Synthese */	
	if ( NbBin >= MIN_TERMES_POUR_KNAPSACK && NbBin <= MAX_TERMES_POUR_KNAPSACK ) {
	  if ( Inf == INF_POSSIBLE ) {
		  if ( QueDesUn == NON_PNE ) {
        if ( Sup == SUP_POSSIBLE ) ContrainteKnapsack[Cnt] = INF_ET_SUP_POSSIBLE;
		    else ContrainteKnapsack[Cnt] = INF_POSSIBLE;				
			}
	  }
	  else if ( Sup == SUP_POSSIBLE ) {
		  if ( QueDesUn == NON_PNE ) {
		    ContrainteKnapsack[Cnt] = SUP_POSSIBLE;				
			}
		}
	}
			
}

return;
}
 
