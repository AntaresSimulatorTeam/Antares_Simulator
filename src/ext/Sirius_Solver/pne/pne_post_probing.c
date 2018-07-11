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

   FONCTION: En fin de probing, si des variables binaires ont ete fixees
	           on regarde si des contraintes sont de venue des contraintes
						 de type "forcing constraints".
						 Plus tard on regardera si des contraintes ne sont pas devenues
						 redondantes afin de les eliminer.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# include "prs_define.h"
# include "prs_fonctions.h"

# define TRACES 0

# define EPSILON_FORCING_CONSTRAINT 1.e-8 /*1.e-7*/
# define FORCING_BMIN 1
# define FORCING_BMAX 2
# define PAS_DE_FORCING 128

# define MARGE_DINFAISABILITE  1.e-7

# define TEST_DES_CONTRAINTES_A_UNE_VARIABLE  OUI_PNE /*OUI_PNE*/
# define EPS 1.e-3 /*1.e-3*/

# define REBOUCLAGE OUI_PNE /*OUI_PNE*/

/*----------------------------------------------------------------------------*/

void PNE_PostProbing( PROBLEME_PNE * Pne )
{
int Cnt; int NombreDeContraintes; int il; int ilMax; int Var; char SminValide;
char SmaxValide; int * TypeDeVariable; int Nb; double Smin; double Smax; double a;
double b; double NouvelleValeur; int * TypeDeBorne; int * Mdeb; int * NbTerm;
int * Nuvar; double * X; double * Xmin; double * Xmax; double * B; double * A;
char * SensContrainte; char SensCnt; char TypeDeForcing; char TypeBorne;
int DerniereContrainte; int * CorrespondanceCntPneCntEntree; double Ai; int NbCntElim;
int NbVarFixees; int TypeBrn; double * CoutLineaire; char Signe; int NombreDeVariables; 
char SuppressionPossible; char DernierSigneTrouve; char SupprimerLaVariable;
int * Cdeb; int * Csui; int * NumContrainte; char SupprimerLaContrainte;
int Var1; double S; double Xi; double Xs; int NbIt; char Reboucler; 

if ( Pne->FaireDuPresolve == NON_PNE ) return;

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

NombreDeContraintes = Pne->NombreDeContraintesTrav;
X = Pne->UTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
CorrespondanceCntPneCntEntree = Pne->CorrespondanceCntPneCntEntree;

NbCntElim = 0;
NbVarFixees = 0;
NbIt = 0;

Debut:

NbIt++;
Reboucler = NON_PNE;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  SminValide = OUI_PNE;
	SmaxValide = OUI_PNE;
  Smin = 0.;
  Smax = 0.;
	SensCnt = SensContrainte[Cnt];
	b = B[Cnt];
	TypeDeForcing = PAS_DE_FORCING;
	Nb = 0;
	SupprimerLaContrainte = NON_PNE;
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
	  Var = Nuvar[il];
		TypeBorne = TypeDeBorne[Var];
		if ( TypeBorne == VARIABLE_NON_BORNEE ) {
      SminValide = NON_PNE;
	    SmaxValide = NON_PNE;
		  goto FinAnalyseContrainte;		
		}
		a = A[il];
		
	  if ( TypeBorne == VARIABLE_FIXE ) {
      Smin += a * X[Var];
      Smax += a * X[Var];
		}
		else {		
		  Nb++; 
	    if ( a > 0.0 ) {
		    /* Calcul de min */
				if ( SminValide == OUI_PNE ) {
				  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {				
            Smin += a * Xmin[Var];
					}
		      else {
            SminValide = NON_PNE;
			      if ( SmaxValide == NON_PNE ) break;						
					}
		    }
		    /* Calcul de max */
				if ( SmaxValide == OUI_PNE ) {
				  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {				
            Smax += a * Xmax[Var];
					}
		      else {
            SmaxValide = NON_PNE;
			      if ( SminValide == NON_PNE ) break;
					}
		    }
	    }
      else {
		    /* Calcul de min */
				if ( SminValide == OUI_PNE ) {
				  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {				
            Smin += a * Xmax[Var];
					}
		      else {
            SminValide = NON_PNE;
			      if ( SmaxValide == NON_PNE ) break;					 
					}
				}
		    /* Calcul de max */
				if ( SmaxValide == OUI_PNE ) {
				  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {				
            Smax += a * Xmin[Var];
					}
		      else {
            SmaxValide = NON_PNE;
			      if ( SminValide == NON_PNE ) break;
					}
				}
			}					
		}		
    il++;
  }
	if ( Nb == 0 ) {
		SupprimerLaContrainte = OUI_PNE;
	  goto FinAnalyseContrainte;
	}
		
	if ( SensCnt == '<' ) {
		if ( SmaxValide == OUI_PNE ) {
		  if ( Smax <= b + MARGE_DINFAISABILITE ) {
			  # if TRACES == 1
			    printf("Contrainte %d redondante Smx = %e < %e   Smin %e SminValide %d\n",Cnt,Smax,b,Smin,SminValide);
        # endif
	      SupprimerLaContrainte = OUI_PNE;								
			}
		}
	}	
  if ( SminValide == OUI_PNE ) {	
	  if ( fabs( Smin - b ) < EPSILON_FORCING_CONSTRAINT ) {
		  # if TRACES == 1
	 	    if ( SensCnt == '<' ) printf("Forcing constraint pendant le variable probing sur contrainte d'inegalite %d\n",Cnt);
		    else printf("Forcing constraint pendant le variable probing sur contrainte d'egalite %d\n",Cnt);
        printf("Bmin %e B %e sens %c\n",Smin,b,SensCnt);
      # endif			
		  TypeDeForcing = FORCING_BMIN;
	    SupprimerLaContrainte = OUI_PNE;			
		}
	}
  else if ( SmaxValide == OUI_PNE ) {
    if ( SensCnt == '=' ) {		
	    if ( fabs( Smax - b ) < EPSILON_FORCING_CONSTRAINT ) {
		    # if TRACES == 1
			    printf("Forcing constraint pendant le variable probing sur contrainte d'egalite %d\n",Cnt);          
          printf("Bmax %e B %e sens %c\n",Smax,b,SensCnt);
			  # endif
			  TypeDeForcing = FORCING_BMAX;
	      SupprimerLaContrainte = OUI_PNE;			
      }
	  }				
  }
  if ( TypeDeForcing != FORCING_BMIN && TypeDeForcing != FORCING_BMAX ) goto FinAnalyseContrainte;

	# if TRACES == 1
	  if ( TypeDeForcing == FORCING_BMIN ) printf("Contraintes %d FORCING_BMIN\n",Cnt);
	  if ( TypeDeForcing == FORCING_BMAX ) printf("Contraintes %d FORCING_BMAX\n",Cnt); 
	# endif
	
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Nuvar[il];
		TypeBorne = TypeDeBorne[Var];
		if ( TypeBorne == VARIABLE_FIXE ) goto NextIlDoForcing; 			 
    a = A[il];
	  if ( a == 0.0 ) goto NextIlDoForcing;
	  if ( TypeDeForcing == FORCING_BMIN ) {
      if ( a > 0.0 ) {
		    # if TRACES == 1
		      printf("Forcing constraint contrainte %d variables %d a fixer au min %e\n",Cnt,Var,Xmin[Var]);		
			  # endif
			  NouvelleValeur = Xmin[Var];
		  }
		  else {
		    # if TRACES == 1
		      printf("Forcing constraint contrainte %d variables %d a fixer au max %e\n",Cnt,Var,Xmax[Var]);
		    # endif
        NouvelleValeur = Xmax[Var];
		  }
	  }
	  else { /* TypeForcing = FORCING_BMAX */
      if ( a > 0.0 ) {
		    # if TRACES == 1
		      printf("Forcing constraint contrainte %d variables %d a fixer au max %e\n",Cnt,Var,Xmax[Var]);
			  # endif
        NouvelleValeur = Xmax[Var];
		  }
		  else {
		    # if TRACES == 1
		      printf("Forcing constraint contrainte %d variables %d a fixer au min %e\n",Cnt,Var,Xmin[Var]);	
		    # endif
		    NouvelleValeur = Xmin[Var];			
		  }
	  }
	  /* Si la variable a ete fixee on met a jour la liste des contraintes a examiner au prochain coup */
  	X[Var] = NouvelleValeur;      
	  Xmin[Var] = NouvelleValeur;
	  Xmax[Var] = NouvelleValeur;
	  TypeDeBorne[Var] = VARIABLE_FIXE;		
	  TypeDeVariable[Var] = REEL;
		NbVarFixees++;
	  NextIlDoForcing:
    il++;
  }

	FinAnalyseContrainte:
			
	if ( SupprimerLaContrainte == OUI_PNE ) {
	
		/* On inverse avec la derniere contrainte */
		
    Pne->NumeroDesContraintesInactives[Pne->NombreDeContraintesInactives] = CorrespondanceCntPneCntEntree[Cnt];
    Pne->NombreDeContraintesInactives++;

 	  DerniereContrainte = NombreDeContraintes - 1;				
 	  Mdeb[Cnt]   = Mdeb[DerniereContrainte];
    NbTerm[Cnt] = NbTerm [DerniereContrainte];				
    B[Cnt]      = B[DerniereContrainte];
    SensContrainte[Cnt] = SensContrainte[DerniereContrainte];
    CorrespondanceCntPneCntEntree[Cnt] = CorrespondanceCntPneCntEntree[DerniereContrainte];
    Pne->ChainageTransposeeExploitable = NON_PNE;
	  Cnt--;
	  NombreDeContraintes--;
	  Pne->NombreDeContraintesTrav = NombreDeContraintes;
	  NbCntElim++;
	}
					
}

# if TEST_DES_CONTRAINTES_A_UNE_VARIABLE == OUI_PNE 
/* Contraintes a une variable */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  S = 0.;
	Nb = 0;
	SupprimerLaContrainte = NON_PNE;
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
	Ai = 1.;
	Var1 = -1;
  while ( il < ilMax ) {
	  Var = Nuvar[il];
		TypeBorne = TypeDeBorne[Var];
		a = A[il];
	  if ( TypeBorne == VARIABLE_FIXE ) { S += a * X[Var]; goto NextElement; }
		else if ( Xmin[Var] == Xmax[Var] ) { S += a * Xmin[Var]; goto NextElement; }
    else {
  	  Nb++;
		  if ( Nb > 1 ) goto Synthese;
		  Var1 = Var;
		  Ai = A[il];
		}
		NextElement:
    il++;
  }
	Synthese:
	if ( Var1 < 0 ) continue;
	if ( Nb == 0 ) {
	  # if TRACES == 1 
		  printf("Contrainte %d sans variable \n",Cnt);
		# endif		
		SupprimerLaContrainte = OUI_PNE;
	}
  if ( Nb == 1 ) {
	  Var = Var1;
		TypeBorne = TypeDeBorne[Var];		
    S = B[Cnt] - S;
    if ( SensContrainte[Cnt] == '=' ) {
			NouvelleValeur = S / Ai;
      if ( TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT || TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES ) { 
        if ( NouvelleValeur < Xmin[Var] - SEUIL_DADMISSIBILITE ) {
			    Pne->YaUneSolution = PROBLEME_INFAISABLE; return;
        }
      }
      else if ( TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT || TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES ) { 
        if ( NouvelleValeur > Xmax[Var] + SEUIL_DADMISSIBILITE ) { 
          Pne->YaUneSolution = PROBLEME_INFAISABLE; return;
        }
      }
		  if ( TypeDeVariable[Var] == ENTIER ) {
		    if ( fabs( NouvelleValeur ) > MARGE_DINFAISABILITE && fabs( 1 - NouvelleValeur ) > MARGE_DINFAISABILITE ) {			
			    Pne->YaUneSolution = PROBLEME_INFAISABLE; return;
			  }
      }
			/* On fixe la variable */
				
			# if TRACES == 1
			  if ( TypeDeVariable[Var] == ENTIER ) {
		      printf("Post probing contrainte d'egalite a une seule variable: on fixe la variable entiere %d a %e (Xmin = %e Xmax = %e)\n",Var,NouvelleValeur,Xmin[Var],Xmax[Var]);
				}
				else {
		      printf("Post probing contrainte d'egalite a une seule variable: on fixe la variable %d a %e (Xmin = %e Xmax = %e)\n",Var,NouvelleValeur,Xmin[Var],Xmax[Var]);
				}
			# endif
					  						
  	  X[Var] = NouvelleValeur;      
	    Xmin[Var] = NouvelleValeur;
	    Xmax[Var] = NouvelleValeur;
	    TypeDeBorne[Var] = VARIABLE_FIXE;
	    TypeDeVariable[Var] = REEL;
		  NbVarFixees++;
		  SupprimerLaContrainte = OUI_PNE;			
		  Reboucler = OUI_PNE;			
    }		
    else {
		  /* La contrainte est de sens < */
			/* Attention, on peut fixer la variable si elle est entiere */
	    if ( Ai > 0 ) {
			  Xs = S / Ai;
				/* Eventuellement on change la borne sup */
        if ( Xs < Xmax[Var] - EPS ) {
				
				  # if TRACES == 1
					  printf("Post probing contrainte d'inegalite a une seule variable: Xmax[%d] = %e devient %e (Xmin = %e)\n",Var,Xmax[Var],Xs,Xmin[Var]);
					# endif
																	
          Xmax[Var] = Xs;
		      SupprimerLaContrainte = OUI_PNE;			
		      Reboucler = OUI_PNE;					
          if ( TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) TypeBorne = VARIABLE_BORNEE_DES_DEUX_COTES;
					else if ( TypeBorne == VARIABLE_NON_BORNEE ) TypeBorne = VARIABLE_BORNEE_SUPERIEUREMENT;

					if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES ) {
            if ( TypeDeVariable[Var] == ENTIER ) {
              /* On fixe la variable a Xmin */
 	            X[Var] = Xmin[Var];      
	            Xmax[Var] = Xmin[Var];
	            TypeBorne = VARIABLE_FIXE;
	            TypeDeVariable[Var] = REEL;
		          NbVarFixees++;												
						}						
					  else if ( fabs( Xmax[Var] - Xmin[Var] ) < ZERO_VARFIXE ) {						
						  /* On fixe la variable */
 	            X[Var] = Xs;      
	            Xmin[Var] = Xs;
	            TypeBorne = VARIABLE_FIXE;
	            TypeDeVariable[Var] = REEL;
		          NbVarFixees++;													
						}
					}
					TypeDeBorne[Var] = TypeBorne;
				}		 			
			}
	    else {			
			  Xi = -S / fabs( Ai ); 								 
				/* Eventuellement on change la borne inf */
        if ( Xi > Xmin[Var] + EPS ) {
				
				  # if TRACES == 1
					  printf("Post probing contrainte d'inegalite a une seule variable: Xmin[%d] = %e devient %e (Xmax = %e)\n",Var,Xmin[Var],Xi,Xmax[Var]);
					# endif
									
          Xmin[Var] = Xi;				
		      SupprimerLaContrainte = OUI_PNE;			
		      Reboucler = OUI_PNE;
          if ( TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) TypeBorne = VARIABLE_BORNEE_DES_DEUX_COTES;
          else if ( TypeBorne == VARIABLE_NON_BORNEE ) TypeBorne = VARIABLE_BORNEE_INFERIEUREMENT;

					if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES ) {
           if ( TypeDeVariable[Var] == ENTIER ) {
              /* On fixe la variable a Xmax */
 	            X[Var] = Xmax[Var];      
	            Xmin[Var] = Xmax[Var];
	            TypeBorne = VARIABLE_FIXE;
	            TypeDeVariable[Var] = REEL;
		          NbVarFixees++;						 
					 }				
					  else if ( fabs( Xmax[Var] - Xmin[Var] ) < ZERO_VARFIXE ) {
						  /* On fixe la variable */
 	            X[Var] = Xi;      
	            Xmax[Var] = Xi;
	            TypeBorne = VARIABLE_FIXE;
	            TypeDeVariable[Var] = REEL;
		          NbVarFixees++;													
						}
					}					
					TypeDeBorne[Var] = TypeBorne;
					
        }		
	    }
		}
	}
	if ( SupprimerLaContrainte == OUI_PNE ) {
	
		/* On inverse avec la derniere contrainte */
		
    Pne->NumeroDesContraintesInactives[Pne->NombreDeContraintesInactives] = CorrespondanceCntPneCntEntree[Cnt];
    Pne->NombreDeContraintesInactives++;

 	  DerniereContrainte = NombreDeContraintes - 1;				
 	  Mdeb[Cnt]   = Mdeb[DerniereContrainte];
    NbTerm[Cnt] = NbTerm [DerniereContrainte];				
    B[Cnt]      = B[DerniereContrainte];
    SensContrainte[Cnt] = SensContrainte[DerniereContrainte];
    CorrespondanceCntPneCntEntree[Cnt] = CorrespondanceCntPneCntEntree[DerniereContrainte];
    Pne->ChainageTransposeeExploitable = NON_PNE;
	  Cnt--;
	  NombreDeContraintes--;
	  Pne->NombreDeContraintesTrav = NombreDeContraintes;
	  NbCntElim++;		
	}	
}
# endif

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );
 
if ( NbCntElim == 0 ) goto Fin;  

CoutLineaire = Pne->LTrav;
NombreDeVariables = Pne->NombreDeVariablesTrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {

  TypeBrn = TypeDeBorne[Var];
  if ( TypeBrn == VARIABLE_FIXE ) continue;

  if ( CoutLineaire[Var] > 0.0 )      Signe = '+';
  else if ( CoutLineaire[Var] < 0.0 ) Signe = '-';
  else Signe = '|';

  SuppressionPossible = OUI_PNE;
  DernierSigneTrouve  = '|';

  il = Cdeb[Var];
  while ( il >= 0 ) {
    Ai  = A[il];
    if ( Ai == 0.0 ) goto ContrainteSuivante;
    Cnt = NumContrainte[il];
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
      if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {			
			  # if TRACES == 1
          printf("Variable %d cout %e fixee a sa borne inf: %e \n",Var,CoutLineaire[Var],Xmin[Var]);
			  # endif
        SupprimerLaVariable = OUI_PNE;							
			  NouvelleValeur = Xmin[Var];				     
			}			
    }
    else if ( Signe == '-' ) {        
      if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {						
			  # if TRACES == 1
          printf("Variable %d cout %e fixee a sa borne sup: %e \n",Var,CoutLineaire[Var],Xmax[Var]);
			  # endif   			
        SupprimerLaVariable = OUI_PNE;						
			  NouvelleValeur = Xmax[Var];				     
			}
    }
    else { /* Alors Signe = '|' ) */		
      if ( DernierSigneTrouve == '+' ) {
        /* La variable n'intervient que dans des contraintes d'inegalite, avec un coefficient positif et 
           de plus elle n'a pas de cout => on la fixe a Xmin */       
				if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {        
				  # if TRACES == 1
            printf("Variable %d cout %e fixee a sa borne inf: %e \n",Var,CoutLineaire[Var],Xmin[Var]);
          # endif				
          SupprimerLaVariable = OUI_PNE;
			    NouvelleValeur = Xmin[Var];				     					
				}				
      }
      else if ( DernierSigneTrouve == '-' ) {			
				if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
				  # if TRACES == 1
            printf("Variable %d cout %e fixee a sa borne sup: %e \n",Var,CoutLineaire[Var],Xmax[Var]);
          # endif 				
				  SupprimerLaVariable = OUI_PNE;
			    NouvelleValeur = Xmax[Var];				     					
				}				
      }
			else {						
        /* DernierSigneTrouve = '|' : la variable n'intervient pas dans les contraintes et son cout est nul */
				# if TRACES == 1
          printf("Variable %d a cout nul et n'intervenant dans aucune contrainte\n",Var);
        # endif				
			}
    }
    if ( SupprimerLaVariable == OUI_PNE ) {		
			# if TRACES == 1
			  printf("Suppression de la variable %d\n",Var);
      # endif			
  	  X[Var] = NouvelleValeur;      
	    Xmin[Var] = NouvelleValeur;
	    Xmax[Var] = NouvelleValeur;
	    TypeDeBorne[Var] = VARIABLE_FIXE;
	    TypeDeVariable[Var] = REEL;
		  NbVarFixees++;									
    }
  }
}

Fin:

if ( Reboucler == OUI_PNE ) {
  NbIt++;
	if ( NbIt < 5 && REBOUCLAGE == OUI_PNE ) {
	  goto Debut;
	}
}

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( NbCntElim != 0 || NbVarFixees != 0 ) {
    printf("Post probing: %d constraint(s) removed - %d variable(s) fixed\n",NbCntElim,NbVarFixees);
	}
}

# if PRISE_EN_COMPTE_DES_GROUPES_DE_VARIABLES_EQUIVALENTS == OUI_PNE
  PNE_ColonnesColineaires( Pne );
# endif

return;
}
