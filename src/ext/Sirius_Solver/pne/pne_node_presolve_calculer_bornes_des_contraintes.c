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

   FONCTION: Appele par le node presolve, on y calcule les min et max
	           des contraintes au noeud de l'arbre.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
  
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define ZERO 1.e-10

# define TRACES 0												
																			
/*----------------------------------------------------------------------------*/

void PNE_NodePresolveInitBornesDesContraintes( PROBLEME_PNE * Pne, int * Faisabilite ) 
{
double * Bmin; double * Bmax; char * BminValide; char * BmaxValide; int NombreDeContraintes;
NOEUD * Noeud; BB * Bb; int i; double * UminSv; double * UmaxSv; 
int * NumeroDeLaVariableModifiee; char * TypeDeBorneModifiee; double * NouvelleValeurDeBorne;
int Var; double ValeurDeVar; int * Cdeb; int * Csui; double * A; int * NumContrainte;
char BorneMiseAJour; int Cnt; char * SensContrainte; double * B; int Nb; int il;
int ilMax; int * Nuvar; int * Mdeb; int * NbTerm; int * TypeDeBorne; 
int NombreDeVariablesInstanciees; int * IndicesDesVariablesInstanciees;
char * VariableAZeroOuAUn; int Var1; double * Umin; double * Umax;
char * BorneInfConnue; char * BorneSupConnue; char UneVariableAEteFixee;
						
Bb = Pne->ProblemeBbDuSolveur;
Noeud = Bb->NoeudEnExamen;
  
NombreDeVariablesInstanciees = Noeud->NombreDeVariablesEntieresInstanciees,
IndicesDesVariablesInstanciees = Noeud->IndicesDesVariablesEntieresInstanciees,
VariableAZeroOuAUn = Noeud->ValeursDesVariablesEntieresInstanciees,

NombreDeContraintes = Pne->NombreDeContraintesTrav;

Cdeb = Pne->CdebTrav;   
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;
SensContrainte = Pne->SensContrainteTrav;   
B = Pne->BTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
Mdeb = Pne->MdebTrav;    
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;

Bmin = Pne->ProbingOuNodePresolve->Bmin;
Bmax = Pne->ProbingOuNodePresolve->Bmax;
/* On recupere les bornes des contraintes au noeud racine */
memcpy( (char *) Bmin, (char *) Pne->ProbingOuNodePresolve->BminSv, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) Bmax, (char *) Pne->ProbingOuNodePresolve->BmaxSv, NombreDeContraintes * sizeof( double ) );
BminValide = Pne->ProbingOuNodePresolve->BminValide;
BmaxValide = Pne->ProbingOuNodePresolve->BmaxValide;

/* On applique d'abord les restrictions de bornes connues au noeud */
UminSv = Pne->UminTravSv;
UmaxSv = Pne->UmaxTravSv;

Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;

NumeroDeLaVariableModifiee = Noeud->NumeroDeLaVariableModifiee;
TypeDeBorneModifiee  = Noeud->TypeDeBorneModifiee;
NouvelleValeurDeBorne = Noeud->NouvelleValeurDeBorne;

for( i = 0 ; i < Noeud->NombreDeBornesModifiees ; i++ ) {
  Var = NumeroDeLaVariableModifiee[i];
	if ( TypeDeBorneModifiee[i] == BORNE_INF ) {		
	  ValeurDeVar = NouvelleValeurDeBorne[i];
		BorneMiseAJour = MODIF_BORNE_INF;
	}
	else if ( TypeDeBorneModifiee[i] == BORNE_SUP ) {
	  ValeurDeVar = NouvelleValeurDeBorne[i];
		BorneMiseAJour = MODIF_BORNE_SUP;
	}
	else continue;
  /* On met a jour les contraintes dans lesquelles la variable intervient */
  PNE_NodePresolveMajBminBmax( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, UminSv, UmaxSv,
                               Var, ValeurDeVar, BorneMiseAJour );	
}

/* On applique les instanciations */
for ( i = 0 ; i < NombreDeVariablesInstanciees ; i++ ) {
  Var = IndicesDesVariablesInstanciees[i];	
	if ( VariableAZeroOuAUn[i] == '0' ) {		
	  ValeurDeVar = 0;
		BorneMiseAJour = MODIF_BORNE_SUP;
	}
	else if ( VariableAZeroOuAUn[i] == '1' ) {	
	  ValeurDeVar = 1;
		BorneMiseAJour = MODIF_BORNE_INF;  
	}
	else continue;
  /* On met a jour les contraintes dans lesquelles la variable intervient */
  PNE_NodePresolveMajBminBmax( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, UminSv, UmaxSv,
                               Var, ValeurDeVar, BorneMiseAJour );		
}

/* Pour toutes les instanciations on applique le graphe de conflits */
for ( i = 0 ; i < NombreDeVariablesInstanciees ; i++ ) {
  Var = IndicesDesVariablesInstanciees[i];	
	if ( VariableAZeroOuAUn[i] == '0' ) ValeurDeVar = 0;
	else if ( VariableAZeroOuAUn[i] == '1' ) ValeurDeVar = 1;
	else continue;
  PNE_NodePresolveGrapheDeConflit( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, UminSv,
	                                 UmaxSv, Umin, Umax, Var, ValeurDeVar, Faisabilite );
  if ( *Faisabilite == NON_PNE ) {
	  # if TRACES == 1 
		  printf("NodePresolveInitBornesDesContraintes pas de solution apres NodePresolveGrapheDeConflit\n");
		# endif
		return;
	}
}

/* On applique les contraintes de borne variable du variable probing */

# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE  
  PNE_NodePresolveAppliquerContraintesDeBornesVariables( Pne , Faisabilite );																		
  if ( *Faisabilite == NON_PNE ) {
	  # if TRACES == 1 
		  printf("NodePresolveInitBornesDesContraintes pas de solution apres application des contraintes de borne variable\n");
		# endif
		return;
	}  
# endif

/* Boucle sur toutes les contraintes pour mettre a jour les indicateurs d'activite de depart */
BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
BorneSupConnue = Pne->ProbingOuNodePresolve->BorneSupConnue;
UneVariableAEteFixee = NON_PNE;
BorneMiseAJour = NON_PNE;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( BminValide[Cnt] == NON_PNE && BmaxValide[Cnt] == NON_PNE ) continue;
  Nb = 0;
  Var1 = -1;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Nuvar[il];		
	  if ( A[il] == 0.0 ) goto NextElement;
		if ( TypeDeBorne[Var] == VARIABLE_FIXE ) goto NextElement;				
		if ( Umin[Var] == Umax[Var] ) goto NextElement;
  	Nb++;
		Var1 = Var;
		if ( Nb > 1 ) break;
	  NextElement:
	  il++;
  }
		
  if ( Nb == 0 ) {
	  /* Tous les termes de la contrainte sont fixes */
    if ( SensContrainte[Cnt] == '<' ) {
		  if ( Bmin[Cnt] > B[Cnt] + SEUIL_DADMISSIBILITE && BminValide[Cnt] == OUI_PNE ) {
	      # if TRACES == 1 
		      printf("Contrainte d'inegalite NodePresolveInitBornesDesContraintes Nb = 0 pas de solution Bmin[%d] = %e B = %e\n",Cnt,Bmin[Cnt],B[Cnt]);
		    # endif		
			  *Faisabilite = NON_PNE;				
			  return;
		  }
	  }
	  else {
	    /* Contrainte d'egalite */			
      if ( ( fabs( Bmax[Cnt] - Bmin[Cnt] ) > SEUIL_DADMISSIBILITE && BmaxValide[Cnt] == OUI_PNE && BminValide[Cnt] == OUI_PNE ) ||
			     ( fabs( Bmax[Cnt] - B[Cnt] ) > SEUIL_DADMISSIBILITE && BmaxValide[Cnt] == OUI_PNE ) ) {
	      # if TRACES == 1 
		      printf("Contrainte d'egalite NodePresolveInitBornesDesContraintes Nb = 0 pas de solution Bmin[%d] = %e Bmax[%d] = %e B = %e\n",Cnt,Bmin[Cnt],Cnt,Bmax[Cnt],B[Cnt]);
		    # endif							 
			  *Faisabilite = NON_PNE;				
			  return;			
		  }			
    }
  }
  else {
    if ( SensContrainte[Cnt] == '<' ) {
		  if ( BminValide[Cnt] == OUI_PNE && Bmin[Cnt] > B[Cnt] + SEUIL_DADMISSIBILITE ) {
	      # if TRACES == 1 
		      printf("NodePresolveInitBornesDesContraintes pas de solution Bmin[%d] = %e %c B = %e BminSv %e\n",
					        Cnt,Bmin[Cnt],SensContrainte[Cnt],B[Cnt],Pne->ProbingOuNodePresolve->BminSv[Cnt]);
          il = Mdeb[Cnt];
          ilMax = il + NbTerm[Cnt];
          while ( il < ilMax ) {
					  Var = Nuvar[il];
            printf(" Var = %d  Umin %e Umax %e A %e UminSv %e UmaxSv %e\n",Var,Umin[Var],Umax[Var],A[il],UminSv[Var],UmaxSv[Var]);
						il++;
					}				 
		    # endif					
			  *Faisabilite = NON_PNE;				
			  return;			
		  }			
	  }
		else {
		  /* Contrainte d'egalite */
	    if ( BmaxValide[Cnt] == OUI_PNE && Bmax[Cnt] < B[Cnt] - SEUIL_DADMISSIBILITE ) {
	      # if TRACES == 1 
		      printf("NodePresolveInitBornesDesContraintes pas de solution Bmax[%d] = %e B = %e\n",Cnt,Bmax[Cnt],B[Cnt]);
		    # endif				
			  *Faisabilite = NON_PNE;				
			  return;							
		  }
	    if ( BminValide[Cnt] == OUI_PNE && Bmin[Cnt] > B[Cnt] + SEUIL_DADMISSIBILITE ) {
	      # if TRACES == 1 
		      printf("NodePresolveInitBornesDesContraintes pas de solution Bmin[%d] = %e B = %e\n",Cnt,Bmin[Cnt],B[Cnt]);
		    # endif				
			  *Faisabilite = NON_PNE;				
			  return;				
		  }			
		}
  }

}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_NodePresolveMajBminBmax( PROBLEME_PNE * Pne,
                                  int * Cdeb, int * Csui, int * NumContrainte,
                                  double * A, double * Bmin, double * Bmax,
                                  double * UminSv, double * UmaxSv,
                                  int Var, double ValeurDeVar, char BorneMiseAJour )
{
int ic; int Cnt; double Ai; char * SensContrainte; char * BmaxValide; double * B;

SensContrainte = Pne->SensContrainteTrav;
BmaxValide = Pne->ProbingOuNodePresolve->BmaxValide;
B = Pne->BTrav;

ic = Cdeb[Var];
while ( ic >= 0 ) {
  Ai = A[ic];	
  Cnt = NumContrainte[ic];		
  if ( BorneMiseAJour == MODIF_BORNE_INF ) {
		/* C'est une modification de borne inf */		
		if ( Ai > 0 ) { Bmin[Cnt] -= Ai * UminSv[Var]; Bmin[Cnt] += Ai * ValeurDeVar; }
		else {
		  Bmax[Cnt] -= Ai * UminSv[Var];
			Bmax[Cnt] += Ai * ValeurDeVar;       
    }		
	}
	else {
		/* C'est une modification de borne sup */
		if ( Ai < 0 ) { Bmin[Cnt] -= Ai * UmaxSv[Var]; Bmin[Cnt] += Ai * ValeurDeVar; }
		else {
		  Bmax[Cnt] -= Ai * UmaxSv[Var];
			Bmax[Cnt] += Ai * ValeurDeVar;      			
		}		
	}			 		
  ic = Csui[ic];
}	
return;
}

/*----------------------------------------------------------------------------*/

void PNE_NodePresolveGrapheDeConflit( PROBLEME_PNE * Pne,
                                      int * Cdeb, int * Csui, int * NumContrainte,
																			double * A, double * Bmin, double * Bmax,
																			double * UminSv, double * UmaxSv,
																			double * Umin, double * Umax,
                                      int Var, double ValeurDeVar,
																			int * Faisabilite )
{
int Edge; int Noeud; int Complement; int Nv; int Pivot; int * First; int * Adjacent; int * Next;
char BorneMiseAJour;

if ( Pne->ConflictGraph == NULL ) return;

if ( *Faisabilite == NON_PNE ) return;

Pivot = Pne->ConflictGraph->Pivot;
First = Pne->ConflictGraph->First;
Adjacent = Pne->ConflictGraph->Adjacent;
Next = Pne->ConflictGraph->Next;

if ( ValeurDeVar == 1.0 ) { Noeud = Var; Complement = Pivot + Var; }
else { Noeud = Pivot + Var; Complement = Var; }

Edge = First[Noeud];
while ( Edge >= 0 ) {
  Nv = Adjacent[Edge];
	/* Attention a ne pas prendre le complement */
	if ( Nv == Complement ) goto NextEdge;
  if ( Nv < Pivot ) {
	  Var = Nv;
		/* On ne doit pas avoir X[Var] = 1.0 */
		if ( Umin[Var] > 0.0001 ) {
		  /*printf("Node probing Instanciation infaisable\n");*/
			*Faisabilite = NON_PNE;
			return;
		}		
		if ( Umin[Var] == Umax[Var] ) goto NextEdge;		
		ValeurDeVar = 0.0;
		BorneMiseAJour = MODIF_BORNE_SUP;
		/*
		printf("Node probing Variable %d fixee a %e  Umin %e  Umax %e\n",Var,ValeurDeVar,Umin[Var],Umax[Var]);
		*/
    /* On met a jour Bmin et Bmax */    
		PNE_NodePresolveMajBminBmax( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, UminSv, UmaxSv, 
		                             Var, ValeurDeVar, BorneMiseAJour );    
		/* On initialise les bornes */

		Umin[Var] = ValeurDeVar;
    Umax[Var] = ValeurDeVar;

    Pne->ProbingOuNodePresolve->BorneInfConnue[Var] = FIXATION_SUR_BORNE_INF;
    Pne->ProbingOuNodePresolve->BorneSupConnue[Var] = FIXATION_SUR_BORNE_INF;
		Pne->ProbingOuNodePresolve->ValeurDeBorneInf[Var] = ValeurDeVar;
    Pne->ProbingOuNodePresolve->ValeurDeBorneSup[Var] = ValeurDeVar;
		
    Pne->ProbingOuNodePresolve->NumeroDesVariablesFixees[Pne->ProbingOuNodePresolve->NombreDeVariablesFixees] = Var;
    Pne->ProbingOuNodePresolve->NombreDeVariablesFixees += 1;					
		
		/* On continue le graphe a partir de Var */    
		PNE_NodePresolveGrapheDeConflit( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax,
																		 UminSv, UmaxSv, Umin, Umax,
                                     Var, ValeurDeVar, Faisabilite );		
		if ( *Faisabilite == NON_PNE ) return;															 
	}
  else {
    /* La valeur borne inf est interdite pour la variable */
		/* On doit donc fixer la variable a Umax et fixer les voisins de ce noeud */
	  Var = Nv - Pivot;
		/* On ne doit pas avoir X[Var] = 0.0 */
		if ( Umax[Var] < 0.9999 ) {
		  /*printf("Node probing Instanciation infaisable\n");*/
			*Faisabilite = NON_PNE;
			return;
		}		
		if ( Umin[Var] == Umax[Var] ) goto NextEdge;		
		ValeurDeVar = 1.0;
		BorneMiseAJour = MODIF_BORNE_INF;
		/*
		printf("Node probing Variable %d fixee a %e  Umin %e  Umax %e\n",Var,ValeurDeVar,Umin[Var],Umax[Var]);
		*/
    /* On met a jour Bmin et Bmax */	  
    PNE_NodePresolveMajBminBmax( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, UminSv, UmaxSv, 
		                             Var, ValeurDeVar, BorneMiseAJour );    
		/* On initialise les bornes */
		Umin[Var] = ValeurDeVar;
    Umax[Var] = ValeurDeVar;

    Pne->ProbingOuNodePresolve->BorneInfConnue[Var] = FIXATION_SUR_BORNE_SUP;
    Pne->ProbingOuNodePresolve->BorneSupConnue[Var] = FIXATION_SUR_BORNE_SUP;
		Pne->ProbingOuNodePresolve->ValeurDeBorneInf[Var] = ValeurDeVar;
    Pne->ProbingOuNodePresolve->ValeurDeBorneSup[Var] = ValeurDeVar;
		
    Pne->ProbingOuNodePresolve->NumeroDesVariablesFixees[Pne->ProbingOuNodePresolve->NombreDeVariablesFixees] = Var;
    Pne->ProbingOuNodePresolve->NombreDeVariablesFixees += 1;			
						
		/* On continue le graphe a partir de Var */
    PNE_NodePresolveGrapheDeConflit( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax,
																		 UminSv, UmaxSv, Umin, Umax,
                                     Var, ValeurDeVar, Faisabilite );
		if ( *Faisabilite == NON_PNE ) return;				
	}
	NextEdge:
  Edge = Next[Edge];
}			

return;
}

/*----------------------------------------------------------------------------*/

# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE

void PNE_NodePresolveAppliquerContraintesDeBornesVariables( PROBLEME_PNE * Pne,
																			                      int * Faisabilite )
{
int Cnt; int * First;  int il; double B; int * Colonne; double * SecondMembre; 
double * Coefficient; double Xmin; double Xmax; double * Bmin; double * Bmax; 
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable; int NombreDeContraintesDeBorne;
char * BorneInfConnue; char * BorneSupConnue; double * ValeurDeBorneInf;
double * ValeurDeBorneSup; char BrnInfConnue; int VarBin; int VarCont; double A1;
int * Cdeb; int * Csui; int * NumContrainte; double * A; char BorneMiseAJour;
double Marge; char * LaContrainteDeBorneVariableEstDansLePool; double A2;
char Bvalide; double NouvelleValeur; char UneVariableAEteFixee;

return; /* Experimentalement on constate que ca consomme du temps et que ca n'apporte pas grand chose
           en terme de noeuds qui seront explores dans la recherche arborescente */  

if ( Pne->ContraintesDeBorneVariable == NULL ) return;

Marge = 1.e-6;

Bmin = Pne->ProbingOuNodePresolve->Bmin;
Bmax = Pne->ProbingOuNodePresolve->Bmax;

BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
BorneSupConnue = Pne->ProbingOuNodePresolve->BorneSupConnue;
ValeurDeBorneInf = Pne->ProbingOuNodePresolve->ValeurDeBorneInf;
ValeurDeBorneSup = Pne->ProbingOuNodePresolve->ValeurDeBorneSup;

ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;

NombreDeContraintesDeBorne = ContraintesDeBorneVariable->NombreDeContraintesDeBorne;
First = ContraintesDeBorneVariable->First;
LaContrainteDeBorneVariableEstDansLePool = ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool;
SecondMembre = ContraintesDeBorneVariable->SecondMembre;
Colonne = ContraintesDeBorneVariable->Colonne;
Coefficient = ContraintesDeBorneVariable->Coefficient;

Cdeb = Pne->CdebTrav;   
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;

/* Parcours des contraintes de bornes variable: la variable entiere est toujours classee en second */

for ( Cnt = 0 ; Cnt < NombreDeContraintesDeBorne ; Cnt++ ) {
  if ( LaContrainteDeBorneVariableEstDansLePool[Cnt] == OUI_PNE ) continue;
  if ( First[Cnt] < 0 ) continue;	
  il = First[Cnt];
	/* Variable continue */
	VarCont = Colonne[il];
  if ( ValeurDeBorneInf[VarCont] == ValeurDeBorneSup[VarCont] ) continue;
  A1 = Coefficient[il];
	
	/* Variable entiere */
	il++;
	VarBin = Colonne[il];
  BrnInfConnue = BorneInfConnue[VarBin];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF || BrnInfConnue == FIXATION_SUR_BORNE_SUP ) {
  	B = SecondMembre[Cnt] - ( Coefficient[il] * ValeurDeBorneInf[VarBin] );
    if ( A1 > 0 ) {
		  /* On peut etablir une borne sup */
			Xmax = B / A1;
			if ( Xmax < ValeurDeBorneSup[VarCont] ) {
			  if ( Xmax < ValeurDeBorneInf[VarCont] - Marge ) { *Faisabilite = NON_PNE; goto FinBornesVariable; }
			  /*
			  printf("Nouvelle valeur de borne sup pour la variable %d: %e -> %e\n",VarCont,ValeurDeBorneSup[VarCont],Xmax);
				*/			 
        /* On met a jour Bmin et Bmax */				
		    BorneMiseAJour = MODIF_BORNE_SUP;
        PNE_NodePresolveMajBminBmax( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, ValeurDeBorneInf, ValeurDeBorneSup,
				                             VarCont, Xmax, BorneMiseAJour );

        ValeurDeBorneSup[VarCont] =  Xmax;
        BorneSupConnue[VarCont] = OUI_PNE;		 			 
			         														 
			}
		}
		else {
		  /* On peut etablir une borne inf */
			Xmin = B / A1;
			if ( Xmin > ValeurDeBorneInf[VarCont] ) {
			  if ( Xmin > ValeurDeBorneSup[VarCont]  + Marge ) { *Faisabilite = NON_PNE; goto FinBornesVariable; }
			  /*
			  printf("Nouvelle valeur de borne inf pour la variable %d: %e -> %e\n",VarCont,ValeurDeBorneInf[VarCont],Xmin);
        */
        /* On met a jour Bmin et Bmax */				
		    BorneMiseAJour = MODIF_BORNE_INF;
        PNE_NodePresolveMajBminBmax( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, ValeurDeBorneInf, ValeurDeBorneSup,
				                             VarCont, Xmin, BorneMiseAJour );

        ValeurDeBorneInf[VarCont] =  Xmin;
        BorneInfConnue[VarCont] = OUI_PNE;
				
			}			
		}
	}
	else {
	  /* A l'inverse on regarde si une modification de borne de la variable continue permet de fixer la variable entiere */
    A2 = Coefficient[il];
		Bvalide = NON_PNE;
		BorneMiseAJour = NON_PNE;
    UneVariableAEteFixee = NON_PNE;
		NouvelleValeur = -1;  
    if ( A1 > 0 ) {
			if ( BorneInfConnue[VarCont] == OUI_PNE ) {
				B = SecondMembre[Cnt] - ( A1 * ValeurDeBorneInf[VarCont] );
				Bvalide = OUI_PNE;
			}
		}
		else if ( BorneSupConnue[VarCont] == OUI_PNE ) {
			B = SecondMembre[Cnt] - ( A1 * ValeurDeBorneSup[VarCont] );
			Bvalide = OUI_PNE;
		}
		if ( Bvalide == OUI_PNE ){
		  if ( A2 > 0.0 ) {
		    /* On peut calculer une borne sup de la variable entiere */
			  Xmax = B / A2;
				if ( Xmax < 1 - 1.e-6 ){
				  
					printf("fixation %d a 0 car %e\n",VarBin,Xmax); fflush( stdout );
					
					NouvelleValeur = ValeurDeBorneInf[VarBin];
				  UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;									
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, VarBin, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );					
				}
			}
	    else {
		    /* On peut calculer une borne inf de la variable entiere */
				Xmin = -B / fabs( A2 );
				if ( Xmin > 0 + 1.e-6 ) {
				  
					printf("fixation %d a 1 car %e\n",VarBin,Xmin); fflush( stdout );
					
					NouvelleValeur = ValeurDeBorneSup[VarBin];
				  UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;									
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, VarBin, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );					
				}
			}
    }
	}
}
FinBornesVariable:

return;
}

# endif

/*----------------------------------------------------------------------------*/
