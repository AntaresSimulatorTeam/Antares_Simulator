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

   FONCTION: A ce stade on a soit des contraintes = , soit des contraintes 
             de type < . 
             Si une variable n'intervient que dans des contraintes de 
             type < avec un coefficient de meme signe et que le signe 
             du cout de la variable est egalement le meme ou bien si 
             le cout est nul alors on peut fixer la variable sur une 
             de ses bornes. 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define TRACES  0

/*----------------------------------------------------------------------------*/

void PRS_FixerVariablesSurCritere( PRESOLVE * Presolve, int * VariableFixee )
{
int Var; int Cnt; int SuppressionPossible; char Signe; int il; int TypeBrn;
char DernierSigneTrouve; int Nbs; char ContraintesSupprimee ; double Ai;
char SupprimerLesContraintes; int ilCnt; int ilMaxCnt; int VarCnt;
char SupprimerLaVariable; double * BorneInfPourPresolve; double * BorneSupPourPresolve;
int * TypeDeBornePourPresolve; int NombreDeVariables; double * CoutLineaire;
int * Cdeb; int * Csui; int * NumContrainte; double * A; char * ContrainteInactive;
char * SensContrainte; double * ValeurDeXPourPresolve; int * Mdeb; int * NbTerm; int * Nuvar;
int * ContrainteBornanteSuperieurement; int * ContrainteBornanteInferieurement;
PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

ContraintesSupprimee = NON_PNE;

NombreDeVariables = Pne->NombreDeVariablesTrav;
CoutLineaire = Pne->LTrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;
SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ContrainteInactive = Presolve->ContrainteInactive;
ContrainteBornanteSuperieurement = Presolve->ContrainteBornanteSuperieurement;
ContrainteBornanteInferieurement = Presolve->ContrainteBornanteInferieurement;

Nbs = 0;

*VariableFixee = NON_PNE;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {

  TypeBrn = TypeDeBornePourPresolve[Var];
  if ( TypeBrn == VARIABLE_FIXE ) continue;

  if ( CoutLineaire[Var] > 0.0 )      Signe = '+';
  else if ( CoutLineaire[Var] < 0.0 ) Signe = '-';
  else 				   Signe = '|';

  SuppressionPossible = OUI_PNE;
  DernierSigneTrouve  = '|';

  il = Cdeb[Var];
  while ( il >= 0 ) {
    Ai  = A[il];
    if ( Ai == 0.0 ) goto ContrainteSuivante;
    Cnt = NumContrainte[il];
		
    if ( ContrainteInactive[Cnt] == OUI_PNE ) goto ContrainteSuivante; 
    if ( SensContrainte[Cnt] == '=' ) { 
      SuppressionPossible = NON_PNE;
      break; 
    }
    if ( Signe == '+' ) {
      if ( Ai < 0.0 ) {
        SuppressionPossible = NON_PNE;
        break;
      } 
    }
    else if ( Signe == '-' ) {
      if ( Ai > 0.0 ) {
        SuppressionPossible = NON_PNE;
        break;
      }
    }
    else { /* Alors Signe = '|' ) */        
      if ( Ai > 0.0 ) {	
        if ( DernierSigneTrouve == '-' ) {
          SuppressionPossible = NON_PNE;
          break;
	      }
	      else DernierSigneTrouve = '+';	
      }
      else if ( Ai < 0.0 ) {
        if ( DernierSigneTrouve == '+' ) {
          SuppressionPossible = NON_PNE;
          break;
	      }	
	      else DernierSigneTrouve = '-';	
      }
    }  
    ContrainteSuivante:
    il = Csui[il];
  }
  
  SupprimerLaVariable = NON_PNE;

  if ( SuppressionPossible == OUI_PNE ) {
    if ( Signe == '+' ) {      
      
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
				# if TRACES == 1
          if ( TypeBrn == VARIABLE_NON_BORNEE ) {
            printf("*** Phase de Presolve-> La variable %d est NON BORNEE or a l'optimum elle doit etre placee\n",Var);
            printf("sur sa borne inf. L'optimum est donc non borne.\n");
          }
          else {
            printf("*** Phase de Presolve-> La variable %d est INFERIEUREMENT NON BORNEE or a l'optimum elle doit etre placee\n",Var);
            printf("sur sa borne inf. L'optimum est donc non borne.\n");
          }
				# endif
        *VariableFixee = NON_PNE;
        Pne->YaUneSolution = PROBLEME_NON_BORNE;
        return;
      }
			
			# if TRACES == 1
        printf("-> Analyse des couts, variable %d cout %e fixee a sa borne inf: %e \n",Var,Pne->LTrav[Var],BorneInfPourPresolve[Var]);
			# endif
      
      SupprimerLaVariable = OUI_PNE;			
      PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneInfPourPresolve[Var] );		  
			
    }
    else if ( Signe == '-' ) {
        
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
				# if TRACES == 1
          if ( TypeBrn == VARIABLE_NON_BORNEE ) {
            printf("*** Phase de Presolve-> La variable %d est NON BORNEE or a l'optimum elle doit etre placee\n",Var);
            printf("sur sa borne sup. L'optimum est donc non borne.\n");
          }
          else {
            printf("*** Phase de Presolve-> La variable %d est SUPERIEUREMENT NON BORNEE or a l'optimum elle doit etre placee\n",Var);
            printf("sur sa borne sup. L'optimum est donc non borne.\n");
          }
				# endif
        *VariableFixee = NON_PNE;
        Pne->YaUneSolution = PROBLEME_NON_BORNE;
        return;
      }
      
			# if TRACES == 1
      printf("-> Analyse des couts, variable %d cout %e fixee a sa borne sup: %lf \n",Var,Pne->LTrav[Var],BorneSupPourPresolve[Var]);
			# endif
   			
      SupprimerLaVariable = OUI_PNE;						
      PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneSupPourPresolve[Var] );  			
			
    }
    else { /* Alors Signe = '|' ) */		
      SupprimerLesContraintes = NON_PNE;
      if ( DernierSigneTrouve == '+' ) {
        /* La variable n'intervient que dans des contraintes d'inegalite, avec un coefficient positif et 
           de plus elle n'a pas de cout => on la fixe a Xmin */
        
        /* Si la variable est non bornee inferieurement toutes les contraintes dans lesquelles elle intervient 
           seront toujours satisfaites => on peut les supprimer. ATTENTION ensuite il faut recalculer la 
           valeur de la variable en sortie */
        if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
				  SupprimerLesContraintes = OUI_PNE;
					/*printf("PRS_FixerVariablesSurCritere: suppression possible des contraintes de la variable %d\n",Var);*/
				}

				if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {        
				  # if TRACES == 1
            printf("-> Analyse des couts, variable %d sans cout, fixee a sa borne inf: %lf\n",Var, BorneInfPourPresolve[Var]);
          # endif				
          SupprimerLaVariable = OUI_PNE;				
          PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneInfPourPresolve[Var] );
				}
				
      }
      else if ( DernierSigneTrouve == '-' ) {			
        /* La variable n'intervient que dans des contraintes d'inegalite, avec un coefficient negatif et 
           de plus elle n'a pas de cout => on la fixe a Xmax */       			    
        /* Si la variable est non bornee inferieurement toutes les contraintes dans lesquelles elle intervient 
           seront toutjours satisfaites => on peut les supprimer. ATTENTION ensuite il faut recalculer la 
           valeur de la variable en sortie */
        if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
				  SupprimerLesContraintes = OUI_PNE;
					/*printf("PRS_FixerVariablesSurCritere: suppression possible des contraintes de la variable %d\n",Var);*/
				}

				if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {

				  # if TRACES == 1
            printf("-> Analyse des couts, variable %d sans cout, fixee a sa borne sup: %lf\n",Var,BorneSupPourPresolve[Var]);
          # endif 				
				  SupprimerLaVariable = OUI_PNE;
          PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneSupPourPresolve[Var] );
				}
				
      }
			else {						
        /* DernierSigneTrouve = '|' : la variable n'intervient pas dans les contraintes et son cout est nul */
        /* On n'intervient pas: il y a la routine qui enleve les variables sans contrainte */
			}
      /* Temporaire: comme je n'ai pas fait le necessaire pour pouvoir recalculer la variable en sortie 
         si elle supprime la contrainte, je ne fais rien. Mais je pense finalement qu'en sortie les valeurs extremes 
         conviennent quand meme et qu'il n'y a pas besoin de recalculer */
      if ( SupprimerLesContraintes == OUI_PNE ) {
        SupprimerLaVariable     = NON_PNE;
	      SupprimerLesContraintes = NON_PNE;
      }
      /* Fin temporaire */

      if ( SupprimerLesContraintes == OUI_PNE ) { 
        il = Cdeb[Var];
        while ( il >= 0 ) {
          ContraintesSupprimee = OUI_PNE;
          Cnt = NumContrainte[il];
          ContrainteInactive[Cnt] = OUI_PNE; /* Attention elle peut etre bornante: non car la variable est au moins non bornee d'un cote */
          /* Comme la contrainte peut etre bornante, on interdit de liberer les variables qu'elle borne */
          ilCnt    = Mdeb[Cnt];
          ilMaxCnt = ilCnt + NbTerm[Cnt];
          while ( ilCnt < ilMaxCnt ) {
            VarCnt = Nuvar[ilCnt];
            /* Comme on est dans le cas ou le cout de la variable supprimee est nul, il est inutile de modifier 
               les couts des variables de la contrainte */
            ilCnt++;
          }
          /*       */
          il = Pne->CsuiTrav[il];
        }
        Pne->VariableElimineeSansValeur[Pne->NombreDeVariablesElimineesSansValeur] = Var;  
        Pne->NombreDeVariablesElimineesSansValeur++;
      }
    }

    if ( SupprimerLaVariable == OUI_PNE ) { 
      Nbs++;
      *VariableFixee = OUI_PNE;
			
			# if TRACES == 1
        if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) printf(" Fixation d'une variable entiere, var num %d val %e \n",Var,ValeurDeXPourPresolve[Var]); 
      # endif
						
    }

  }

}

#if VERBOSE_PRS
  printf("-> Nombre de variables fixees sur critere: %d\n",Nbs);
#endif

return;
} 


