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

   FONCTION: Substitution des variables non bornees. La routine n'est
	           eventuellement utilisee qu'une seule fois au debut du presolve.
           
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

# define PIVOT_MIN     1.e-5
# define RAPPORT_MIN   1.e-2

void PRS_SubstitutionDUneVariableNonBornee( PRESOLVE * , int , int , double , double * , int * , char * ,
																						int * , int * , int , int * , char * );
																					
/*----------------------------------------------------------------------------*/
/* Important: ne peut etre utilise qu'une seule fois tout au debut du presolve */

void PRS_SubstitutionDUneVariableNonBornee( PRESOLVE * Presolve,
                                            int CntDeSubstitution,
                                            int VarSubstituee,
																	          double Pivot,
																						double * LignePivot,
																						int * DernierIndexDispo,  
																						char * T,
																						int * NbContraintesParColonne,
	                                          int * NbContraintesEgaliteParColonne,
																						int NbTermesColonnePivot,
																						int * ContraintesDeLaColonnePivot,
																						char * ContrainteModifiee
				                                  )
{
int il2 ; int il2Max; int ic; int Cnt; double CoeffPivot; int Var; int i; 
double BDeCntDeSubstitution; int NombreDeContraintesImpactees; int NbTermesLignePivot;
int Nn; int Nb; int il1; int il1Max; int icPrec; char Flag; PROBLEME_PNE * Pne;

return; /* Ne pas utiliser */

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

BDeCntDeSubstitution = Pne->BTrav[CntDeSubstitution];
NombreDeContraintesImpactees = 0;
NbTermesLignePivot = Pne->NbTermTrav[CntDeSubstitution]; /* On y a deja supprime le terme pivot */

/* Remarque: il faut conserver Pne-> pour eviter les pb lies aux redimensionnements */
for ( i = 0 ; i < NbTermesColonnePivot ; i++ ) {
  Cnt = ContraintesDeLaColonnePivot[i];
  ContrainteModifiee[Cnt] = OUI_PNE;
	/* Remarque: toutes les contraintes ayant servi a une substitution on deja ete dechainees */

	/* Recherche du terme de la ligne qui se trouve dans la colonne pivot */
  Flag = 0;
  il2 = Pne->MdebTrav[Cnt];
  il2Max = il2 + Pne->NbTermTrav[Cnt];
  while ( il2 < il2Max ) {	
		if ( Pne->NuvarTrav[il2] == VarSubstituee ) {
      CoeffPivot = Pne->ATrav[il2];
		  Flag = 1;
	    /* On supprime le terme de la ligne qui se trouve dans la colonne pivot */
			/* Attention au cas ou c'est le premier terme qu'on supprime */
			if ( il2 == Pne->MdebTrav[Cnt] ) {
        Pne->MdebTrav[Cnt]++;
			}
			else {
		    il2Max--;
	      Pne->ATrav[il2] = Pne->ATrav[il2Max];
		    Var = Pne->NuvarTrav[il2Max];
				
	      Pne->NuvarTrav[il2] = Pne->NuvarTrav[il2Max];

		    /* On change le chainage de la colonne Var de la colonne */  		
		    if ( Var != VarSubstituee ) {
				  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {
		        icPrec = -1;
		        ic = Pne->CdebTrav[Var];
		        while ( ic >= 0 ) {
              if ( ic == il2Max ) break;
			        icPrec = ic;
              ic = Pne->CsuiTrav[ic];
		        }	
		        if ( icPrec >= 0 ) {
			        Pne->CsuiTrav[icPrec] = il2;
				      Pne->CsuiTrav[il2] = Pne->CsuiTrav[ic];
			      }
		        else {
			        Pne->CdebTrav[Var] = il2;
			        Pne->CsuiTrav[il2] = Pne->CsuiTrav[ic];
			      }
		      }
				}
			}
		  Pne->NbTermTrav[Cnt]--;		
			break;
		}
	  il2++;
	}
  if ( Flag == 0 ) {
	  printf("Bug contrainte %d on ne trouve pas la variable pivot %d\n",Cnt,VarSubstituee);
		exit(0);
	}
		
  il2 = Pne->MdebTrav[Cnt];
  il2Max = il2 + Pne->NbTermTrav[Cnt];
	Nb = NbTermesLignePivot;
  while ( il2 < il2Max ) {			
    Var = Pne->NuvarTrav[il2];
		/* T[Var] = 0 si Var = VarSubstituee */
		if ( T[Var] == 1 ) {
		  /* Le terme existe deja */
      Pne->ATrav[il2] -= CoeffPivot * LignePivot[Var];
			T[Var] = 0;
			Nb--;
		}
	  il2++;
	}

	if ( Nb > 0 ) {
	  /* Nb est le nombre de termes que l'on va creer */
		/* Nn est le dernier index apres recopie. Quand on recopie on en profite pour reconstituer la marge */
		
		Nn = Pne->MdebTrav[Cnt] + Pne->NbTermTrav[Cnt] + Nb - 1 ;
	  if ( Nn > DernierIndexDispo[Cnt] ) {		
		  /* Ca ne tient pas: on doit recopier la contrainte a la fin et reconstituer la marge */
      Nn = Pne->PremierIndexLibre + Pne->NbTermTrav[Cnt] + Nb - 1 + MARGE_EN_FIN_DE_CONTRAINTE;			
		  if ( Nn >= Pne->TailleAlloueePourLaMatriceDesContraintes - 10 /* Marge */) {
			  /*printf("Redimensionnement\n");*/
			  PNE_AugmenterLaTailleDeLaMatriceDesContraintes( Pne );
			}
			/*printf("Recopie d'une contrainte a la fin  %d\n",Cnt);*/
      /* Recopie de la contrainte a la fin */
			il1 = Pne->MdebTrav[Cnt];		
			il1Max = il1 + Pne->NbTermTrav[Cnt];

		  il2 = Pne->PremierIndexLibre;
      Pne->MdebTrav[Cnt] = il2;
			
			while ( il1 < il1Max ) {
			  Pne->ATrav[il2] = 	Pne->ATrav[il1];
				Var = Pne->NuvarTrav[il1];
			  Pne->NuvarTrav[il2] = Var;
				
				Pne->NumContrainteTrav[il2] = Cnt;
							
        if ( Var != VarSubstituee ) {
			    if ( Pne->TypeDeBorneTrav[Var] != VARIABLE_NON_BORNEE ) {				
				    /* Modification du chainage par colonne */
					  /* Recherche de l'index il1 */
	          icPrec = -1;
		        ic = Pne->CdebTrav[Var];
		        while ( ic >= 0 ) {
              if ( ic == il1 ) break;
			        icPrec = ic;
              ic = Pne->CsuiTrav[ic];
		        }	
		        if ( icPrec >= 0 ) {
			        Pne->CsuiTrav[icPrec] = il2;
				      Pne->CsuiTrav[il2] = Pne->CsuiTrav[ic];
			      }
		        else {
			        Pne->CdebTrav[Var] = il2;
			       Pne->CsuiTrav[il2] = Pne->CsuiTrav[ic];
			      }
          }					
		    }
				il1++;
				il2++;
			}
			
      DernierIndexDispo[Cnt] = Nn;
			Pne->PremierIndexLibre = DernierIndexDispo[Cnt] + 1;			
		}
		
    il2 = Pne->MdebTrav[Cnt] + Pne->NbTermTrav[Cnt];

    il1 = Pne->MdebTrav[CntDeSubstitution];
    il1Max = il1 + Pne->NbTermTrav[CntDeSubstitution];
    while ( il1 < il1Max ) {		
      Var = Pne->NuvarTrav[il1];
		  if ( T[Var] == 1 ) {			
		    /* Le terme est cree */
        Pne->ATrav[il2] = -CoeffPivot * LignePivot[Var];
        Pne->NuvarTrav[il2] = Var;
				Pne->NbTermTrav[Cnt]++;
				NbContraintesParColonne[Var]++;
				if ( Pne->SensContrainteTrav[Cnt] == '=' ) NbContraintesEgaliteParColonne[Var]++;
				/* On chaine le nouveau terme */
				if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {						
				  icPrec = Pne->CdebTrav[Var];			
				  Pne->CdebTrav[Var] = il2;
				  Pne->NumContrainteTrav[il2] = Cnt;
				  Pne->CsuiTrav[il2] = icPrec;
				}
			  il2++;
			  T[Var] = 0;				
		  }
	    il1++;
	  }		
	}
	
  Pne->BTrav[Cnt] -= CoeffPivot * BDeCntDeSubstitution / Pivot;
	 	
  NombreDeContraintesImpactees++;

  il1 = Pne->MdebTrav[CntDeSubstitution];
  il1Max = il1 + Pne->NbTermTrav[CntDeSubstitution];
	Nb = 0;
  while ( il1 < il1Max ) { 
	  T[Pne->NuvarTrav[il1]] = 1; /* La colonne pivot a ete supprimee de la ligne pivot */
		Nb++;
	  il1++;
  }
	
}


/*printf("    NombreDeContraintesImpactees %d\n",NombreDeContraintesImpactees);*/
	  
return;
}  

/*----------------------------------------------------------------------------*/

void PRS_SubstituerLesVariablesNonBornees( PRESOLVE * Presolve )
{
int Var; int ic; int NbCMin; int NbC; int Nb; int VariableChoisie; int ContrainteChoisie;
int CntDeMinTermes; int MinTermesDeCnt; int Cnt; double Pivot; double PlusGrandTermeDeLaLigne;
int il; int ilMax; double PivotDeMinTermes; double PivotChoisi; int i;
double CoutDeLaVariableSubstituee; double Rapport; double RapportChoisi; PROBLEME_PNE * Pne;
double * LignePivot; char * T; int NombreDeVariablesNonBornees; int icPrec;
int * NumeroDesVariablesNonBornees; int ilPivot; int * DernierIndexDispo; int * NbContraintesEgaliteParColonne;
int * NbContraintesParColonne; int NbTermesColonnePivot; int * ContraintesDeLaColonnePivot;
char * ContrainteModifiee;


printf(" Ne pas utiliser PRS_SubstituerLesVariablesNonBornees !!!!!!!!!!!!!!!!\n");

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

printf("Nombre de contraintes au debut des subtsitutions %d\n",Pne->NombreDeContraintesTrav);

ContrainteModifiee = (char *) malloc( Pne->NombreDeContraintesTrav * sizeof( char ) );

ContraintesDeLaColonnePivot = (int *) malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );

NbContraintesParColonne = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
memset( (char *) NbContraintesParColonne, 0, Pne->NombreDeVariablesTrav * sizeof( int ) ); 

NbContraintesEgaliteParColonne = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
memset( (char *) NbContraintesEgaliteParColonne, 0, Pne->NombreDeVariablesTrav * sizeof( int ) ); 

DernierIndexDispo = (int *) malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  ContrainteModifiee[Cnt] = NON_PNE;
  DernierIndexDispo[Cnt] = Pne->MdebTrav[Cnt] + Pne->NbTermTrav[Cnt] + MARGE_EN_FIN_DE_CONTRAINTE - 1;
}

NumeroDesVariablesNonBornees = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );

LignePivot = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );

T = (char *) malloc( Pne->NombreDeVariablesTrav * sizeof( char ) );
memset( (char *) T, 0, Pne->NombreDeVariablesTrav * sizeof( char ) );

NombreDeVariablesNonBornees = 0;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {
	  NumeroDesVariablesNonBornees[NombreDeVariablesNonBornees] = Var;
		NombreDeVariablesNonBornees++;
	}
}
if ( NombreDeVariablesNonBornees == 0 ) goto PasDeSubsitutions;

for ( i = 0 ; i < NombreDeVariablesNonBornees ; i++ ) {
  Var = NumeroDesVariablesNonBornees[i];
	Nb = 0;
	NbC = 0;
  ic = Pne->CdebTrav[Var];
  while ( ic >= 0 ) {
	  if ( Presolve->ContrainteInactive[Pne->NumContrainteTrav[ic]] == NON_PNE ) {
      NbC++;		
      if ( Pne->SensContrainteTrav[Pne->NumContrainteTrav[ic]] == '=' ) Nb++;
		}
	  ic = Pne->CsuiTrav[ic];
	}
	NbContraintesParColonne[Var] = NbC;
	NbContraintesEgaliteParColonne[Var] = Nb;
}


NextSubstitution:
/* On substitue la variable non bornee qui a le moins de termes dans sa colonne.
   La contrainte choisie est la contrainte d'egalite qui a le moins de terme */
NbCMin = Pne->NombreDeContraintesTrav + 1;
VariableChoisie = -1;
ContrainteChoisie = -1;
PivotChoisi = 0.0;
RapportChoisi = 0.0;
for ( i = 0 ; i < NombreDeVariablesNonBornees ; i++ ) {
  Var = NumeroDesVariablesNonBornees[i];
	if ( NbContraintesEgaliteParColonne[Var] <= 0 ) continue;

	if ( NbContraintesParColonne[Var] != 1 ) continue; /* On ne supprime que les colonnes singleton */
	
	NbC = 0;
	MinTermesDeCnt = Pne->NombreDeContraintesTrav * 2;
	CntDeMinTermes = -1;
	PivotDeMinTermes = 0.0;
	Rapport = 0.0;
  ic = Pne->CdebTrav[Var];
  while ( ic >= 0 ) {
	  Cnt = Pne->NumContrainteTrav[ic];
    if ( Pne->SensContrainteTrav[Cnt] == '=' && ContrainteModifiee[Cnt] == NON_PNE && Presolve->ContrainteInactive[Pne->NumContrainteTrav[ic]] == NON_PNE ) {
			if ( Pne->NbTermTrav[Cnt] < MinTermesDeCnt ) {
				Pivot = Pne->ATrav[ic]; 
				if ( fabs( Pivot ) > PIVOT_MIN ) {
          PlusGrandTermeDeLaLigne = -1.;
					icPrec = -1;
					il = Pne->MdebTrav[Cnt];
          ilMax = il + Pne->NbTermTrav[Cnt];
					while ( il < ilMax ) {
					  if ( Pne->NuvarTrav[il] == Var ) icPrec = il;
						if ( fabs( Pne->ATrav[il] ) > PlusGrandTermeDeLaLigne ) PlusGrandTermeDeLaLigne = fabs( Pne->ATrav[il] );
            il++;
					}
					
					if ( icPrec == -1 ) {
            printf("Bug incoherence ligne colonne ligne %d colonne %d\n",Cnt,Var);
            printf("la ligne %d ne continient pas la colonne %d\n",Cnt,Var);
						exit(0);
					}
					
					if ( fabs( Pivot ) / PlusGrandTermeDeLaLigne > RAPPORT_MIN ) {				 
				    MinTermesDeCnt = Pne->NbTermTrav[Cnt];
					  CntDeMinTermes = Cnt;
						PivotDeMinTermes = Pivot;
						Rapport = fabs( Pivot ) / PlusGrandTermeDeLaLigne;
					}
				}				
			}
      NbC++;
		}
	  ic = Pne->CsuiTrav[ic];
	}
	if ( CntDeMinTermes >= 0 && PivotDeMinTermes != 0.0 ) {
	  if ( NbC < NbCMin ) {
		  NbCMin = NbC;
			VariableChoisie = Var;
      ContrainteChoisie = CntDeMinTermes;
			PivotChoisi = PivotDeMinTermes;
			RapportChoisi = Rapport;			
		} 
	}
}
if ( VariableChoisie >= 0 && ContrainteChoisie >= 0 && PivotChoisi != 0.0 ) {

  /*printf("VariableChoisie %d  ContrainteChoisie %d\n",VariableChoisie,ContrainteChoisie);*/

  NumeroDesVariablesNonBornees[i] = NumeroDesVariablesNonBornees[NombreDeVariablesNonBornees - 1];
  NombreDeVariablesNonBornees--;

	/* Expand de la contrainte choisie */
  CoutDeLaVariableSubstituee = Pne->LTrav[VariableChoisie];	
	ilPivot = -1;
	il = Pne->MdebTrav[ContrainteChoisie];
  ilMax = il + Pne->NbTermTrav[ContrainteChoisie];
	while ( il < ilMax ) {	
	  Var = Pne->NuvarTrav[il];
		if ( Var == VariableChoisie ) ilPivot = il;
    else Pne->LTrav[Var] -= CoutDeLaVariableSubstituee *  Pne->ATrav[il] / PivotChoisi;
	  LignePivot[Var] = Pne->ATrav[il] / PivotChoisi;
		T[Var] = 1;		
		/* On dechaine le terme dans la colonne */
	 	if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {								
		  icPrec = -1;
      ic = Pne->CdebTrav[Var];
		  while ( ic >= 0 ) {
		    if ( ic == il ) {
			    if ( icPrec >= 0 ) Pne->CsuiTrav[icPrec] = Pne->CsuiTrav[ic];
				  else Pne->CdebTrav[Var] = Pne->CsuiTrav[ic];								
	        NbContraintesParColonne[Var]--;
				  /* ContrainteChoisie est toujours une contrainte d'eaglite */
	        NbContraintesEgaliteParColonne[Var]--;				
				  break;			  
			  }
			  icPrec = ic; 
        ic = Pne->CsuiTrav[ic];
		  }
    }		
    il++;
	}
	/* Suppression du terme pivot */
  Pne->LTrav[VariableChoisie] = 0.0;	
	T[VariableChoisie] = 0;
	
	if ( ilPivot == Pne->MdebTrav[ContrainteChoisie] ) {
	  /* Si on supprimme le 1er terme */
    Pne->MdebTrav[ContrainteChoisie]++;
	}
	else {
	  ilMax--;
	  Pne->ATrav[ilPivot] = Pne->ATrav[ilMax];
	  Pne->NuvarTrav[ilPivot] = Pne->NuvarTrav[ilMax];
	}
	Pne->NbTermTrav[ContrainteChoisie]--;
	
  NbTermesColonnePivot = 0;
  ic = Pne->CdebTrav[VariableChoisie];
	while ( ic >= 0 ) {
    ContraintesDeLaColonnePivot[NbTermesColonnePivot] = Pne->NumContrainteTrav[ic];
	  NbTermesColonnePivot++;
    ic = Pne->CsuiTrav[ic];
	}
		
  Pne->TypeDeBorneTrav[VariableChoisie] = VARIABLE_FIXE;  
  Pne->UTrav[VariableChoisie] = 0.; /* Pas d'importance mais ainsi on n'a pas besoin de la supprimer de la liste des 
                                       variables ni de la supprimer des contraintes */
  Presolve->ContrainteInactive[ContrainteChoisie] = OUI_PNE;
	Pne->NumeroDesContraintesInactives[Pne->NombreDeContraintesInactives] = ContrainteChoisie;
	Pne->NombreDeContraintesInactives++;	
	
  /* On substitue la variable */
	if ( NbContraintesParColonne[VariableChoisie] > 0 ) {			
    PRS_SubstitutionDUneVariableNonBornee( Presolve, ContrainteChoisie, VariableChoisie, PivotChoisi,
																			  	 LignePivot, DernierIndexDispo, T, NbContraintesParColonne,
	                                         NbContraintesEgaliteParColonne, NbTermesColonnePivot, ContraintesDeLaColonnePivot,
																					 ContrainteModifiee );
	}
	
  Pne->CdebTrav[VariableChoisie] = -1;


	il = Pne->MdebTrav[ContrainteChoisie];
  ilMax = il + Pne->NbTermTrav[ContrainteChoisie];
	while ( il < ilMax ) {	
		T[Pne->NuvarTrav[il]] = 0;
    il++;
	}

  goto NextSubstitution;
																				 
}

printf("Fin\n");

PRS_EnleverLesContraintesInactives( Presolve );
PNE_ConstruireLeChainageDeLaTransposee( Pne );

PasDeSubsitutions:

free( ContrainteModifiee );
free( ContraintesDeLaColonnePivot );
free( NbContraintesParColonne );
free( NbContraintesEgaliteParColonne );
free( DernierIndexDispo );
free( LignePivot );
free( T );
free( NumeroDesVariablesNonBornees );

printf("Nombre de contraintes a la fin des subtsitutions %d\n",Pne->NombreDeContraintesTrav);

return;
}


