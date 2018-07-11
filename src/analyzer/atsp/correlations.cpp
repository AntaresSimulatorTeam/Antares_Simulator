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
#include <antares/date.h>
#include "../solver/misc/matrix-dp-make.h"


using namespace Yuni;


#define SEP  Yuni::IO::Separator


namespace Antares
{


	bool ATSP::computeMonthlyCorrelations()
	{
		logs.checkpoint() << "Monthly correlation values";

		// Prepare the cache
		cacheCreate();

		// pre-cleaning, just in case
		SERIE_N.clear();
		SERIE_P.clear();
		SERIE_Q.clear();

		// The real number of areas
		const uint realAreaCount = pEnabledAreaCount;

		// stockage d'une matrice mensuelle de correlations inter-zones
		Matrix<> CORR_MNP;
		CORR_MNP.reset(realAreaCount, realAreaCount);
		// stockage de la matrice annuelle obtenue par moyenne des correlations mensuelles
		Matrix<> CORR_YNP;
		CORR_YNP.reset(realAreaCount, realAreaCount);

		Matrix<> ID;
		ID.reset(realAreaCount, realAreaCount);
		ID.fillUnit();

		Matrix<> resultNDP;
		resultNDP.reset(realAreaCount, realAreaCount);
		Matrix<> tmpNDP;
		tmpNDP.reset(realAreaCount, realAreaCount);

		double* tmpArray = new double[realAreaCount + 1];

		// Initialize mapping, to skip areas which has been disabled
		// the real number of items is `realAreaCount`
		uint* mapping = new uint [pArea.size()];
		{
			uint z = 0;
			for (uint i = 0; i != pArea.size(); ++i)
			{
				mapping[i] = (uint) -1;
				if (pArea[i]->enabled)
				{
					mapping[z] = i;
					++z;
				}
			}
		}

		// Buffer for reading matrices
		Matrix<>::BufferType buffer;

		//
		size_t sizePerMatrix = 744 * pTimeseriesCount * sizeof(double);

		for (uint m = 0; m < 12; ++m)
		{
			logs.info() << "Correlation: Precaching data for " << Antares::Date::MonthToString(m);
			cacheClear();

			{
				uint iZ = realAreaCount;
				do
				{
					if (pCacheMemoryUsed + sizePerMatrix > pLimitMemory)
						break;
					--iZ;
					const uint i = mapping[iZ];
					const String& folder = folderPerArea[i];
					pStr.clear() << folder << SEP << "userfile-m";
					if (m < 10)
						pStr << '0';
					pStr << m << ".txt";

					if (!cachePreload(i, pStr, durmois[m], buffer))
						break;
				}
				while (iZ);
			}

			for (uint iZ = 0; iZ < realAreaCount; ++iZ)
			{
				const uint i = mapping[iZ];

				// The folder for the area
				const String& folder = folderPerArea[i];


				if (!cacheFetch(i, SERIE_N))
				{
					pStr.clear() << folder << SEP << "userfile-m";
					if (m < 10)
						pStr << '0';
					pStr << m << ".txt";
					if (!SERIE_N.loadFromCSVFile(pStr, NBS, durmois[m], Matrix<>::optImmediate|Matrix<>::optFixedSize, &buffer))
					{
						logs.error() << "impossible to open " << pStr;
						continue;
					}
				}

				CORR_MNP.entry[iZ][iZ] = 1.;
				for (uint jZ = iZ + 1; jZ < realAreaCount; ++jZ)
				{
					const uint j = mapping[jZ];
					// The folder for the area
					const String& folderJ = folderPerArea[j];

					logs.info() << "Correlation: month: " << Antares::Date::MonthToString(m)
						<< ", area " << (1+iZ) << '/' << realAreaCount << ": " << pArea[i]->name
						<< " with area " << (1+jZ) << '/' << realAreaCount << ": " << pArea[j]->name;

					if (!cacheFetch(j, SERIE_P))
					{
						pStr.clear() << folderJ << SEP << "userfile-m";
						if (m < 10)
							pStr << '0';
						pStr << m << ".txt";
						if (!SERIE_P.loadFromCSVFile(pStr, NBS, durmois[m], Matrix<>::optImmediate|Matrix<>::optFixedSize, &buffer))
							continue;
					}


					double coeff = 0.;
					for (uint q = 0; q < NBS; ++q)
					{
						Extrait_col(buffer_n, SERIE_N, durmois[m], q);
						Extrait_col(buffer_p, SERIE_P, durmois[m], q);
						const double xx = Correlation(buffer_n, buffer_p, durmois[m],
							moments_centr[i].data[m][0], moments_centr[j].data[m][0],
							moments_centr[i].data[m][1], moments_centr[j].data[m][1], 0);

						coeff += xx / NBS;
					}

					// The rounding must be done later, otherwise CORR_YNP will
					// be rounding as well
					CORR_MNP.entry[iZ][jZ] = coeff;
					CORR_MNP.entry[jZ][iZ] = coeff;
				}
			}

			for (uint i = 0; i < realAreaCount; ++i)
			{
				Matrix<>::ColumnType& outcol = CORR_YNP.entry[i];
				const Matrix<>::ColumnType& col = CORR_MNP.entry[i];
				for (uint j = 0; j < realAreaCount; ++j)
					outcol[j] += col[j] / 12.;
			}

			// Rounding monthly correlation coefficients (matrix trimming)
			for (uint i = 1; i < CORR_MNP.width; ++i)
			{
				auto& column = CORR_MNP[i];
				for (uint j = 0; j < i; ++j)
				{
					if (Math::Abs(column[j]) < RTZ)
					{
						column[j] = 0.;
						CORR_MNP[j][i] = 0.;
					}
					else if (column[j] > 1.)
					{
						column[j] = 1.;
						CORR_MNP[j][i] = 1.;
					}
					else if (column[j] < -1.)
					{
						column[j] = -1.;
						CORR_MNP[j][i] = -1.;
					}
				}
			}

			// Writing the monthly correlation coefficients
			resultNDP.zero();

			pStr.clear() << pTemp << SEP << tsName << SEP << "origin-correlation-m";
			if (m < 10)
				pStr << '0';
			pStr << m << ".txt";
			CORR_MNP.saveToCSVFile(pStr);


			// Converting into an admissible matrix
			double ret = Solver::MatrixDPMake<double>(tmpNDP.entry, CORR_MNP.entry, resultNDP.entry, ID.entry, ID.width, tmpArray);
			if (ret < 1.)
			{
				if (ret <= -1.)
				{
					logs.error() << "invalid data, can not be processed";
				}
				else
				{
					logs.warning() << "TS-Analyzer: " << Date::MonthToString(m) << ": correlation matrix was shrinked by " << ret
						<< ". Trimming threshold may be too high";
				}
			}

			// copying results
			for (uint y = 1; y < resultNDP.height; ++y)
			{
				for (uint x = 0; x < y; ++x)
					resultNDP[x][y] = resultNDP[y][x];
			}

			pStr.clear() << pTemp << SEP << tsName << SEP << "correlation-m";
			if (m < 10)
				pStr << '0';
			pStr << m << ".txt";
			resultNDP.saveToCSVFile(pStr);
		}

		// Memory cleaning
		buffer.clear().shrink();
		SERIE_N.clear();
		SERIE_P.clear();
		SERIE_Q.clear();
		CORR_MNP.clear();
		moments_centr.clear();
		cacheDestroy();


		// Rounding annual correlation coefficients
		for (uint i = 1; i < CORR_YNP.width; ++i)
		{
			auto& column = CORR_YNP[i];
			for (uint j = 0; j < i; ++j)
			{
				if (Math::Abs(column[j]) < RTZ)
				{
					column[j] = 0.;
					CORR_YNP[j][i] = 0.;
				}
				else if (column[j] > 1.)
				{
					column[j] = 1.;
					CORR_YNP[j][i] = 1.;
				}
				else if (column[j] < -1.)
				{
					column[j] = -1.;
					CORR_YNP[j][i] = -1.;
				}
			}
		}

		// Annual correlation coefficients
		pStr.clear() << pTemp << SEP << tsName << SEP << "origin-correlation-year.txt";
		CORR_YNP.saveToCSVFile(pStr);

		// Converting into an admissible matrix
		double ret = Solver::MatrixDPMake<double>(tmpNDP.entry, CORR_YNP.entry, resultNDP.entry, ID.entry, ID.width, tmpArray);
		if (ret < 1.)
		{
			if (ret <= -1.)
			{
				logs.error() << "invalid data, can not be processed";
			}
			else
			{
				logs.warning() << "TS-Analyzer: annual: correlation matrix was shrinked by "
					<< ret << ". Trimming threshold may be too high";
			}
		}
		for (uint y = 1; y < resultNDP.height; ++y)
		{
			for (uint x = 0; x < y; ++x)
				resultNDP[x][y] = resultNDP[y][x];
		}

		// Annual correlation coefficients
		pStr.clear() << pTemp << SEP << tsName << SEP << "correlation-year.txt";
		resultNDP.saveToCSVFile(pStr);


		{
			// We won't use the class `Correlation` here
			// because it would be memory hungry
			CString<ant_k_area_name_max_length*2+1, false> key;

			pStr.clear() << pStudyFolder << SEP << "input" << SEP << tsName
				<< SEP << "prepro" << SEP << "correlation.ini";
			IO::File::Stream f;
			if (not f.open(pStr, IO::OpenMode::write | IO::OpenMode::truncate))
			{
				logs.error() << "Impossible to create " << pStr;
			}
			else
			{
				f << "[general]\n";
				f << "mode = monthly\n\n";

				f << "[annual]\n";
				for (uint iZ = 0; iZ != realAreaCount; ++iZ)
				{
					const uint i = mapping[iZ];
					//const auto& col = CORR_YNP.entry[iZ];
					const auto& col = resultNDP[iZ];
					for (uint jZ = iZ + 1; jZ < realAreaCount; ++jZ)
					{
						if (not Math::Zero(col[jZ]))
						{
							const uint j = mapping[jZ];
							f << pArea[i]->name << '%' << pArea[j]->name << " = " << col[jZ] << '\n';
						}
					}
				}
				f << '\n';
				CORR_YNP.clear();

				for (uint m = 0; m != 12; ++m)
				{
					pStr.clear() << pTemp << SEP << tsName << SEP << "correlation-m";
					if (m < 10)
						pStr << '0';
					pStr << m << ".txt";

					if (not CORR_MNP.loadFromCSVFile(pStr, realAreaCount, realAreaCount, Matrix<>::optImmediate|Matrix<>::optFixedSize))
					{
						logs.error() << "Impossible to reload " << pStr;
						continue;
					}

					f << '[' << m << "]\n";
					for (uint iZ = 0; iZ < realAreaCount; ++iZ)
					{
						const uint i = mapping[iZ];
						const auto& col = CORR_MNP.entry[iZ];
						for (uint jZ = iZ + 1; jZ < realAreaCount; ++jZ)
						{
							if (not Math::Zero(col[jZ]))
							{
								const uint j = mapping[jZ];
								f << pArea[i]->name << '%' << pArea[j]->name << " = " << col[jZ] << '\n';
							}
						}
					}
					f << '\n';
				}
			}
		}

		// Writing the index
		{
			IO::File::Stream f;
			pStr.clear() << pTemp << SEP << tsName << SEP << "arealist.txt";
			if (f.openRW(pStr))
			{
				for (uint iZ = 0; iZ != realAreaCount; ++iZ)
				{
					const uint i = mapping[iZ];
					f << pArea[i]->name << '\n';
				}
			}
			else
				logs.error() << "Impossible to create " << pStr;
		}

		// removing the mapping list
		delete mapping;

		return true;
	}






} // namespace Antares
