/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include <yuni/io/file.h>
#include "opt_fonctions.h"

extern "C"
{
# include "../ext/Sirius_Solver/simplexe/spx_definition_arguments.h"
# include "../ext/Sirius_Solver/simplexe/spx_fonctions.h"

# include "../ext/Sirius_Solver/pne/pne_definition_arguments.h"
# include "../ext/Sirius_Solver/pne/pne_fonctions.h"

}

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

# ifdef _MSC_VER
#	define SNPRINTF sprintf_s
# else
#	define SNPRINTF snprintf
# endif

void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage( PROBLEME_HEBDO * , int * , int , int );





void OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage( PROBLEME_HEBDO * ProblemeHebdo )
{
	int Pays; int Index; int PdtHebdo; int NombreDePasDeTempsProblemeHebdo; double X;

	int * NombreMinDeGroupesEnMarcheDuPalierThermique;
	int * NombreMaxDeGroupesEnMarcheDuPalierThermique;
	double NombreDeGroupesEnMarcheDuPalier;

	double * PuissanceDisponibleDuPalierThermique;
	double * PuissanceMinDuPalierThermique;
	double ProductionThermiqueDuPalier;
	double PminDUnGroupeDuPalierThermique;
	double PmaxDUnGroupeDuPalierThermique;
	
	double P; double Eps;
	double eps_prodTherm = 1.0;
	double eps_nbGroupes = 1.0;

	RESULTATS_HORAIRES *                  ResultatsHoraires;
	PRODUCTION_THERMIQUE_OPTIMALE **      ProductionThermique; 
	PDISP_ET_COUTS_HORAIRES_PAR_PALIER ** PuissanceDisponibleEtCout;
	PALIERS_THERMIQUES *                  PaliersThermiquesDuPays;

	
	if ( ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == NON_ANTARES ) return;
	
	NombreDePasDeTempsProblemeHebdo = ProblemeHebdo->NombreDePasDeTemps;
	Eps = 1.e-3;

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays; ++Pays)
	{
		ResultatsHoraires = ProblemeHebdo->ResultatsHoraires[Pays];
		ProductionThermique = ResultatsHoraires->ProductionThermique;
	
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];	
		PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout;
	
		for (Index = 0 ; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Index++)
		{
			if (ProblemeHebdo->Expansion)
			{
				NombreMinDeGroupesEnMarcheDuPalierThermique = PuissanceDisponibleEtCout[Index]->NombreMinDeGroupesEnMarcheDuPalierThermique;
				NombreMaxDeGroupesEnMarcheDuPalierThermique = PuissanceDisponibleEtCout[Index]->NombreMaxDeGroupesEnMarcheDuPalierThermique;

				PuissanceDisponibleDuPalierThermique = PuissanceDisponibleEtCout[Index]->PuissanceDisponibleDuPalierThermique;
				PuissanceMinDuPalierThermique = PuissanceDisponibleEtCout[Index]->PuissanceMinDuPalierThermique;

				for (PdtHebdo = 0; PdtHebdo < NombreDePasDeTempsProblemeHebdo; PdtHebdo++)
				{
					
					ProductionThermiqueDuPalier = ProductionThermique[PdtHebdo]->ProductionThermiqueDuPalier[Index];

					
					if (ProductionThermiqueDuPalier - eps_prodTherm > PuissanceMinDuPalierThermique[PdtHebdo])
						PuissanceMinDuPalierThermique[PdtHebdo] = ProductionThermiqueDuPalier - eps_prodTherm;
					
					if (ProductionThermiqueDuPalier + eps_prodTherm < PuissanceDisponibleDuPalierThermique[PdtHebdo])
						PuissanceDisponibleDuPalierThermique[PdtHebdo] = ProductionThermiqueDuPalier + eps_prodTherm;

					
					NombreDeGroupesEnMarcheDuPalier = ProductionThermique[PdtHebdo]->NombreDeGroupesEnMarcheDuPalier[Index];

					
					if (NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes > NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo])
						NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo] = (int) ceil(NombreDeGroupesEnMarcheDuPalier - eps_nbGroupes);
					






				}

			}
			else
			{
				NombreMinDeGroupesEnMarcheDuPalierThermique = PuissanceDisponibleEtCout[Index]->NombreMinDeGroupesEnMarcheDuPalierThermique;
				NombreMaxDeGroupesEnMarcheDuPalierThermique = PuissanceDisponibleEtCout[Index]->NombreMaxDeGroupesEnMarcheDuPalierThermique;
				PuissanceDisponibleDuPalierThermique = PuissanceDisponibleEtCout[Index]->PuissanceDisponibleDuPalierThermique;
				PminDUnGroupeDuPalierThermique = PaliersThermiquesDuPays->PminDUnGroupeDuPalierThermique[Index];
				PmaxDUnGroupeDuPalierThermique = PaliersThermiquesDuPays->PmaxDUnGroupeDuPalierThermique[Index];

				for (PdtHebdo = 0; PdtHebdo < NombreDePasDeTempsProblemeHebdo; PdtHebdo++) 
				{
					X = ProductionThermique[PdtHebdo]->NombreDeGroupesEnMarcheDuPalier[Index];
					if (X > NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo] + Eps) {
						printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
						printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e max %d\n", Pays, Index, X, NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo]);
					}
					if (X < NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo] - Eps) {
						printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
						printf("Pays %d palier dans le pays %d NombreDeGroupesEnMarche %e min %d\n", Pays, Index, X, NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo]);
					}
					P = ProductionThermique[PdtHebdo]->ProductionThermiqueDuPalier[Index];
					if (P < X * PminDUnGroupeDuPalierThermique - Eps) {
						printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
						printf("Pays %d palier dans le pays %d P %e < NbGroupe (%e) * PminGroupe (%e)\n", Pays, Index, P, X, PminDUnGroupeDuPalierThermique);
					}
					if (P > X * PmaxDUnGroupeDuPalierThermique + Eps) {
						printf("Attention, AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage: \n");
						printf("Pays %d palier dans le pays %d P %e > NbGroupe (%e) * PmaxGroupe (%e)\n", Pays, Index, P, X, PmaxDUnGroupeDuPalierThermique);
					}

					NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo] = (int)ceil(X);

				}

				OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(ProblemeHebdo, NombreMinDeGroupesEnMarcheDuPalierThermique,
					Pays, Index);

				for (PdtHebdo = 0; PdtHebdo < NombreDePasDeTempsProblemeHebdo; PdtHebdo++)
				{
					if (NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo] < NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo]) 
						NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo] = NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo];

					if (PminDUnGroupeDuPalierThermique * NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo] > PuissanceDisponibleDuPalierThermique[PdtHebdo]) 
						PuissanceDisponibleDuPalierThermique[PdtHebdo] = PminDUnGroupeDuPalierThermique * NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo];
				}
			}
																																					 
		}
	}

	return;
}



void OPT_PbLineairePourAjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage( PROBLEME_HEBDO * ProblemeHebdo,
                                                                            int * NbMinOptDeGroupesEnMarche,
																																						int Pays, int Index )
{
int t1; int k; double SMarche; int t1moins1; int Pdt; double SArret; int NombreDeTermes; char * Sens;
int NombreDePasDeTempsProblemeHebdo; int * NombreMaxDeGroupesEnMarcheDuPalierThermique; double * Xmax;
int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique; int DureeMinimaleDArretDUnGroupeDuPalierThermique;
int NombreDePasDeTemps; PALIERS_THERMIQUES * PaliersThermiquesDuPays; int NombreDeVariables;
int NombreDeContraintes; int NbTermesMatrice; double * CoutLineaire; double * Xsolution; double * Xmin;
int * TypeDeVariable; int * IndicesDebutDeLigne; int * NombreDeTermesDesLignes; double * SecondMembre;
int * IndicesColonnes; double * CoefficientsDeLaMatriceDesContraintes; int * NumeroDeVariableDeM;
int * NumeroDeVariableDeMMoinsMoins; int * NumeroDeVariableDeMPlus; int * NumeroDeVariableDeMMoins;
int * PositionDeLaVariable; int * ComplementDeLaBase; int ResoudreLeProblemeLineaire; int * TypeEntierOuReel;
int Var; PRODUCTION_THERMIQUE_OPTIMALE ** ProductionThermique; 

PROBLEME_SIMPLEXE Probleme; PROBLEME_SPX * ProbSpx; PROBLEME_A_RESOUDRE ProblemePourPne;

NombreDePasDeTempsProblemeHebdo = ProblemeHebdo->NombreDePasDeTemps;
NombreDePasDeTemps = NombreDePasDeTempsProblemeHebdo;

PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
NombreMaxDeGroupesEnMarcheDuPalierThermique = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]->NombreMaxDeGroupesEnMarcheDuPalierThermique;
DureeMinimaleDeMarcheDUnGroupeDuPalierThermique = PaliersThermiquesDuPays->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[Index];		
DureeMinimaleDArretDUnGroupeDuPalierThermique = PaliersThermiquesDuPays->DureeMinimaleDArretDUnGroupeDuPalierThermique[Index];

ProductionThermique = ProblemeHebdo->ResultatsHoraires[Pays]->ProductionThermique;

ResoudreLeProblemeLineaire = NON_ANTARES;


for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ ) {

  t1 = Pdt;
  t1moins1 = t1 - 1;
  if ( t1moins1 < 0 ) t1moins1 = NombreDePasDeTemps + t1moins1;
	
  if ( NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] < 0 ) {
    ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index] = 0;
    ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index] = NbMinOptDeGroupesEnMarche[t1moins1] - NbMinOptDeGroupesEnMarche[t1];
		ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] = 0;		
    if ( NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] < NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]  ) {
      if ( NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] <
			     ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index] ) {
		    ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] =
				                                       NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
			}
			else {
		    ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] = ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index];
			}					
    }
	}
	else if ( NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1] > 0 ) {
    ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index] = NbMinOptDeGroupesEnMarche[t1] - NbMinOptDeGroupesEnMarche[t1moins1];
    ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index] = 0;
		ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] = 0;		
  }
	else {
    ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index] = 0;
    ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index] = 0;
		ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index] = 0;		
	}	   
}
 

for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ ) {					
  if ( DureeMinimaleDeMarcheDUnGroupeDuPalierThermique <= 0 ) goto ContrainteDeDureeMinDArret;

	
  SMarche = 0;
	for ( k = Pdt-DureeMinimaleDeMarcheDUnGroupeDuPalierThermique+1 ; k <= Pdt ; k++ ) {
    t1 = k;
		
		if ( k < 0 ) t1 = NombreDePasDeTemps + k;					
    SMarche += ProductionThermique[t1]->NombreDeGroupesQuiDemarrentDuPalier[Index] -
		           ProductionThermique[t1]->NombreDeGroupesQuiTombentEnPanneDuPalier[Index];		
	}
  if ( NbMinOptDeGroupesEnMarche[t1] < SMarche ) {
    ResoudreLeProblemeLineaire = OUI_ANTARES;
		break;
	}

  ContrainteDeDureeMinDArret:
  
  t1 = Pdt-DureeMinimaleDArretDUnGroupeDuPalierThermique;				
	
	if ( t1 < 0 ) t1 = NombreDePasDeTemps + t1;
  SArret = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
	
  for ( k = Pdt-DureeMinimaleDArretDUnGroupeDuPalierThermique+1 ; k <= Pdt ; k++ ) {
    t1 = k;				
		
		if ( t1 < 0 ) t1 = NombreDePasDeTemps + t1;
    t1moins1 = t1 - 1;
    if ( t1moins1 < 0 ) t1moins1 = NombreDePasDeTemps + t1moins1;
		if ( NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1] > 0 ) {
      SArret += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1];
		}		
    SArret -= ProductionThermique[t1]->NombreDeGroupesQuiSArretentDuPalier[Index];
	}
  if ( NbMinOptDeGroupesEnMarche[t1] > SArret ) {
    ResoudreLeProblemeLineaire = OUI_ANTARES;
		break;
	}	
}

if ( ResoudreLeProblemeLineaire == NON_ANTARES ) return;


 
NombreDeVariables = 0;
NombreDeVariables += NombreDePasDeTemps; 
NombreDeVariables += NombreDePasDeTemps; 
NombreDeVariables += NombreDePasDeTemps; 
NombreDeVariables += NombreDePasDeTemps; 

NumeroDeVariableDeM = (int *) MemAlloc( NombreDePasDeTemps * sizeof( int ) );
NumeroDeVariableDeMMoinsMoins = (int *) MemAlloc( NombreDePasDeTemps * sizeof( int ) );
NumeroDeVariableDeMPlus = (int *) MemAlloc( NombreDePasDeTemps * sizeof( int ) );
NumeroDeVariableDeMMoins = (int *) MemAlloc( NombreDePasDeTemps * sizeof( int ) );

NombreDeContraintes = 0;
NombreDeContraintes += NombreDePasDeTemps; 
NombreDeContraintes += NombreDePasDeTemps; 
NombreDeContraintes += NombreDePasDeTemps; 
NombreDeContraintes += NombreDePasDeTemps; 
NombreDeContraintes += NombreDePasDeTemps; 

PositionDeLaVariable = (int *) MemAlloc( NombreDeVariables * sizeof( int ) );
CoutLineaire = (double *) MemAlloc( NombreDeVariables * sizeof( double ) );
Xsolution = (double *) MemAlloc( NombreDeVariables * sizeof( double ) );
Xmin = (double *) MemAlloc( NombreDeVariables * sizeof( double ) );
Xmax = (double *) MemAlloc( NombreDeVariables * sizeof( double ) );
TypeDeVariable = (int *) MemAlloc( NombreDeVariables * sizeof( int ) );
TypeEntierOuReel = (int *) MemAlloc( NombreDeVariables * sizeof( int ) );

ComplementDeLaBase = (int *) MemAlloc( NombreDeContraintes * sizeof( int ) );
IndicesDebutDeLigne = (int *) MemAlloc( NombreDeContraintes * sizeof( int ) );
NombreDeTermesDesLignes = (int *) MemAlloc( NombreDeContraintes * sizeof( int ) );
Sens = (char *) MemAlloc( NombreDeContraintes * sizeof( char ) );
SecondMembre = (double *) MemAlloc( NombreDeContraintes * sizeof( double ) );

NbTermesMatrice = 0;
NbTermesMatrice += 4 * NombreDePasDeTemps; 
NbTermesMatrice += 2 * NombreDePasDeTemps; 
NbTermesMatrice += 1 * NombreDePasDeTemps; 
NbTermesMatrice += NombreDePasDeTemps * ( 1 + (2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique) ); 
NbTermesMatrice += NombreDePasDeTemps * ( 1 + DureeMinimaleDArretDUnGroupeDuPalierThermique ); 

IndicesColonnes = (int *) MemAlloc( NbTermesMatrice * sizeof( int ) );
CoefficientsDeLaMatriceDesContraintes = (double *) MemAlloc( NbTermesMatrice * sizeof( double ) );

if ( NumeroDeVariableDeM == NULL || NumeroDeVariableDeMMoinsMoins == NULL || NumeroDeVariableDeMPlus == NULL || NumeroDeVariableDeMMoins == NULL || 
     PositionDeLaVariable == NULL || CoutLineaire == NULL || Xsolution == NULL || Xmin == NULL || Xmax == NULL || TypeDeVariable == NULL ||
		 TypeEntierOuReel == NULL ||  ComplementDeLaBase == NULL || IndicesDebutDeLigne == NULL || NombreDeTermesDesLignes == NULL || Sens == NULL ||
		 SecondMembre == NULL || IndicesColonnes == NULL || CoefficientsDeLaMatriceDesContraintes	== NULL ) {
  logs.info(); 
  logs.error() << "Internal error: insufficient memory";
  logs.info(); 
  AntaresSolverEmergencyShutdown();  
  return;	
}




NombreDeVariables = 0;
for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ ) {					
  
	NumeroDeVariableDeM[Pdt] = NombreDeVariables;
  CoutLineaire[NombreDeVariables] = 1;
  Xsolution[NombreDeVariables] = 0;
  Xmin[NombreDeVariables] = NbMinOptDeGroupesEnMarche[Pdt];
  Xmax[NombreDeVariables] = NombreMaxDeGroupesEnMarcheDuPalierThermique[Pdt];
  TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
  TypeEntierOuReel[NombreDeVariables] = REEL;	
	NombreDeVariables++;
	
	
	NumeroDeVariableDeMMoinsMoins[Pdt] = NombreDeVariables;
  CoutLineaire[NombreDeVariables] = 0;
  Xsolution[NombreDeVariables] = 0;
  Xmin[NombreDeVariables] = 0;
	# if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES		
    Xmax[NombreDeVariables] = LINFINI_ANTARES; 
    TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
	TypeEntierOuReel[NombreDeVariables] = REEL;
	# else
    TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
	TypeEntierOuReel[NombreDeVariables] = REEL;
  # endif	
	NombreDeVariables++;
	
	
	NumeroDeVariableDeMPlus[Pdt] = NombreDeVariables;
  CoutLineaire[NombreDeVariables] = 0;
  Xsolution[NombreDeVariables] = 0;
  Xmin[NombreDeVariables] = 0;
  



	Xmax[NombreDeVariables] = LINFINI_ANTARES;
  TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
  TypeEntierOuReel[NombreDeVariables] = REEL;
	NombreDeVariables++;
	
	
	NumeroDeVariableDeMMoins[Pdt] = NombreDeVariables;
  CoutLineaire[NombreDeVariables] = 0;
  Xsolution[NombreDeVariables] = 0;
  Xmin[NombreDeVariables] = 0;
  



	Xmax[NombreDeVariables] = LINFINI_ANTARES;
  TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
  TypeEntierOuReel[NombreDeVariables] = REEL;
	NombreDeVariables++;		
}


NbTermesMatrice = 0;
NombreDeContraintes = 0;
for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ ) {

  
	NombreDeTermes = 0;
	IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;
		
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[Pdt];
  NombreDeTermes++;
  NbTermesMatrice++;	
	
	t1moins1 = Pdt - 1;
	if ( t1moins1 < 0 ) t1moins1 = NombreDePasDeTemps + t1moins1;
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[t1moins1];
  NombreDeTermes++;
  NbTermesMatrice++;		
	
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMPlus[Pdt];	
  NombreDeTermes++;
  NbTermesMatrice++;
	
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoins[Pdt];	
  NombreDeTermes++;
  NbTermesMatrice++;
	
	NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
	Sens[NombreDeContraintes] = '=';
	SecondMembre[NombreDeContraintes] = 0;
	NombreDeContraintes++;

  
	NombreDeTermes = 0;
	IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;
	
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoinsMoins[Pdt];
  NombreDeTermes++;
  NbTermesMatrice++;
	
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoins[Pdt];
  NombreDeTermes++;
  NbTermesMatrice++;
	
	NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
	Sens[NombreDeContraintes] = '<';
	SecondMembre[NombreDeContraintes] = 0;
	NombreDeContraintes++;
	
  
	# if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES	
    t1 = Pdt;
	  t1moins1 = t1-1;
	  if ( t1moins1 < 0 ) t1moins1 = NombreDePasDeTemps + t1moins1;	
	  NombreDeTermes = 0;
	  IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;
	  
	  CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	  IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoinsMoins[Pdt];
    NombreDeTermes++;
    NbTermesMatrice++;
	
	  NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
	  Sens[NombreDeContraintes] = '<';
	  if ( NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]-NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] > 0 ) {
	    SecondMembre[NombreDeContraintes] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]-NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
	  }
	  else {
	    SecondMembre[NombreDeContraintes] = 0;
	  }
	  NombreDeContraintes++;
	# else
	  Var = NumeroDeVariableDeMMoinsMoins[Pdt];
	  t1 = Pdt;
    t1moins1 = t1 - 1;				
		if ( t1moins1 < 0 ) t1moins1 = NombreDePasDeTemps + t1moins1;					
		Xmax[Var] = 0;
		if ( NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] > 0 ) {
			Xmax[Var] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
		}	
	# endif

  
	NombreDeTermes = 0;
	IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;
		
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[Pdt];
  NombreDeTermes++;
  NbTermesMatrice++;
  for ( k = Pdt-DureeMinimaleDeMarcheDUnGroupeDuPalierThermique+1 ; k <= Pdt ; k++ ) {				
    t1 = k;
		if ( t1 < 0 ) t1 = NombreDePasDeTemps + t1;
	  		
	  CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = -1;
	  IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMPlus[t1];
    NombreDeTermes++;
    NbTermesMatrice++;
	  
	  CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	  IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoinsMoins[t1];
    NombreDeTermes++;
    NbTermesMatrice++;		
	}
	
	NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;	
	Sens[NombreDeContraintes] = '>';
	SecondMembre[NombreDeContraintes] = 0;	
	NombreDeContraintes++;	

  
	NombreDeTermes = 0;
	IndicesDebutDeLigne[NombreDeContraintes] = NbTermesMatrice;
		
	CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeM[Pdt];
  NombreDeTermes++;
  NbTermesMatrice++;	
		
  for ( k = Pdt-DureeMinimaleDArretDUnGroupeDuPalierThermique+1 ; k <= Pdt ; k++ ) {
    t1 = k;
		if ( t1 < 0 ) t1 = NombreDePasDeTemps + t1;			
	  CoefficientsDeLaMatriceDesContraintes[NbTermesMatrice] = 1;
	  IndicesColonnes[NbTermesMatrice] = NumeroDeVariableDeMMoins[t1];
    NombreDeTermes++;
    NbTermesMatrice++;
	}
	
	NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;
	Sens[NombreDeContraintes] = '<';
	t1 = Pdt-DureeMinimaleDArretDUnGroupeDuPalierThermique;
	if ( t1 < 0 ) t1 = NombreDePasDeTemps + t1;	
  SecondMembre[NombreDeContraintes] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];	
  for ( k = Pdt-DureeMinimaleDArretDUnGroupeDuPalierThermique+1 ; k <= Pdt ; k++ ) {
		t1 = k;				
		
		if ( t1 < 0 ) t1 = NombreDePasDeTemps + t1;
		t1moins1 = t1-1;
  		if ( t1moins1 < 0 ) t1moins1 = NombreDePasDeTemps + t1moins1;		
		if ( NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1] > 0) {
			SecondMembre[NombreDeContraintes] += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1] - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1];
		}
	}			
	NombreDeContraintes++;
}

# ifdef TRACES
	printf("Optimisation auxiliaire: NombreDeVariables %d NombreDeContraintes %d NbTermesMatrice %d\n",NombreDeVariables,NombreDeContraintes,NbTermesMatrice);
# endif

if ( ProblemeHebdo->SolveurDuProblemeLineaire == ANTARES_SIMPLEXE ) {

  

  Probleme.Contexte            = SIMPLEXE_SEUL;
  Probleme.BaseDeDepartFournie = NON_SPX;

  Probleme.NombreMaxDIterations = -1; 
  Probleme.DureeMaxDuCalcul     = -1.;

  Probleme.CoutLineaire      = CoutLineaire;
  Probleme.X                 = Xsolution;
  Probleme.Xmin              = Xmin;
  Probleme.Xmax              = Xmax;
  Probleme.NombreDeVariables = NombreDeVariables;
  Probleme.TypeDeVariable    = TypeDeVariable;

  Probleme.NombreDeContraintes                   = NombreDeContraintes;
  Probleme.IndicesDebutDeLigne                   = IndicesDebutDeLigne;
  Probleme.NombreDeTermesDesLignes               = NombreDeTermesDesLignes;
  Probleme.IndicesColonnes                       = IndicesColonnes;
  Probleme.CoefficientsDeLaMatriceDesContraintes = CoefficientsDeLaMatriceDesContraintes;
  Probleme.Sens                                  = Sens;
  Probleme.SecondMembre                          = SecondMembre;

  Probleme.ChoixDeLAlgorithme = SPX_DUAL;

  Probleme.TypeDePricing               = PRICING_STEEPEST_EDGE;
  Probleme.FaireDuScaling              = OUI_SPX;
  Probleme.StrategieAntiDegenerescence = AGRESSIF;

  Probleme.PositionDeLaVariable       = PositionDeLaVariable;
  Probleme.NbVarDeBaseComplementaires = 0; 
  Probleme.ComplementDeLaBase         = ComplementDeLaBase;

  Probleme.LibererMemoireALaFin = OUI_SPX;

  Probleme.UtiliserCoutMax = NON_SPX;
  Probleme.CoutMax = 0.0;

  Probleme.CoutsMarginauxDesContraintes = NULL;
  Probleme.CoutsReduits                 = NULL;

  Probleme.AffichageDesTraces = NON_SPX;
 
  Probleme.NombreDeContraintesCoupes = 0;

  ProbSpx = NULL;
  ProbSpx = SPX_Simplexe( &Probleme , ProbSpx );

  if ( Probleme.ExistenceDUneSolution == OUI_SPX ) {
    
    for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ ) {
      NbMinOptDeGroupesEnMarche[Pdt] = (int) ceil(Xsolution[NumeroDeVariableDeM[Pdt]]);
	  }
  }
  else {
	# ifdef TRACES
    printf("Pas de solution au probleme auxiliaire\n");
	# endif
  }
}
else {

  

  ProblemePourPne.CoutLineaire            = CoutLineaire;
  ProblemePourPne.X                       = Xsolution;
  ProblemePourPne.Xmin                    = Xmin;
  ProblemePourPne.Xmax                    = Xmax;
  ProblemePourPne.NombreDeVariables       = NombreDeVariables;
  ProblemePourPne.TypeDeVariable          = TypeEntierOuReel;
  ProblemePourPne.TypeDeBorneDeLaVariable = TypeDeVariable; 
  ProblemePourPne.NombreDeContraintes                   = NombreDeContraintes;
  ProblemePourPne.IndicesDebutDeLigne                   = IndicesDebutDeLigne;
  ProblemePourPne.NombreDeTermesDesLignes               = NombreDeTermesDesLignes;
  ProblemePourPne.IndicesColonnes                       = IndicesColonnes;
  ProblemePourPne.CoefficientsDeLaMatriceDesContraintes = CoefficientsDeLaMatriceDesContraintes;
  ProblemePourPne.Sens                                  = Sens;	
  ProblemePourPne.SecondMembre                          = SecondMembre;
  ProblemePourPne.VariablesDualesDesContraintes         = NULL;
  ProblemePourPne.SortirLesDonneesDuProbleme = NON_PNE;
  ProblemePourPne.AlgorithmeDeResolution     = SIMPLEXE;   
  ProblemePourPne.CoupesLiftAndProject       = NON_PNE; 
  ProblemePourPne.AffichageDesTraces = NON_PNE;
  ProblemePourPne.FaireDuPresolve = OUI_PNE ;               
  if ( ProblemePourPne.FaireDuPresolve == NON_PNE ) printf("!!!!!!!!!!!!!!  Attention pas de presolve dans les problemes auxiliaires   !!!!!!!!!\n");
  ProblemePourPne.TempsDExecutionMaximum       = 0;  
  ProblemePourPne.NombreMaxDeSolutionsEntieres = -1;   
  ProblemePourPne.ToleranceDOptimalite         = 1.e-4; 

  PNE_Solveur( &ProblemePourPne );

  if ( ProblemePourPne.ExistenceDUneSolution == SOLUTION_OPTIMALE_TROUVEE ) {
    
    for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ ) {
      NbMinOptDeGroupesEnMarche[Pdt] = (int) ceil(Xsolution[NumeroDeVariableDeM[Pdt]]);
	  }
	}
  else {
	  # ifdef TRACES
    printf("Pas de solution au probleme auxiliaire\n");
	# endif
  }	
}

MemFree( NumeroDeVariableDeM );
MemFree( NumeroDeVariableDeMMoinsMoins );
MemFree( NumeroDeVariableDeMPlus );
MemFree( NumeroDeVariableDeMMoins );
MemFree( PositionDeLaVariable );
MemFree( CoutLineaire );
MemFree( Xsolution );
MemFree( Xmin );
MemFree( Xmax );
MemFree( TypeDeVariable );
MemFree( TypeEntierOuReel );
MemFree( ComplementDeLaBase );
MemFree( IndicesDebutDeLigne );
MemFree( NombreDeTermesDesLignes );
MemFree( Sens );
MemFree( SecondMembre );
MemFree( IndicesColonnes );
MemFree( CoefficientsDeLaMatriceDesContraintes );

return;
}

