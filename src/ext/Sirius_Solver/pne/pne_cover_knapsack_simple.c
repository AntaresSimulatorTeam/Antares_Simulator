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

   FONCTION: Recherche de couverture de sac a dos 

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_CoverKnapsackSimple( PROBLEME_PNE * Pne )
{
int Cnt; int il; int ilMax; int ilDeb; int ilFin; char CouvertureTrouvee; 
int NbKnapsack; int Var; int NombreDeTermes; double SecondMembre; double * Coeff;
int * Variable; char Inf; char Sup; char * ContrainteKnapsack; int * MdebTrav;
int * NbTermTrav; double * BTrav; double * ATrav; int * NuvarTrav; int * TypeDeVariableTrav;
int TypeBorne; int * TypeDeBorneTrav; double * UTrav; double * UminTrav; double * UmaxTrav;
char * VariableSupprimee; char * ContrainteDejaUtilisee; double * VariableDuale;
char Mixed_0_1_Knapsack; int Cnt1; int i; char Found; double u; double l; double bBorne;
int VarBin; int * CntDeBorneInfVariable; int * CntDeBorneSupVariable; char RendreLesCoeffsEntiers;

/*printf("PNE_CoverKnapsackSimple\n");*/  

ContrainteDejaUtilisee = (char *) malloc( Pne->NombreDeContraintesTrav * sizeof( char ) );
if ( ContrainteDejaUtilisee == NULL ) {
  printf("Saturation memoire dans PNE_CoverKnapsackSimple\n");
	return;
}
VariableSupprimee = (char *) malloc( Pne->NombreDeVariablesTrav * sizeof( char ) );
if ( VariableSupprimee == NULL ) {
  free( ContrainteDejaUtilisee );
  printf("Saturation memoire dans PNE_CoverKnapsackSimple\n");
	return;
}

Mixed_0_1_Knapsack = NON_PNE;

CntDeBorneInfVariable = Pne->CntDeBorneInfVariable;
CntDeBorneSupVariable = Pne->CntDeBorneSupVariable;

MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
BTrav = Pne->BTrav;   
ATrav = Pne->ATrav;
NuvarTrav = Pne->NuvarTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
UTrav = Pne->UTrav;
UminTrav = Pne->UminTravSv;
UmaxTrav = Pne->UmaxTravSv;

VariableDuale = Pne->VariablesDualesDesContraintesTravEtDesCoupes;

ContrainteKnapsack = Pne->ContrainteKnapsack;
Coeff = Pne->ValeurLocale;
Variable = Pne->IndiceLocal;
  
NbKnapsack = 0;

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  if ( ContrainteKnapsack[Cnt] == IMPOSSIBLE ) continue;
	if ( NbTermTrav[Cnt] > NB_TERMES_FORCE_CALCUL_DE_K ) {
	  if ( fabs( VariableDuale[Cnt] ) < SEUIL_VARIABLE_DUALE_POUR_CALCUL_DE_COUPE ) continue;
	}
	Inf = IMPOSSIBLE;
	Sup = IMPOSSIBLE;	
	if ( ContrainteKnapsack[Cnt] == INF_ET_SUP_POSSIBLE ) {
	  Inf = INF_POSSIBLE;
		Sup = SUP_POSSIBLE;
	}
	else if ( ContrainteKnapsack[Cnt] == INF_POSSIBLE ) {
	  Inf = INF_POSSIBLE;
		Sup = IMPOSSIBLE;
	}
	else if ( ContrainteKnapsack[Cnt] == SUP_POSSIBLE ) {
	  Inf = IMPOSSIBLE;
		Sup = SUP_POSSIBLE;
	}  

	ilDeb = MdebTrav[Cnt];
	ilFin = ilDeb + NbTermTrav[Cnt];
	if ( Inf == INF_POSSIBLE ) {
		SecondMembre = BTrav[Cnt];
    NombreDeTermes = 0;
    il    = ilDeb;
    ilMax = ilFin;
    while ( il < ilMax ) { 
			if ( ATrav[il] == 0.0 ) goto Next1_Il;
      Var = NuvarTrav[il];
		  TypeBorne = TypeDeBorneTrav[Var];
      if ( TypeBorne == VARIABLE_FIXE ) {
			  SecondMembre -= ATrav[il] * UTrav[Var];				
				goto Next1_Il;
			}			
			if ( TypeDeVariableTrav[Var] == ENTIER ) {

        /* Attention a cause des bornes variables il faut verifier que la variable n'y est pas deja. Peut etre ameliore. */
				Found = 0;
				/*
				for ( i = 0 ; i < NombreDeTermes ; i++ ) {
				  if ( Variable[i] == Var ) {
					  Found = 1;
            Coeff[i] += ATrav[il];
						break;
					}
				}
				*/
        /* Fin borne sup variable */
			
	 	    if ( Found == 0 ) {	
          Coeff[NombreDeTermes] = ATrav[il];
			    Variable[NombreDeTermes] = Var;
				  NombreDeTermes++;
				}			
				goto Next1_Il;				
			}						
		  /* La variable n'est pas entiere */			
		  if ( ATrav[il] < 0.0 ) {			
		    /* Il faut monter la variable au max */
				
        /* S'il y a une borne sup variable on remplace la variable par sa borne sup variable */
	      if ( CntDeBorneSupVariable != NULL && 0 ) {
          if ( CntDeBorneSupVariable[Var] >= 0 ) {
		        Cnt1 = CntDeBorneSupVariable[Var];
	          i = MdebTrav[Cnt1];
			      bBorne = BTrav[Cnt1];
			      SecondMembre -= ATrav[il] * bBorne ;								
		        VarBin = NuvarTrav[i];					
	          u = -ATrav[i];
						/* Peut etre ameliore */
						Found = 0;
						for ( i = 0 ; i < NombreDeTermes ; i++ ) {
						  if ( Variable[i] == VarBin ) {
							  Found = 1;
                Coeff[i] += ATrav[il] * u;
								break;
							}
						}
						if ( Found == 0 ) {
              Coeff[NombreDeTermes] = ATrav[il] * u;
			        Variable[NombreDeTermes] = VarBin;
				      NombreDeTermes++;
            }
						/*printf("Variable bound ajoute\n");*/						
				    goto Next1_Il;				
	        }
	      }
        /* Fin borne sup variable */
				
			  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			    SecondMembre -= ATrav[il] * UmaxTrav[Var];								
        }								
				else {
				  NombreDeTermes = 0;
					printf("2- Erreur dans la constitution des contraintes de sac a dos TypeBorne %d\n",TypeBorne);					
					break;
				}				
			}
		  else {
		    /* Il faut baisser la variable au min */
        /* S'il y a une borne sup variable on remplace la variable par sa borne sup variable */
        if ( CntDeBorneInfVariable != NULL && 0 ) {
          if ( CntDeBorneInfVariable[Var] >= 0 ) {
					  Cnt1 = CntDeBorneInfVariable[Var];
		        i = MdebTrav[Cnt1];
			      bBorne = -BTrav[Cnt1];
			      SecondMembre -= ATrav[il] * bBorne ;								
		        VarBin = NuvarTrav[i];
	          l = ATrav[i];
						/* Peut etre ameliore */
						Found = 0;
						for ( i = 0 ; i < NombreDeTermes ; i++ ) {
						  if ( Variable[i] == VarBin ) {
							  Found = 1;
                Coeff[i] += ATrav[il] * l;
								break;
							}
						}						
						if ( Found == 0 ) {
              Coeff[NombreDeTermes] = ATrav[il] * l;
			        Variable[NombreDeTermes] = VarBin;
				      NombreDeTermes++;
            }
						/*printf("Variable bound ajoute\n");*/					
				    goto Next1_Il;				
	        } 		
        }
        /* Fin borne inf variable */
				
			  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
			    SecondMembre -= ATrav[il] * UminTrav[Var];				
        }								
				else {
				  NombreDeTermes = 0;
					printf("4- Erreur dans la constitution des contraintes de sac a dos TypeBorne %d\n",TypeBorne);					
					break;
				}								
			}			
			Next1_Il:
      il++; 
    }  
    /* Recherche couverture de sac a dos */		
		if ( NombreDeTermes >= MIN_TERMES_POUR_KNAPSACK ) {
		  /*printf("Recherche Knapsack simple\n");*/
			RendreLesCoeffsEntiers = OUI_PNE;
		  CouvertureTrouvee = NON_PNE;						
      PNE_GreedyCoverKnapsack( Pne, 0, NombreDeTermes, Variable, Coeff, SecondMembre, RendreLesCoeffsEntiers, &CouvertureTrouvee,
															 Mixed_0_1_Knapsack, 0.0, 0, NULL, NULL, NULL );				
      if ( CouvertureTrouvee == OUI_PNE ) {
        /*
			  printf("   K trouvee !!!!!!!!!!!!!!\n");
			  */
			  NbKnapsack++;
			}
		}
		/* Si couverture pas trouvee */
		/* On essaie maintenant de remplacer les variables de cette contrainte par d'autres variables en combinant
			 les contraintes */
    if ( /*CouvertureTrouvee == NON_PNE &&*/ NbTermTrav[Cnt] >= MIN_TERMES_POUR_KNAPSACK
		     && KNAPSACK_SUR_COMBINAISONS_DE_CONTRAINTES == OUI_PNE ) {
		  /* Attention: Coeff et Variable sont detruits dans GreedyCoverKnapsack */
		  SecondMembre = BTrav[Cnt];
      NombreDeTermes = 0;
      il    = ilDeb;
      ilMax = ilFin;
      while ( il < ilMax ) { 
			  if ( ATrav[il] == 0.0 ) goto Next12_Il;
        Var = NuvarTrav[il];
		    TypeBorne = TypeDeBorneTrav[Var];
        if ( TypeBorne == VARIABLE_FIXE ) {
			    SecondMembre -= ATrav[il] * UTrav[Var];				
				  goto Next12_Il;
			  }			
			  if ( TypeDeVariableTrav[Var] == ENTIER ) {
          Coeff[NombreDeTermes] = ATrav[il];
			    Variable[NombreDeTermes] = Var;
				  NombreDeTermes++;
				  goto Next12_Il;				
			  }
		    /* La variable n'est pas entiere */
		    if ( ATrav[il] < 0.0 ) {			  
		      /* Il faut mettre la variable au max */
			    if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			      SecondMembre -= ATrav[il] * UmaxTrav[Var];								
          }										
				  else {
				    NombreDeTermes = 0;
					  break;
				  }				
			  }
		    else {
		      /* Il faut monter la variable au min */
			    if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
			      SecondMembre -= ATrav[il] * UminTrav[Var];				
          }									
				  else {
				    NombreDeTermes = 0;
					  break;
				  }
				}
			  Next12_Il:
        il++; 
      }
      PNE_KnapsackSurCombinaisonsDeContraintes( Pne, Cnt, NombreDeTermes, Variable, Coeff, SecondMembre,
			                                          ContrainteDejaUtilisee, VariableSupprimee);
		} 
		
	}
	if ( Sup == SUP_POSSIBLE ) {
		SecondMembre = BTrav[Cnt];
    NombreDeTermes = 0;
    il    = ilDeb;   
    ilMax = ilFin;
    while ( il < ilMax ) { 
			if ( ATrav[il] == 0.0 ) goto Next2_Il;
      Var = NuvarTrav[il];
		  TypeBorne = TypeDeBorneTrav[Var];
      if ( TypeBorne == VARIABLE_FIXE ) {
			  SecondMembre -= ATrav[il] * UTrav[Var];
				goto Next2_Il;
			}			
			if ( TypeDeVariableTrav[Var] == ENTIER ) {
        Coeff[NombreDeTermes] = ATrav[il];
			  Variable[NombreDeTermes] = Var;
				NombreDeTermes++;
				goto Next2_Il;
			}
		  /* La variable n'est pas entiere */			
		  if ( ATrav[il] < 0.0 ) {			  
		    /* Il faut mettre la variable au max */
			  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
			    SecondMembre -= ATrav[il] * UminTrav[Var];								
        }								
				else {
				  NombreDeTermes = 0;
					printf("6- Erreur dans la constitution des contraintes de sac a dos TypeBorne %d echec mise au max ATrav %e\n",TypeBorne,ATrav[il]);												
					break;
				}				
			}
		  else {
		    /* Il faut mettre la variable au max */
			  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			    SecondMembre -= ATrav[il] * UmaxTrav[Var];				
        }								
				else {
				  NombreDeTermes = 0;
					printf("8- Erreur dans la constitution des contraintes de sac a dos TypeBorne %d echec mise au max ATrav %e\n",TypeBorne,ATrav[il]);												
					break;
				}								
			}						
			Next2_Il:
      il++;
    }
    /* Recherche couverture de sac a dos */				
		if ( NombreDeTermes >= MIN_TERMES_POUR_KNAPSACK ) {		
      SecondMembre *= -1.;
			for ( il = 0 ; il < NombreDeTermes ; il++ ) Coeff[il] *= -1.;
			RendreLesCoeffsEntiers = OUI_PNE;
		  CouvertureTrouvee = NON_PNE;		
      PNE_GreedyCoverKnapsack( Pne, 0, NombreDeTermes, Variable, Coeff, SecondMembre, RendreLesCoeffsEntiers, &CouvertureTrouvee,
															 Mixed_0_1_Knapsack, 0.0, 0, NULL, NULL, NULL );							
      if ( CouvertureTrouvee == OUI_PNE ) {
			  /*printf("   K trouvee !!!!!!!!!!!!!!\n");*/
        NbKnapsack++;
			}
		}		
	}
}
/*
if ( NbKnapsack != 0 || 1 ) printf("CoverKnapsackSimple : %d\n",NbKnapsack);
*/

free( ContrainteDejaUtilisee );
free( VariableSupprimee );

return;
}

