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


#include <math.h>
#include <sstream>
#include <cctype>
#include <iomanip>

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

/* Pour la recuperation de donnees annuelles pour les tests sur les arret pour entretien */
#include <antares/study.h>
#include <antares/study/area/scratchpad.h>
#include "../simulation/sim_structure_donnees.h"

////////////////////////////////////////////////////////////////////
// Export de la structure des LPs
////////////////////////////////////////////////////////////////////
#include <fstream>
#include <sstream>

#include "opt_export_structure.h"
namespace Antares {
namespace Data {
	namespace Enum {

		template <>
		const std::initializer_list<std::string>& getNames<ExportStructDict>() {
			
			static std::initializer_list<std::string> s_exportStructDictNames{
				"ValeurDeNTCOrigineVersExtremite",
				"PalierThermique",
				"ProdHyd",
				"DefaillancePositive", 
				"DefaillanceNegative"
				"BilansPays",
				"CoutOrigineVersExtremiteDeLInterconnexion",
				"CoutExtremiteVersOrigineDeLInterconnexion"
			};
			return s_exportStructDictNames;
		}
	} // namespace Enum
} // namespace Data
} // namespace Antares

void OPT_ExportInterco(PROBLEME_HEBDO* ProblemeHebdo, uint numSpace)
{
	auto& study = *Antares::Data::Study::Current::Get();

	//Interco are exported only once for first year
	if (study.runtime->currentYear[numSpace] == 0 && study.runtime->weekInTheYear[numSpace] == 0) {
		FILE* Flot = study.createFileIntoOutputWithExtension("interco", "txt", numSpace);
		for (int i(0); i < ProblemeHebdo->NombreDInterconnexions; ++i) {
			fprintf(Flot, "%d %d %d\n"
				,
				i,
				ProblemeHebdo->PaysOrigineDeLInterconnexion[i],
				ProblemeHebdo->PaysExtremiteDeLInterconnexion[i]
			);
		}
		fclose(Flot);
		Flot = study.createFileIntoOutputWithExtension("area", "txt", numSpace);
		for (int i(0); i < study.areas.size(); ++i) {
			fprintf(Flot, "%s\n", study.areas[i]->name.c_str());
		}
		fclose(Flot);
	}
}

void OPT_ExportAreaName(PROBLEME_HEBDO* ProblemeHebdo, uint numSpace)
{
	auto& study = *Antares::Data::Study::Current::Get();

	//Area name are exported only once for first year
	if (study.runtime->currentYear[numSpace] == 0 && study.runtime->weekInTheYear[numSpace] == 0) {
		FILE* Flot = study.createFileIntoOutputWithExtension("area", "txt", numSpace);
		for (int i(0); i < study.areas.size(); ++i) {
			fprintf(Flot, "%s\n", study.areas[i]->name.c_str());
		}
		fclose(Flot);
	}
}

void OPT_Export_add_variable(std::vector<std::string>& varname, int Var, Antares::Data::Enum::ExportStructDict structDict, int Pays, int Interco, int ts)
{
	if (varname.size() > Var && varname[Var].empty())
	{
		std::stringstream buffer;
		buffer << Var << " ";
		buffer << Antares::Data::Enum::toString(structDict) << " ";
		buffer << Pays << " ";
		buffer << Interco << " ";
		buffer << ts << " ";
		varname[Var] = buffer.str();
	}
}

void OPT_ExporterLaStrcutureDuProblemeLineaire(PROBLEME_HEBDO * ProblemeHebdo, uint numSpace)
{
	int Interco; int Index; int Pays; int Pdt; int Var; int Palier;
	int NbInterco, NbClusters; int CntCouplante; double Poids; int PdtDebut;
	int NombreDePasDeTempsDUneJournee; int NombreDePasDeTempsPourUneOptimisation;
	int Jour; int Semaine; int * NumeroDeJourDuPasDeTemps; int Pdt1; int Var1;
	int Var2; int Var3; int Offset; int Zone; int NombreDeZonesDeReserveJMoins1;
	int * NumeroDeZoneDeReserveJMoins1; char ContrainteDeReserveJMoins1ParZone;
	char Simulation;

	CORRESPONDANCES_DES_VARIABLES *        CorrespondanceVarNativesVarOptim;
	CORRESPONDANCES_DES_CONTRAINTES *      CorrespondanceCntNativesCntOptim;
	PALIERS_THERMIQUES *                   PaliersThermiquesDuPays;
	COUTS_DE_TRANSPORT *                   CoutDeTransport;
	CONTRAINTES_COUPLANTES *               MatriceDesContraintesCouplantes;
	PROBLEME_ANTARES_A_RESOUDRE *          ProblemeAResoudre;

	CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES  * CorrespondanceCntNativesCntOptimJournalieres;
	CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES * CorrespondanceCntNativesCntOptimHebdomadaires;

	ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
	NombreDeZonesDeReserveJMoins1 = ProblemeHebdo->NombreDeZonesDeReserveJMoins1;
	NumeroDeZoneDeReserveJMoins1 = ProblemeHebdo->NumeroDeZoneDeReserveJMoins1;
	ContrainteDeReserveJMoins1ParZone = ProblemeHebdo->ContrainteDeReserveJMoins1ParZone;

	NombreDePasDeTempsDUneJournee = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
	NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
	NumeroDeJourDuPasDeTemps = ProblemeHebdo->NumeroDeJourDuPasDeTemps;

	/*
	Convention pour les variables de flux (transit):
	- elle sont signees
	- la variable de flux est positive quand le flux va du noeud origine vers le noeud extremite
	Donc une contrainte de NTC est:
	valeur de la variable de flux <  ValeurDeNTCOrigineVersExtremite
	et valeur de la variable de flux > -ValeurDeNTCExtremiteVersOrigine
	Second membre des containtes de bilan: il est egal a -C
	*/
	int nvars = ProblemeAResoudre->NombreDeVariables;
	int ncons = ProblemeAResoudre->NombreDeContraintes;
	std::vector<std::string> varname;
	std::vector<std::string> conname;

	varname.assign(nvars, "");
	conname.assign(ncons, "");

	auto& study = *Antares::Data::Study::Current::Get();
	
	/* Boucle sur les pas de temps */
	for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++) {
		int ts = study.runtime->weekInTheYear[numSpace] * 168 + Pdt;
		CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
		CorrespondanceCntNativesCntOptim = ProblemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];

		for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++) {
			/* Partie variables de flux */
			Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Pays];
			while (Interco >= 0) {
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
				if (Var >= 0 && varname[Var].empty()){
					std::stringstream buffer;
					buffer << Var << " ";
					buffer << "ValeurDeNTCOrigineVersExtremite" << " ";
					buffer << Pays << " ";
					buffer << Interco << " ";
					buffer << ts << " ";
					varname[Var] = buffer.str();
				}
				Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
			}
			Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Pays];
			while (Interco >= 0) {
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
				if (Var >= 0 && varname[Var].empty()) {
					std::stringstream buffer;
					buffer << Var << " ";
					buffer << "ValeurDeNTCOrigineVersExtremite" << " ";
					buffer << Pays << " ";
					buffer << Interco << " ";
					buffer << ts << " ";
					varname[Var] = buffer.str();
				}
				Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
			}

			/* Partie variables de production thermique du pays */
			PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
			for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++) {
				Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
				if (Var >= 0 && varname[Var].empty()) {
					std::stringstream buffer;
					buffer << Var << " ";
					buffer << "PalierThermique" << " ";
					buffer << Pays << " ";
					buffer << Palier << " ";
					buffer << ts << " ";
					varname[Var] = buffer.str();
				}
			}

			/* Partie variables de production hydraulique du pays */
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
			if (Var >= 0 && varname[Var].empty()) {
				std::stringstream buffer;
				buffer << Var << " ";
				buffer << "ProdHyd" << " ";
				buffer << Pays << " ";
				buffer << ts << " ";				
				varname[Var] = buffer.str();
			}

			/* Groupes de defaillance */
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Pays];
			if (Var >= 0 && varname[Var].empty()) {
				std::stringstream buffer;
				buffer << Var << " ";
				buffer << "DefaillancePositive" << " ";
				buffer << Pays << " ";
				buffer << ts << " ";
				varname[Var] = buffer.str();

			}
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
			if (Var >= 0 && varname[Var].empty()) {
				std::stringstream buffer;
				buffer << Var << " ";
				buffer << "DefaillanceNegative" << " ";
				buffer << Pays << " ";
				buffer << ts << " ";
				varname[Var] = buffer.str();
			}

			/* Etablir le lien entre contrainte et bilan */
			{
				std::stringstream buffer;
				buffer << "BilansPays" << " ";
				buffer << Pays << " ";
				buffer << ts << " ";
				//conname[CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[Pays]] = buffer.str();
			}

			/* Partie variables de production thermique du pays */
			PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
			for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++) {
				Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
				if (Var >= 0 && varname[Var].empty()) {
					std::stringstream buffer;
					buffer << Var << " ";
					buffer << "PalierThermique" << " ";
					buffer << Pays << " ";
					buffer << Palier << " ";
					buffer << ts << " ";
					varname[Var] = buffer.str();
				}
			}

			/* Partie variables de production hydraulique du pays */
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
			if (Var >= 0 && varname[Var].empty()) {
				std::stringstream buffer;
				buffer << Var << " ";
				buffer << "ProdHyd" << " ";
				buffer << Pays << " ";
				buffer << ts << " ";
				varname[Var] = buffer.str();
			}

			/* Groupes de defaillance negative */
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
			if (Var >= 0 && varname[Var].empty()) {
				std::stringstream buffer;
				buffer << Var << " ";
				buffer << "DefaillanceNegative" << " ";
				buffer << Pays << " ";
				buffer << ts << " ";
				varname[Var] = buffer.str();
			}

			/* Etablir le lien entre contrainte et contrainte pour eviter les charges fictives */
			//CorrespondanceCntNativesCntOptim->NumeroDeContraintePourEviterLesChargesFictives[Pays];

		}

		/* Les contraintes pour identifier la partie positive/negative du flux si l'interco est geree avec des couts */
		if (COUT_TRANSPORT == OUI_ANTARES) {
			for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++) {
				CoutDeTransport = ProblemeHebdo->CoutDeTransport[Interco];
				if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES) {
					Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
					if (Var >= 0 && varname[Var].empty()) {
						std::stringstream buffer;
						buffer << Var << " ";
						buffer << "DefaillanceNegative" << " ";
						buffer << Pays << " ";
						buffer << ts << " ";
						buffer << ts << " ";
						varname[Var] = buffer.str();
					}
					Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
					if (Var >= 0 && varname[Var].empty()) {
						std::stringstream buffer;
						buffer << Var << " ";
						buffer << "CoutOrigineVersExtremiteDeLInterconnexion" << " ";
						buffer << Interco << " ";
						buffer << ts << " ";varname[Var] = buffer.str();
					}
					Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
					if (Var >= 0 && varname[Var].empty()) {
						std::stringstream buffer;
						buffer << Var << " ";
						buffer << "CoutExtremiteVersOrigineDeLInterconnexion" << " ";
						buffer << Interco << " ";
						buffer << ts << " ";
						varname[Var] = buffer.str();
					}
					///* Etablir le lien entre contrainte et flux */
					//CorrespondanceCntNativesCntOptim->NumeroDeContrainteDeDissociationDeFlux[Interco] = ProblemeAResoudre->NombreDeContraintes;
				}
			}
		}
		/* Les contraintes couplantes horaires de flux */
		for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes; CntCouplante++)
		{
			MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

			if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
			{
				/*Pour les interconnexions*/
				NbInterco = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
				for (Index = 0; Index < NbInterco; Index++)
				{
					Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
					Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
					Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];
					/*
					Pdt1    = Pdt + Offset;
					if ( Pdt1 >= NombreDePasDeTempsPourUneOptimisation ) Pdt1 = Pdt1 % NombreDePasDeTempsPourUneOptimisation;
					*/

					/* 26/11/2015 */
					if (Offset >= 0)
					{
						Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
					}
					else
					{
						Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
					}

					/*           */
					Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDeLInterconnexion[Interco];
					/* Ancienne instruction */
					if (Var >= 0 && varname[Var].empty()) {

					}
				}

				/*Pour les clusters thermiques*/
				NbClusters = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
				for (Index = 0; Index < NbClusters; Index++)
				{
					Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
					PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
					Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index]];
					Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];
					Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[Index];

					if (Offset >= 0)
					{
						Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
					}
					else
					{
						Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
					}

					/*           */
					Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDuPalierThermique[Palier];
					/* Ancienne instruction */
					/* Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco]; */
					if (Var >= 0 && varname[Var].empty()) {
						std::stringstream buffer;
						buffer << Var << " ";
						buffer << "CorrespondanceVarNativesVarOptim" << " ";						
						buffer << Palier << " ";
						buffer << Pdt1 << " ";
						varname[Var] = buffer.str();
					}
				}

				/* Etablir le lien entre contrainte et contrainte couplante de flux */
				CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesContraintesCouplantes[CntCouplante] = ProblemeAResoudre->NombreDeContraintes;
			}
		}

		if (ContrainteDeReserveJMoins1ParZone == OUI_ANTARES) {
			for (Zone = 0; Zone < NombreDeZonesDeReserveJMoins1; Zone++) {
				for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++) {
					if (NumeroDeZoneDeReserveJMoins1[Pays] != Zone) continue;
					/* Partie variables de production thermique du pays */
					PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
					for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++) {
						Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
						Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
						if (Var >= 0 && varname[Var].empty()) {
						}
					}
					/* Partie variables de production hydraulique du pays */
					Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
					if (Var >= 0 && varname[Var].empty()) {
					}

					/* Groupes de defaillance en reserve */
					Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceEnReserve[Pays];
					if (Var >= 0 && varname[Var].empty()) {
					}
				}
				/* Etablir le lien entre contrainte et bilan */
				CorrespondanceCntNativesCntOptim->NumeroPremiereContrainteDeReserveParZone[Zone] = ProblemeAResoudre->NombreDeContraintes;
				for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++) {
					if (NumeroDeZoneDeReserveJMoins1[Pays] != Zone) continue;
					/* Partie variables de production thermique du pays */
					PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
					for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++) {
						Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
						Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
						if (Var >= 0) {
						}
					}
					/* Partie variables de production hydraulique du pays */
					Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
					if (Var >= 0) {
					}

					/* Groupes de defaillance en reserve */
					Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceEnReserve[Pays];
					if (Var >= 0) {
					}
				}
				/* Etablir le lien entre contrainte et bilan */
				CorrespondanceCntNativesCntOptim->NumeroDeuxiemeContrainteDeReserveParZone[Zone] = ProblemeAResoudre->NombreDeContraintes;
			}
			/* Fin de la construction des contraintes de reserve par zone */
		}
	}

	/* Les contraintes couplantes journalieres de flux */
	for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes; CntCouplante++)
	{
		MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
		if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_JOURNALIERE)
		{
			NbInterco = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
			NbClusters = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
			PdtDebut = 0;
			while (PdtDebut < NombreDePasDeTempsPourUneOptimisation)
			{
				/* Si on travaille sur la journee, Jour vaudra toujours 0. Si on travaille sur la semaine, Jour vaudra de 0 a 6 */
				Jour = NumeroDeJourDuPasDeTemps[PdtDebut];
				CorrespondanceCntNativesCntOptimJournalieres = ProblemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[Jour];
				/*Pour les Interconnexions*/
				for (Index = 0; Index < NbInterco; Index++)
				{

					Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
					Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
					Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];

					for (Pdt = PdtDebut; Pdt < PdtDebut + NombreDePasDeTempsDUneJournee; Pdt++)
					{
						/* Anciennes instructions */
						/*
						CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
						Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
						*/
						/*
						Pdt1 = Pdt + Offset;
						if ( Pdt1 >= NombreDePasDeTempsPourUneOptimisation ) Pdt1 = Pdt1 % NombreDePasDeTempsPourUneOptimisation;
						*/

						/* 02/05/2016 */
						if (Offset >= 0)
						{
							Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
						}
						else
						{
							Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
						}

						Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDeLInterconnexion[Interco];
						if (Var >= 0)
						{
						}
					}
				}

				/*Pour les clusters thermiques*/
				for (Index = 0; Index < NbClusters; Index++)
				{

					Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
					PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
					Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index]];
					Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];
					Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[Index];

					for (Pdt = PdtDebut; Pdt < PdtDebut + NombreDePasDeTempsDUneJournee; Pdt++)
					{

						if (Offset >= 0)
						{
							Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
						}
						else
						{
							Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
						}

						Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDuPalierThermique[Palier];
						if (Var >= 0)
						{
						}
					}
				}


				/* Etablir le lien entre contrainte et contrainte mutli horaire */
				assert(CntCouplante >= 0);
				CorrespondanceCntNativesCntOptimJournalieres->NumeroDeContrainteDesContraintesCouplantes[CntCouplante] = ProblemeAResoudre->NombreDeContraintes;
				PdtDebut += NombreDePasDeTempsDUneJournee;
			}
		}
	}

	/* Les contraintes couplantes hebdomadaires de flux */
	if (ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation > ProblemeHebdo->NombreDePasDeTempsDUneJournee) {
		Semaine = 0;
		CorrespondanceCntNativesCntOptimHebdomadaires = ProblemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires[Semaine];
		for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes; CntCouplante++)
		{
			MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
			if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HEBDOMADAIRE)
			{
				/* Pour les Interconnexions*/
				NbInterco = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
				for (Index = 0; Index < NbInterco; Index++)
				{
					Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
					Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
					Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];
					for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
					{
						/* Anciennes instructions */
						/*
						CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
						Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
						*/
						/*
						Pdt1 = Pdt + Offset;
						if ( Pdt1 >= NombreDePasDeTempsPourUneOptimisation ) Pdt1 = Pdt1 % NombreDePasDeTempsPourUneOptimisation;
						*/

						/* 02/05/2016 */
						if (Offset >= 0)
						{
							Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
						}
						else
						{
							Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
						}

						Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDeLInterconnexion[Interco];
						if (Var >= 0)
						{
						}
					}
				}

				/*Pour les clusters thermiques*/
				NbClusters = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
				for (Index = 0; Index < NbClusters; Index++)
				{
					Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
					PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
					Palier = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index]];
					Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];
					Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[Index];
					for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
					{

						if (Offset >= 0)
						{
							Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
						}
						else
						{
							Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps) % NombreDePasDeTempsPourUneOptimisation;
						}

						Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariableDuPalierThermique[Palier];
						if (Var >= 0)
						{
						}
					}
				}

				/* Etablir le lien entre contrainte et contrainte mutli horaire */
				CorrespondanceCntNativesCntOptimHebdomadaires->NumeroDeContrainteDesContraintesCouplantes[CntCouplante] = ProblemeAResoudre->NombreDeContraintes;
			}
		}
	}

	/* Pour chaque pays, la contrainte d'energie hydraulique sur l'intervalle optimise */
	for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++) {
		if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable == OUI_ANTARES) {
			for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++) {
				Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]->NumeroDeVariablesDeLaProdHyd[Pays];
				if (Var >= 0) {
				}
			}
			/* Etablir le lien entre contrainte et pays */
			ProblemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays] = ProblemeAResoudre->NombreDeContraintes;
		}
		else
			ProblemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays] = -1;
	}

	if (ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS) {
		for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++) {
			if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable != OUI_ANTARES) continue;
			/* Boucle sur les pas de temps */
			for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++) {
				CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
				if (Var >= 0) {
				}
				Pdt1 = Pdt + 1;
				if (Pdt1 >= NombreDePasDeTempsPourUneOptimisation) Pdt1 = 0;
				Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]->NumeroDeVariablesDeLaProdHyd[Pays];
				if (Var1 >= 0) {
				}
				Var2 = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaBaisse[Pays];
				if (Var2 >= 0) {
				}
				Var3 = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaHausse[Pays];
				if (Var3 >= 0) {
				}
			}
		}
	}
	else if (ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX) {
		/* On cree 2 equations:
		x(t) < Mu
		x(t) > Nu
		et on met Mu * Epsilon - Nu * Epsilon dans le critere
		*/
		for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++) {
			if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable != OUI_ANTARES) continue;
			/* Boucle sur les pas de temps */
			for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++) {
				CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
				Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
				if (Var >= 0) {
				}
				Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]->NumeroDeVariablesVariationHydALaBaisse[Pays];
				if (Var1 >= 0) {
				}
				/* Etablir le lien entre contrainte et bilan */
				/* Attention, on n'etablit pas de correspondance car ce n'est pas utile */

				if (Var >= 0) {
				}
				Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]->NumeroDeVariablesVariationHydALaHausse[Pays];
				if (Var1 >= 0) {
				}
				/* Attention, on n'etablit pas de correspondance car ce n'est pas utile */
			}
		}
	}

	//if (ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES) {
	//	Simulation = NON_ANTARES;
	//	OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(ProblemeHebdo, Simulation);	
	//}


	{
		FILE * Flot = study.createFileIntoOutputWithExtension("variables", "txt", numSpace);
		for (auto const & line : varname) {
			fprintf(Flot, "%s\n", line.c_str());

		}
		fclose(Flot);
	}
	{
		FILE * Flot = study.createFileIntoOutputWithExtension("constraints", "txt", numSpace);
		for (auto const & line : conname) {
			fprintf(Flot, "%s\n", line.c_str());
		}
		fclose(Flot);
	}
	return;
}
