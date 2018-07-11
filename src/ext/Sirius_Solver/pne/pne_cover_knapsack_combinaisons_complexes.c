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

   FONCTION: Recherche de couverture de sac a dos sur des combinaisons
	           un peu complexes de contraintes.
						 Remarque: on peu calculer les combinaisons une bonne fois
						 pour toutes. Idem pour les autres contraintes sur lesquelles
						 on fait un cover knapsack ou une MIR.

						 On part d'une contrainte qui a les proprietes suivantes:
						 - soit c'est une contrainte mixte
						 - soit c'est une contrainte en variables entieres dont
						   certains coeffs sont negatifs.

						 1- On essai de combiner la contrainte de depart avec les autres.
						    Chaque combinaison doit supprimer une variable continue mais
								la combinaison obtenue ne doit pas contenir plus de variables
								continues.
						 2- Pour chaque variable entiere a coeff negatif, on cherche
						    une combinaison qui fasse diminuer le nombre de signes
								negatifs et qui n'amene pas de variable continue.
						 On passe le resultat au generateur de coupes de knapsack.

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

void PNE_KnapsackSurCombinaisonsSupprimerUneVariableEntiere( PROBLEME_PNE * , int , double , int * , int * ,
																															double * , int * , double * , char *, char * );
void PNE_KnapsackSurCombinaisonsSupprimerUneVariableContinue( PROBLEME_PNE * , int , double , int * , int * ,
                                                              double * , int * , double * , char * , char * );
																															
/*----------------------------------------------------------------------------*/

void PNE_KnapsackSurCombinaisonsSupprimerUneVariableEntiere( PROBLEME_PNE * Pne,
                                                             int VarEnt,
																														 double CoeffVarEnt,
																														 int * NombreDeTermes,
																														 int * Variable,
																														 double * Coeff,
																														 int * Presence,
																														 double * SecondMembre,
																														 char * ContrainteDejaUtilisee,
	  																												 char * Echec )
{
double CoeffAi; double Rapport; int ic; int il; int ilMax;
int * CdebTrav; int * CsuiTrav; int * NumContrainteTrav; int * TypeDeVariableTrav;
int * MdebTrav; int * NbTermTrav; int * NuvarTrav; int Cnt; int Var; int ilDeb;
double * ATrav; char * SensContrainteTrav; double Sign;

*Echec = OUI_PNE;

CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
NumContrainteTrav = Pne->NumContrainteTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;

ATrav = Pne->ATrav;
MdebTrav = Pne->MdebTrav;    
NbTermTrav = Pne->NbTermTrav;
NuvarTrav = Pne->NuvarTrav;
SensContrainteTrav = Pne->SensContrainteTrav;

ic = CdebTrav[VarEnt];
while ( ic >= 0 ) {
  Cnt = NumContrainteTrav[ic];
	if ( ContrainteDejaUtilisee[Cnt] == OUI_PNE ) goto NextIc2;	
	if ( SensContrainteTrav[ic] == '<' ) {
    if ( ATrav[ic] >= 0.0 ) goto NextIc2;		
	}
  CoeffAi = ATrav[ic];
	Rapport = -CoeffVarEnt / CoeffAi;
	if ( Rapport > 0 ) Sign = 1.;
	else Sign = -1.;
  /* Contrainte candidate */
	ilDeb = MdebTrav[Cnt];
	il = ilDeb;
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax ) {
		Var = NuvarTrav[il];
		if ( Var == VarEnt ) goto NextIl2;
    if ( TypeDeVariableTrav[Var] != ENTIER ) goto NextIc2;
		if ( Sign * ATrav[il] < 0. && Presence[Var] < 0 ) goto NextIc2;
		NextIl2:
	  il++;
	}
  /* Si on est arrive jusque la c'est que la combinaison est acceptable: on fait
	   la combinaison */
	il = ilDeb;
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax ) {
		Var = NuvarTrav[il];
		if ( Presence[Var] >= 0 ) {
		  Coeff[Presence[Var]] += Rapport * ATrav[il];
		}
		else {
      Variable[*NombreDeTermes] = Var;
		  Coeff[*NombreDeTermes] = Rapport * ATrav[il];
			*NombreDeTermes = *NombreDeTermes + 1;			
		}	
	  il++;
  }
	*SecondMembre += Rapport * Pne->BTrav[Cnt];
	/* Suppression de VarEnt */
	if ( Presence[VarEnt] != *NombreDeTermes - 1 ) {
    il = Presence[VarEnt];
    Variable[il] = Variable[*NombreDeTermes-1];
	  Coeff[il] = Coeff[*NombreDeTermes-1];
	  Presence[Variable[*NombreDeTermes-1]] = il;
	  Presence[VarEnt] = -1;
	}
  *NombreDeTermes = *NombreDeTermes - 1;
	ContrainteDejaUtilisee[Cnt] = OUI_PNE;
	*Echec = NON_PNE;
	break;
  NextIc2:
  ic = CsuiTrav[ic];
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_KnapsackSurCombinaisonsSupprimerUneVariableContinue( PROBLEME_PNE * Pne,
                                                              int VarCont,
																															double CoeffVarCont,
																															int * NombreDeTermes,
																															int * Variable,
																															double * Coeff,
																															int * Presence,
																															double * SecondMembre,
																															char * ContrainteDejaUtilisee,
																															char * Echec )
{
double CoeffAi; double Rapport; int ic; int il; int ilMax;
int * CdebTrav; int * CsuiTrav; int * NumContrainteTrav; int * TypeDeVariableTrav;
int * MdebTrav; int * NbTermTrav; int * NuvarTrav; int Cnt; int Var; int ilDeb;
double * ATrav; char * SensContrainteTrav;

*Echec = OUI_PNE;

CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
NumContrainteTrav = Pne->NumContrainteTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;

ATrav = Pne->ATrav;
MdebTrav = Pne->MdebTrav;    
NbTermTrav = Pne->NbTermTrav;
NuvarTrav = Pne->NuvarTrav;
SensContrainteTrav = Pne->SensContrainteTrav;

ic = CdebTrav[VarCont];
while ( ic >= 0 ) {
  Cnt = NumContrainteTrav[ic];
	if ( ContrainteDejaUtilisee[Cnt] == OUI_PNE ) goto NextIc1;
  CoeffAi = ATrav[ic];
  if ( CoeffAi == 0.0 ) goto NextIc1;
	Rapport = -CoeffVarCont / CoeffAi;
	
	if ( SensContrainteTrav[Cnt] == '<' ) {
	  if ( Rapport < 0.0 ) goto NextIc1;
	}
  /* Contrainte candidate */
	ilDeb = MdebTrav[Cnt];
	il = ilDeb;
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax ) {
		Var = NuvarTrav[il];
		if ( Var == VarCont ) goto NextIl1;
    if ( TypeDeVariableTrav[Var] != ENTIER ) {
		  /* On suppose qu'il n'y a pas de "cancelation" numerique */
			/* La combinaison ajouterait une variable continue ? */
      if ( Presence[Var] < 0 ) goto NextIc1;
    }	
    NextIl1:
	  il++;
	}
  /* Si on est arrive jusque la c'est que la combinaison est acceptable: on fait
	   la combinaison */		 	
	Rapport = -CoeffVarCont / CoeffAi;

	if ( SensContrainteTrav[Cnt] == '<' && Rapport < 0.0 ) {
	  printf("Rapport %e SensContrainteTrav %c\n",Rapport,SensContrainteTrav[Cnt]);
		exit(0);
	}
	
	il = ilDeb;
	ilMax = il + NbTermTrav[Cnt];	
	while ( il < ilMax ) {
		Var = NuvarTrav[il];
		if ( Presence[Var] >= 0 ) {
		  Coeff[Presence[Var]] += Rapport * ATrav[il];
			/*printf("Modif coeff de variable %d -> %e\n",Var,Coeff[Presence[Var]]);*/
		}
		else {
      Variable[*NombreDeTermes] = Var;
		  Coeff[*NombreDeTermes] = Rapport * ATrav[il];
			Presence[Var] = *NombreDeTermes;
			/*printf("Creation coeff de variable %d -> %e\n",Var,Coeff[Presence[Var]]);*/
			*NombreDeTermes = *NombreDeTermes + 1;			
		}	
	  il++;
  }
	*SecondMembre += Rapport * Pne->BTrav[Cnt];

	if ( Coeff[Presence[VarCont]] != 0.0 ) {
	  printf("Coeff %e  Presence[VarCont] %d NombreDeTermes %d \n",Coeff[Presence[VarCont]],Presence[VarCont],*NombreDeTermes);
		exit(0);
	}	
	
	ContrainteDejaUtilisee[Cnt] = OUI_PNE;
	
	*Echec = NON_PNE;
	break;
  NextIc1:
  ic = CsuiTrav[ic];
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_KnapsackSurCombinaisonsComplexesDeContraintes( PROBLEME_PNE * Pne )
{ 
char Mixed_0_1_Knapsack; int Cnt; int il; int ilMax; int ilDeb; int NombreDeTermes;
int * TypeDeVariableTrav; char * ContrainteKnapsack; char * ContrainteMixte;
int * MdebTrav; int * NbTermTrav; int * NuvarTrav; double * ATrav; int * Variable;
double * Coeff; int * Presence; double * BTrav; double SecondMembre; char Echec;
int i; int Var; int TypeBorne; int * TypeDeBorneTrav; double * UTrav; double * UminTrav;
double * UmaxTrav; char CouvertureTrouvee; char RendreLesCoeffsEntiers;
char * ContrainteDejaUtilisee; char * Buffer; char * pt; int Index;

if ( Pne->ContrainteMixte == NULL ) return;	

/*
printf("KnapsackSurCombinaisonsComplexesDeContraintes  \n");
*/
i = 0;
i += Pne->NombreDeVariablesTrav * sizeof( int ); /* Variable */
i += Pne->NombreDeVariablesTrav * sizeof( double ); /* Coeff */
i += Pne->NombreDeVariablesTrav * sizeof( int ); /* Presence */
i += Pne->NombreDeContraintesTrav * sizeof( char ); /*ContrainteDejaUtilisee*/

Buffer = (char *) malloc( i * sizeof( char ) );

pt = Buffer;
Variable = (int *) pt;
pt += Pne->NombreDeVariablesTrav * sizeof( int );
Coeff = (double *) pt;
pt += Pne->NombreDeVariablesTrav * sizeof( double );
Presence = (int *) pt;
pt += Pne->NombreDeVariablesTrav * sizeof( int );
ContrainteDejaUtilisee = (char *) pt;
pt += Pne->NombreDeContraintesTrav * sizeof( char );

Mixed_0_1_Knapsack = NON_PNE;

UTrav = Pne->UTrav;
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;

TypeDeBorneTrav = Pne->TypeDeBorneTrav;
ContrainteKnapsack = Pne->ContrainteKnapsack;
ContrainteMixte = Pne->ContrainteMixte;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
BTrav = Pne->BTrav;
ATrav = Pne->ATrav;
MdebTrav = Pne->MdebTrav;    
NbTermTrav = Pne->NbTermTrav;
NuvarTrav = Pne->NuvarTrav;

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  if ( ContrainteKnapsack[Cnt] != INF_POSSIBLE ) continue;
  for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) Presence[i] = -1;
  for ( i = 0 ; i < Pne->NombreDeContraintesTrav ; i++ ) ContrainteDejaUtilisee[i] = NON_PNE;
	ilDeb = MdebTrav[Cnt];
	il = ilDeb;
	ilMax = il + NbTermTrav[Cnt];
  NombreDeTermes = 0;
	while ( il < ilMax ) {
	  if ( ATrav[il] != 0.0 ) {
      Variable[NombreDeTermes] = NuvarTrav[il];
		  Coeff[NombreDeTermes] = ATrav[il];
		  Presence[NuvarTrav[il]] = NombreDeTermes;
		  NombreDeTermes++;
		}
	  il++;
	}
	/*
	printf("Etude contrainte NombreDeTermes %d\n",NombreDeTermes);
	il = ilDeb;
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax ) {
    if ( TypeDeVariableTrav[NuvarTrav[il]] == ENTIER ) printf(" %e (i%d) ",ATrav[il],NuvarTrav[il]);
	  else printf(" %e (c%d)",ATrav[il],NuvarTrav[il]);	 
	  il++;
	}
	printf(" %c %e  ContrainteMixte %d \n",Pne->SensContrainteTrav[Cnt],Pne->BTrav[Cnt],ContrainteMixte[Cnt]);
	*/
	ContrainteDejaUtilisee[Cnt] = OUI_PNE;
	SecondMembre = BTrav[Cnt];
  if ( ContrainteMixte[Cnt] == OUI_PNE ) {
	  /* On essaie d'enlever les variables continues par des combinaisons */
	  il = ilDeb;
	  ilMax = il + NbTermTrav[Cnt];
	  while ( il < ilMax ) {
		  Var = NuvarTrav[il];
			if ( TypeDeVariableTrav[Var] == ENTIER ) goto NextIl;
			if ( Presence[Var] < 0 ) goto NextIl;
			if ( Coeff[Presence[Var]] == 0.0 ) goto NextIl;			
      PNE_KnapsackSurCombinaisonsSupprimerUneVariableContinue( Pne, Var,Coeff[Presence[Var]],
					                             &NombreDeTermes, Variable, Coeff, Presence, &SecondMembre,
																			 ContrainteDejaUtilisee, &Echec );
			if ( Echec == OUI_PNE ) {
			  /* Remarque: on pourrait tenter une MIR de Marchand Wolsey */
			  break;
			}
			else {
	      /* Suppression de Var */
	      if ( Presence[Var] != NombreDeTermes - 1 ) {
          Index = Presence[Var];
          Variable[Index] = Variable[NombreDeTermes-1];
	        Coeff[Index] = Coeff[NombreDeTermes-1];
	        Presence[Variable[NombreDeTermes-1]] = Index;
	      }
	      Presence[Var] = -1;
        NombreDeTermes = NombreDeTermes - 1;	
			}			
			NextIl:
			il++;
	  }
	}
	else continue;

	if ( Echec == OUI_PNE ) continue;
	
	/*
	printf("1- on arrive a NombreDeTermes %d\n",NombreDeTermes);
  for ( i = 0 ; i < NombreDeTermes ; i++ ) {
    Var = Variable[i];
    if ( TypeDeVariableTrav[Var] == ENTIER ) printf(" %e (i%d) ",Coeff[i],Var);
	  else printf(" %e (c%d) ",Coeff[i],Var);
	  il++;
	}
	printf(" < %e\n",SecondMembre);
	*/
	/* On essai d'enlever les signes negatifs sur les variables entieres de la contrainte d'origine */
	il = ilDeb;
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax && 0 ) {		  
    if ( TypeDeVariableTrav[NuvarTrav[il]] == ENTIER ) {
			if ( ATrav[il] < 0.0 ) {
			  /*printf("On essai de supprimer le terme %d   %e\n",NuvarTrav[il],ATrav[il]);*/
        PNE_KnapsackSurCombinaisonsSupprimerUneVariableEntiere( Pne, NuvarTrav[il], ATrav[il],
																	 	 &NombreDeTermes, Variable, Coeff, Presence, &SecondMembre,
	  																 ContrainteDejaUtilisee, &Echec );														 
				if ( Echec == OUI_PNE ) {
					/* Peut etre s'assurer qu'on a eu au moins une reussite ou bien que le remplacement
					   d'une variable continue a reussi */
					/*continue;*/
				}
			}
		}
	 	il++;
	}
  /*
	printf("2- on arrive a NombreDeTermes %d\n",NombreDeTermes);
  for ( i = 0 ; i < NombreDeTermes ; i++ ) {
    Var = Variable[i];
    if ( TypeDeVariableTrav[Var] == ENTIER ) printf(" %e (i%d) ",Coeff[i],Var);
	  else printf(" %e (c%d) ",Coeff[i],Var);
	  il++;
	}
	printf(" < %e\n",SecondMembre);	
	*/
	/* On va pouvoir utiliser la contrainte */

  for ( i = 0 ; i < NombreDeTermes ; i++ ) {	
    Var = Variable[i];
		if ( Coeff[i] == 0.0 ) {		
      Variable[i] = Variable[NombreDeTermes-1];
	    Coeff[i] = Coeff[NombreDeTermes-1];
			NombreDeTermes--;
			i--;
			continue;
 	  }
    Var = Variable[i];
		TypeBorne = TypeDeBorneTrav[Var];
    if ( TypeBorne == VARIABLE_FIXE ) {		
		  SecondMembre -= Coeff[i] * UTrav[Var];
      Variable[i] = Variable[NombreDeTermes-1];
	    Coeff[i] = Coeff[NombreDeTermes-1];
			NombreDeTermes--;
			i--;
			continue;		
		}			
	  if ( TypeDeVariableTrav[Var] == ENTIER ) continue;

		printf("Erreur il reste des variabes continues\n");
		exit(0);
NombreDeTermes = 0;
break;

		
		/* La variable n'est pas entiere */			
		if ( Coeff[i] < 0.0 ) {			
		  /* Il faut monter la variable au max */								
			if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			  SecondMembre -= Coeff[i] * UmaxTrav[Var];
        Variable[i] = Variable[NombreDeTermes-1];
	      Coeff[i] = Coeff[NombreDeTermes-1];
			  NombreDeTermes--;
			  i--;
      }						
			else {						
				NombreDeTermes = 0;
				break;
			}				
		}
		else {
		  /* Il faut baisser la variable au min */			
			if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
			  SecondMembre -= Coeff[i] * UminTrav[Var];
				Variable[i] = Variable[NombreDeTermes-1];
	      Coeff[i] = Coeff[NombreDeTermes-1];
			  NombreDeTermes--;
			  i--;					
      }						
			else {
				NombreDeTermes = 0;
				break;
			}								
		}			
  }
	/*
	printf("3 on arrive a NombreDeTermes %d\n",NombreDeTermes);
  for ( i = 0 ; i < NombreDeTermes ; i++ ) {
    Var = Variable[i];
    if ( TypeDeVariableTrav[Var] == ENTIER ) printf(" %e (i%d) ",Coeff[i],Var);
	  else printf(" %e (c%d) ",Coeff[i],Var);
	  il++;
	}
	printf(" < %e\n",SecondMembre);
	*/
	
  if ( NombreDeTermes <= 1 ) continue;
	
 	RendreLesCoeffsEntiers = NON_PNE;
	/*
	if ( CoeffMn > ZERO_COEFFMN ) {
	  if ( CoeffMx / CoeffMn < RAPPORT_MAX ) RendreLesCoeffsEntiers = OUI_PNE;
	} 
	*/
	CouvertureTrouvee = NON_PNE;						  
  PNE_GreedyCoverKnapsack( Pne, 0, NombreDeTermes, Variable, Coeff, SecondMembre, RendreLesCoeffsEntiers, &CouvertureTrouvee,
													 Mixed_0_1_Knapsack, 0.0, 0, NULL, NULL, NULL );			
	/*
	if ( CouvertureTrouvee == OUI_PNE ) {
	  printf("   K trouvee sur combinaison complexe !!!!!!!!!!!!!!\n");
	}
	*/
}

free( Buffer );

return;
}
