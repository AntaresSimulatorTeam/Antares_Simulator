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

#include "atsp.h"
#include <antares/inifile.h>
#include <yuni/io/directory.h>


using namespace Yuni;


#define SEP  Yuni::IO::Separator


namespace Antares
{


	bool ATSP::preflight()
	{
		logs.info();
		logs.checkpoint() << "Extracting coefficients";

		// Preparing folders
		logs.info() << "Preparing folders...";
		{
			uint error = 0;
			for (uint i = 0; i != pArea.size(); ++i)
			{
				String& folder = folderPerArea[i];
				folder.clear() << pTemp << SEP << tsName << SEP << (i / 256) << SEP << pArea[i]->name;
				if (!IO::Directory::Create(folder))
				{
					logs.error() << "impossible to create the directory " << folder;
					if (++error > 10) // try all folders before aborting
					{
						logs.info() << "Too many errors. Aborting now.";
						return false;
					}
				}
			}
			if (error)
				return false;
		}

		logs.info(); // empty line

		pEnabledAreaCount = 0;
		for (uint i = 0; i != pArea.size(); ++i)
		{
			if (!preflight(i))
			{
				pArea[i]->enabled = false;
				logs.info() << " The area '" << pArea[i]->name << "' has been removed";
			}
			else
				++pEnabledAreaCount;
		}

		// moments
		writeMoments();

		// Statistics
		logs.info(); // empty line
		if (pUseUpperBound || pUseLowerBound)
		{
			logs.info() << " Summary:";
			logs.info() << ' ' << pTimeseriesCount << " timeseries";
			logs.info() << ' ' << pRoundingCount << " values / " << pRoundingCountTotal << " out of bounds ("
				<< (100. * pRoundingCount / pRoundingCountTotal) << "%)";
			if (pUseUpperBound)
			{
				logs.info() << " " << pRounding80percentCount << " values above 80% of the upper bound ("
					<< (100. * pRounding80percentCount / pRoundingCountTotal) << "%)";
			}
			logs.info(); // empty line
		}

		return (pEnabledAreaCount > 0);
	}



	bool ATSP::preflight(const uint areaIndex)
	{
		// Alias to the current area info
		const AreaInfo& info = *(pArea[areaIndex]);
		// The folder for the current area
		const String& folder = folderPerArea[areaIndex];

		// log
		logs.info() << "Coefficients: area " << (1+areaIndex) << '/' << pArea.size()
			<< ": " << info.name;


		// Reading the matrix file for the current area
		Matrix<> MTRX;
		if (!MTRX.loadFromCSVFile(info.filename, 1, pMHeight, Matrix<>::optImmediate))
			return false;
		// Shrinking the matrix if required
		if (MTRX.width > pTimeseriesCount)
			MTRX.resizeWithoutDataLost(pTimeseriesCount, MTRX.height);
		else
		{
			if (MTRX.width < pTimeseriesCount)
			{
				logs.error() << "not enough timeseries for the area '" << info.name << "'";
				return false;
			}
		}

		// Round values
		roundMatrixValues(MTRX);

		// Matrix used for writing data
		Matrix<> mtrxWriter;
		// Modulation
		Matrix<> modulation(12, 24);

		// Initializing non standardized bounds
		double minimu_brut[12];
		double maximu_brut[12];
		for (uint j = 0; j != 12; ++j)
		{
			minimu_brut[j] = +1e+30;
			maximu_brut[j] = -1e+30;
		}

		// Series de coefficients d'autocorrelation, en pratique seuls
		// les PRA premières valeurs sont utilisées
		double AUTO_ESTIM1[744];
		// les deux estimations convergent pour des series infinies
		double AUTO_ESTIM2[744];

		// col 1=alpha 2= beta 3=gamma 4=delta 5=theta 6=mu
		double stocha_values[12][6];

		// traitement des donnees brutes avec decoupage mensuel, normalisation
		// et copie locale (ces copies sont a ranger dans user/ wind,solar,load/ "areaname.txt")
		for (uint j = 0; j != 12; ++j)
		{
			// Alias to the stochastics values of the month
			double* stocha = stocha_values[j];
			double* moments_centr_mois = moments_centr[areaIndex].data[j];

			Extrait_bloc(SERIE_N, posmois[j], 0, MTRX, durmois[j], NBS);

			// si on travaille avec les donnees brutes on les recopie et la serie de
			// translation vaut zero, sinon on calcule la moyenne pluis les ecarts
			if (!info.rawData)
			{
				// buffer_n will be reseted. No need to initialize it with memset.
				Colonne_moyenne(buffer_n, SERIE_N, durmois[j], NBS);
				Retranche_mtrx(SERIE_N, buffer_n, durmois[j], NBS);
			}
			else
				(void)::memset(buffer_n, 0, sizeof(buffer_n));


			// buffer_n contient la serie de translation et SERIE_N les donnees à normaliser
			pStr.clear() << folder << SEP << "translation-m";
			if (j < 10)
				pStr << '0';
			pStr << j << ".txt";
			// To reduce the size of each file, and consequently speed-up their loading
			// we will use the standard matrix of Antares, which perform a lot of
			// optimization when writing to a file.
			mtrxWriter.resize(1, durmois[j]);
			mtrxWriter.pasteToColumn(0, buffer_n);
			if (!mtrxWriter.saveToCSVFile(pStr, 3))
				return false;

			// On calcule les valeurs extrêmes du processus brut
			Mtrx_bound(minimu_brut[j], maximu_brut[j], SERIE_N, durmois[j], NBS);

			// On calcule ensuite les coefficients horaires de modulation pour le mois courant
			{
				Mtrx_abs(SERIE_P, SERIE_N, durmois[j], NBS);
				Colonne_moyenne(buffer_p, SERIE_P, durmois[j], NBS);
				const double expect = Moyenne_generale(buffer_p, durmois[j]);
				double ratios[24];

				for (uint n = 0; n < 24; ++n)
				{
					double d  = 0;
					for (uint m = 0; m < lonmois[j]; ++m)
						d += (buffer_p[24 * m + n] / lonmois[j]);
					ratios[n] = d / expect;
				}
				modulation.pasteToColumn(j, ratios);

				// il faut normaliser les series avant de les reecrire
				for (uint n = 0; n < durmois[j]; ++n)
				{
					const uint mod = n % 24;
					for (uint m = 0; m < NBS; ++m)
					{
						if (ratios[mod] != 0)
							SERIE_N.entry[m][n] /= ratios[mod];
					}
				}
			}

			// copie locale des donnees mormalisees dans study/user/wind,solar,load/ ...
			{
				pStr.clear() << folder << SEP << "userfile-m";
				if (j < 10)
					pStr << '0';
				pStr << j << ".txt";
				SERIE_N.height = durmois[j];
				if (!SERIE_N.saveToCSVFile(pStr, 3))
					return false;

				SERIE_N.height = 744; // restore the previous value
			}


			// calcul de l'esperance, de la variance et des valeurs extremes pour la
			// zone et le mois
			double expect_global = 0.;
			double varian_global = 0.;
			// premiere boucle sur les séries pour calcul des esperances et variances
			for (uint m = 0; m < NBS; ++m)
			{
				Extrait_col(buffer_n, SERIE_N, durmois[j], m);
				expect_global += Moyenne_generale(buffer_n,durmois[j]) / NBS;

				Square(buffer_p, buffer_n, durmois[j]);
				varian_global += Moyenne_generale(buffer_p,durmois[j]) / NBS;
			}

			double variance = varian_global - expect_global*expect_global;
			if (variance < 0.)
				variance = 0.; // si bruit numerique
			const double standard = sqrt(variance);


			// calcul des moments d'ordre 3 et 4
			double skewne_global = 0.;
			double kurtos_global = 0.;

			// passage aux valeurs centrees reduites si l'ecart-type n'est pas nul
			if (standard > 0.)
			{
				Retranche_scalaire(SERIE_P,SERIE_N,expect_global,744,NBS);
				Divise_scalaire(SERIE_Q,SERIE_P, standard, 744, NBS);

				// seconde boucle sur les series pour calcul des skewness et kurtosis
				for (uint m = 0; m < NBS; ++m)
				{
					Extrait_col(buffer_n,SERIE_Q,durmois[j],m);
					Cube(buffer_p,buffer_n,durmois[j]);
					skewne_global += Moyenne_generale(buffer_p, durmois[j]) / NBS;

					Dsquare(buffer_p, buffer_n, durmois[j]);
					kurtos_global += Moyenne_generale(buffer_p, durmois[j]) / NBS;
				}
			}

			// mise a jour du tableau general des moments (NBZ X 12 X 4)
			moments_centr_mois[0] = expect_global;
			moments_centr_mois[1] = standard;
			moments_centr_mois[2] = skewne_global;
			moments_centr_mois[3] = kurtos_global - 3;

			// Calcul des autocorrelations a l'aide de deux estimateurs AUTO_ESTIM1
			// (pour theta) et AUTO_ESTIM2 (pour mu).
			//
			// AUTO_ESTIM1 et AUTO_ESTIM2 convergent vers des valeurs identiques pour
			// des series longues et nombreuses
			(void)::memset(AUTO_ESTIM1, 0, sizeof(AUTO_ESTIM1));
			(void)::memset(AUTO_ESTIM2, 0, sizeof(AUTO_ESTIM2));

			AUTO_ESTIM1[0] = 1.; // autocorrelation (Xt, Xt + 0) = 100%
			AUTO_ESTIM2[0] = 1.; // autocorrelation (Xt, Xt + 0) = 100%

			for (uint m = 0; m < NBS; ++m)
			{
				Extrait_col(buffer_n, SERIE_N, durmois[j],  m);
				Extrait_seg(buffer_p, buffer_n, durmois[j], 0);

				// commence a 1 car AUTOCOR[0] deja calcule
				for (uint n = 1; n < PRA; ++n)
				{
					Extrait_seg(buffer_q,buffer_n,durmois[j]-(n+1),n);

					// les correlations de chaque paire (buffer_n, buffer_q)
					// pour chacune des NBS séries contribuent à la corrélation (X0,X0+n)*/
					//
					// 20/07/2010 : on inhibe le calcul de AUTO_ESTIM1 car toujours moins bon
					//   que ESTIM2 dans sa formulation actuelle

					// xx = Correlation(buffer_p,buffer_q,durmois[j]-(n+1),moments_centr[i][j][0],moments_centr[i][j][0],moments_centr[i][j][1],moments_centr[i][j][1],1);
					double yy = Correlation(buffer_p, buffer_q, durmois[j] - (n + 1), 0, 0, 0, 0, 0);
					//	AUTO_ESTIM1[n] += xx/NBS;
					AUTO_ESTIM2[n] += yy / NBS;
				}
			}


			// identification de theta et mu.
			// On tient compte de HOR si on n'analyse pas une loi normale
			if (info.distribution != Data::XCast::dtNormal)
				Analyse_auto(AUTO_ESTIM2, PRA, AUC, AUM, (double)HOR, stocha[4], stocha[5]);
			else
				Analyse_auto(AUTO_ESTIM2, PRA, AUC, AUM, 0,           stocha[4], stocha[5]);

			// On constate que ESTIM2 conduit à surévaluer theta, on corrige le biais
			// avec une fonction empirique
			if (stocha[4] > 0.002)
				stocha[4] -= 0.04 * sqrt(stocha[4]);

			// si mu>1 il faut majorer l'ecart-type observe  sur les valeurs lissees
			// pour remonter a l'ecart-type des valeurs des series non-lissees
			double standard_majore = (Math::Abs(stocha[5] - 1.) < 1e-6)
				? standard
				: standard / Standard_shrinkage((int)stocha[5], exp(-stocha[4]));

			// identification des alpha, beta, gamma, delta
			if (!Probab_density_funct(minimu_brut[j], maximu_brut[j], expect_global, standard_majore,
				info.distribution, stocha[0], stocha[1], stocha[2], stocha[3]))
			{
				logs.error() << "Area " << info.name << ", month " << (j + 1)
					<< ": fitting impossible for the chosen type of law";
				return false;
			}

		} // each month


		// Monthly Modulation coefficients
		{
			logs.info() << "  Exporting daily profile";
			pStr.clear() << folder << SEP << "diurmodulation.txt";
			modulation.saveToCSVFile(pStr);
			pStr.clear() << pStudyFolder << SEP << "input" << SEP << tsName
				<< SEP << "prepro" << SEP << info.name << SEP << "k.txt";
			//	pStr.clear() << folder << SEP << "diurmodulation.txt";
			modulation.saveToCSVFile(pStr);
		}

		// Coefficients
		{
			// ecriture du bloc des parametres des lois marginales
			MTRX.resize(6, 12);
			for (uint x = 0; x != 6; ++x)
			{
				auto& out = MTRX[x];
				for (uint y = 0; y != 12; ++y)
					out[y] = stocha_values[y][x];
			}

			// Override MU coefficients, to have mu [1..23], and not 24
			auto& mu = MTRX[Data::XCast::dataCoeffMu];
			for (uint y = 0; y != 12 /*MTRX.height*/; ++y)
			{
				if (mu[y] > 23.f)
					mu[y] = 23.f;
			}

			logs.info() << "  Exporting coefficients";
			pStr.clear() << folder << SEP << "stochpara.txt";
			MTRX.saveToCSVFile(pStr);

			pStr.clear() << pStudyFolder << SEP << "input" << SEP << tsName
				<< SEP << "prepro" << SEP << info.name << SEP << "data.txt";
			MTRX.saveToCSVFile(pStr);
		}

		// Conversion
		logs.info() << "  The conversion will be disabled but the data will remain untouched.";

		// Translation
		{
			logs.info() << "  Exporting translation data";
			mtrxWriter.reset(1, 8760);

			uint y = 0;
			for (uint m = 0; m != 12; ++m)
			{
				pStr.clear() << folder << SEP << "translation-m";
				if (m < 10)
					pStr << '0';
				pStr << m << ".txt";

				if (!MTRX.loadFromCSVFile(pStr, 1, durmois[m], Matrix<>::optImmediate|Matrix<>::optFixedSize))
				{
					logs.error() << "Impossible to reload " << pStr;
					continue;
				}

				auto& src = MTRX[0];
				auto& dst = mtrxWriter[0];
				for (uint j = 0; j != MTRX.height; ++j)
				{
					dst[y] = src[j];
					++y;
				}
			}

			pStr.clear() << pStudyFolder << SEP << "input" << SEP << tsName
				<< SEP << "prepro" << SEP << info.name << SEP << "translation.txt";
			if (!mtrxWriter.saveToCSVFile(pStr))
				logs.error() << "Impossible to write " << pStr;
		}

		// Settings for the TS generator (XCast)
		{
			logs.info() << "  Updating the TS-Generator settings";
			pStr.clear() << pStudyFolder << SEP << "input" << SEP << tsName
				<< SEP << "prepro" << SEP << info.name << SEP << "settings.ini";
			IO::File::Stream f;
			if (!f.open(pStr, IO::OpenMode::write | IO::OpenMode::truncate))
				logs.error() << "Impossible to create " << pStr;
			else
			{
				// Example : [general]
				// distribution = Beta
				// capacity = 0.000000
				// conversion = false
				// translation = never
				//
				f << "[general]"
					<< "\ndistribution = " << Data::XCast::DistributionToNameID(info.distribution)
					<< "\ncapacity = 1"
					<< "\nconversion = false";
				if (info.rawData)
					f << "\ntranslation = never\n";
				else
					f << "\ntranslation = before-conversion\n";
			}
		}

		return true;
	}



} // namespace Antares
