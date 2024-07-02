/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <antares/date/date.h>
#include <antares/utils/utils.h>
#include "antares/solver/misc/matrix-dp-make.h"

#include "atsp.h"

using namespace Yuni;

#define SEP Yuni::IO::Separator

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

    // stockage d'une matrice mensuelle de correlations inter-zones, signal nul inclus
    Matrix<> CORR_MNPZ;
    CORR_MNPZ.reset(realAreaCount, realAreaCount);
    // stockage de la matrice annuelle obtenue par moyenne des correlations mensuelles, signal nul
    // inclus
    Matrix<> CORR_YNPZ;
    CORR_YNPZ.reset(realAreaCount, realAreaCount);

    // stockage d'une matrice mensuelle de correlations inter-zones, zeros exclus
    Matrix<> CORR_MNP;
    CORR_MNP.reset(realAreaCount, realAreaCount);
    // stockage de la matrice annuelle obtenue par moyenne des correlations mensuelles, zeros exclus
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
    uint* mapping = new uint[pArea.size()];
    {
        uint z = 0;
        for (uint i = 0; i != pArea.size(); ++i)
        {
            mapping[i] = (uint)-1;
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

    int Ni = 0;  // local counter used to remove the contribution of hours hidden by zeroes
    int Nj = 0;  // local counter used to remove the contribution of hours hidden by zeroes
    int Nij = 0; // local counter used to remove the contribution of hours hidden by zeroes
    int NZR = 0; // local counter used to remove the contribution of hours hidden by zeroes

    double shrink; // matrix adjustment factor to ensure sdp

    for (uint m = 0; m < 12; ++m)
    {
        logs.info() << "Correlation: Precaching data for " << Antares::Date::MonthToString(m);
        cacheClear();

        {
            uint iZ = realAreaCount;
            do
            {
                if (pCacheMemoryUsed + sizePerMatrix > pLimitMemory)
                {
                    break;
                }
                --iZ;
                const uint i = mapping[iZ];
                const String& folder = folderPerArea[i];
                pStr.clear() << folder << SEP << "userfile-m";
                if (m < 10)
                {
                    pStr << '0';
                }
                pStr << m << ".txt";

                if (!cachePreload(i, pStr, durmois[m], buffer))
                {
                    break;
                }
            } while (iZ);
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
                {
                    pStr << '0';
                }
                pStr << m << ".txt";
                if (!SERIE_N.loadFromCSVFile(pStr,
                                             NBS,
                                             durmois[m],
                                             Matrix<>::optImmediate | Matrix<>::optFixedSize,
                                             &buffer))
                {
                    logs.error() << "impossible to open " << pStr;
                    continue;
                }
            }

            CORR_MNP.entry[iZ][iZ] = 1.;
            CORR_MNPZ.entry[iZ][iZ] = 1.;
            for (uint jZ = iZ + 1; jZ < realAreaCount; ++jZ)
            {
                const uint j = mapping[jZ];
                // The folder for the area
                const String& folderJ = folderPerArea[j];

                logs.info() << "Correlation: month: " << Antares::Date::MonthToString(m)
                            << ", area " << (1 + iZ) << '/' << realAreaCount << ": "
                            << pArea[i]->name << " with area " << (1 + jZ) << '/' << realAreaCount
                            << ": " << pArea[j]->name;

                if (!cacheFetch(j, SERIE_P))
                {
                    pStr.clear() << folderJ << SEP << "userfile-m";
                    if (m < 10)
                    {
                        pStr << '0';
                    }
                    pStr << m << ".txt";
                    if (!SERIE_P.loadFromCSVFile(pStr,
                                                 NBS,
                                                 durmois[m],
                                                 Matrix<>::optImmediate | Matrix<>::optFixedSize,
                                                 &buffer))
                    {
                        continue;
                    }
                }

                double coeff = 0.;

                for (uint q = 0; q < NBS; ++q)
                {
                    Extrait_col(buffer_n, SERIE_N, durmois[m], q);
                    Extrait_col(buffer_p, SERIE_P, durmois[m], q);

                    /*					The following expression suits better data
                       including inter-annual oscillations (such data is out of  the scope of the
                       built-in generators that might be used afterwards)

                                                            const double xx = Correlation(buffer_n,
                       buffer_p, durmois[m], moments_centr_raw[i].data[m][0],
                       moments_centr_raw[j].data[m][0], moments_centr_raw[i].data[m][1],
                       moments_centr_raw[j].data[m][1], 1);
                    */

                    /*					The following expression suits better data
                       with recurrent annual behaviour (such data is within the scope of the
                       built-in generators that might be used afterwards)
                    */
                    const double xx = Correlation(buffer_n, buffer_p, durmois[m], 0, 0, 0, 0, 0);
                    coeff += xx / NBS;
                }

                // The rounding must be done later, otherwise CORR_YNP will
                // be rounding as well
                CORR_MNPZ.entry[iZ][jZ] = coeff;
                CORR_MNPZ.entry[jZ][iZ] = coeff;
                CORR_MNP.entry[iZ][jZ] = coeff;
                CORR_MNP.entry[jZ][iZ] = coeff;

                // The contribution of hidden hours (structural zeroes) in the time-series
                // needs to be removed
                // C(iz,jz)= C(jz,iz) =  coeff*raw correlation
                // with coeff = (24-Ni)^0.5*(24-Nj)^0.5/(24+Nij-Ni-Nj)
                // where Ni,Nj : hidden hours in i and j
                // Nij         : hours hidden in both i and j

                Ni = 0;
                Nj = 0;
                Nij = 0;

                for (uint n = 0; n < 24; ++n)
                {
                    Ni += hidden_hours[i].data[m][n];
                    Nj += hidden_hours[j].data[m][n];
                    Nij += hidden_hours[i].data[m][n] * hidden_hours[j].data[m][n];
                }
                NZR = 24 + Nij - (Ni + Nj);
                if (NZR > 0)
                {
                    coeff = (24. - double(Ni)) * (24. - double(Nj));
                    coeff = sqrt(coeff);
                    coeff /= double(NZR);
                    CORR_MNP.entry[iZ][jZ] = coeff * CORR_MNPZ.entry[iZ][jZ];
                    CORR_MNP.entry[jZ][iZ] = coeff * CORR_MNPZ.entry[jZ][iZ];
                }
            }
        }

        for (uint i = 0; i < realAreaCount; ++i)
        {
            Matrix<>::ColumnType& outcol = CORR_YNP.entry[i];
            const Matrix<>::ColumnType& col = CORR_MNP.entry[i];
            for (uint j = 0; j < realAreaCount; ++j)
            {
                outcol[j] += col[j] / 12.;
            }
        }
        for (uint i = 0; i < realAreaCount; ++i)
        {
            Matrix<>::ColumnType& outcol = CORR_YNPZ.entry[i];
            const Matrix<>::ColumnType& col = CORR_MNPZ.entry[i];
            for (uint j = 0; j < realAreaCount; ++j)
            {
                outcol[j] += col[j] / 12.;
            }
        }

        // Rounding monthly correlation coefficients (matrix trimming) -zero -excluded
        for (uint i = 1; i < CORR_MNP.width; ++i)
        {
            auto& column = CORR_MNP[i];
            for (uint j = 0; j < i; ++j)
            {
                if (std::abs(column[j]) < RTZ)
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
        // Rounding monthly correlation coefficients (matrix trimming) - zeros -included
        for (uint i = 1; i < CORR_MNPZ.width; ++i)
        {
            auto& column = CORR_MNPZ[i];
            for (uint j = 0; j < i; ++j)
            {
                if (std::abs(column[j]) < RTZ)
                {
                    column[j] = 0.;
                    CORR_MNPZ[j][i] = 0.;
                }
                else if (column[j] > 1.)
                {
                    column[j] = 1.;
                    CORR_MNPZ[j][i] = 1.;
                }
                else if (column[j] < -1.)
                {
                    column[j] = -1.;
                    CORR_MNPZ[j][i] = -1.;
                }
            }
        }

        // Writing the monthly correlation coefficients,including zeroes,  before sdp check

        pStr.clear() << pTemp << SEP << tsName << SEP << "origin-corr-zero_in-m";
        if (m < 10)
        {
            pStr << '0';
        }
        pStr << m << ".txt";
        CORR_MNPZ.saveToCSVFile(pStr);

        // Writing the monthly correlation coefficients, excluding zeroes, before sdp check
        pStr.clear() << pTemp << SEP << tsName << SEP << "origin-corr-zero_no-m";
        if (m < 10)
        {
            pStr << '0';
        }
        pStr << m << ".txt";
        CORR_MNP.saveToCSVFile(pStr);

        // Converting original matrix with zero signal into an admissible matrix
        resultNDP.zero();
        shrink = Solver::MatrixDPMake<double>(tmpNDP.entry,
                                              CORR_MNPZ.entry,
                                              resultNDP.entry,
                                              ID.entry,
                                              ID.width,
                                              tmpArray);
        if (shrink < 1.)
        {
            if (shrink <= -1.)
            {
                logs.error() << "invalid data, can not be processed";
            }
            else
            {
                logs.warning()
                  << "TS-Analyzer: " << Date::MonthToString(m)
                  << ": correlation matrix was shrinked by " << shrink
                  << ". to reach admissibility (stage 1)- Consider using lower trimming threshold";
            }
        }
        // Replace original matrix with zero by the admissible matrix
        for (uint y = 1; y < resultNDP.height; ++y)
        {
            for (uint x = 0; x < y; ++x)
            {
                resultNDP[x][y] = resultNDP[y][x];
            }
        }
        for (uint i = 1; i < CORR_MNPZ.width; ++i)
        {
            for (uint j = 0; j < i; ++j)
            {
                CORR_MNPZ[j][i] = resultNDP[j][i];
            }
        }

        // Converting original matrix without zero signal into an admissible matrix

        resultNDP.zero();
        shrink = Solver::MatrixDPMake<double>(tmpNDP.entry,
                                              CORR_MNP.entry,
                                              resultNDP.entry,
                                              CORR_MNPZ.entry,
                                              CORR_MNPZ.width,
                                              tmpArray);
        if (shrink < 1.)
        {
            if (shrink <= -1.) // CORR_MNPZ is too close to sdp boundary, shrink CORR_MNP instead
                               // Note : this default code should be replaced by proper eigen value
                               // analysis (see Higham 2002 / Strabic 2016)
            {
                shrink = Solver::MatrixDPMake<double>(tmpNDP.entry,
                                                      CORR_MNP.entry,
                                                      resultNDP.entry,
                                                      ID.entry,
                                                      ID.width,
                                                      tmpArray);
                if (shrink <= -1.)
                {
                    logs.error() << "invalid data, can not be processed";
                }
                else
                {
                    logs.warning() << "TS-Analyzer: " << Date::MonthToString(m)
                                   << ": correlation matrix was shrinked by " << shrink
                                   << ". to reach admissibility (stage 2) - Time-series may "
                                      "involve too many hours with no signal";
                }
            }
            else
            {
                logs.info() << "TS-Analyzer: " << Date::MonthToString(m)
                            << ": some terms of the correlation matrix were shrinked by " << shrink
                            << ". to reach admissibility (hidden signal padding)";
            }
        }

        // copying results
        for (uint y = 1; y < resultNDP.height; ++y)
        {
            for (uint x = 0; x < y; ++x)
            {
                resultNDP[x][y] = resultNDP[y][x];
            }
        }

        pStr.clear() << pTemp << SEP << tsName << SEP << "correlation-m";
        if (m < 10)
        {
            pStr << '0';
        }
        pStr << m << ".txt";
        resultNDP.saveToCSVFile(pStr);
    }

    // Memory cleaning
    buffer.clear().shrink();
    SERIE_N.clear();
    SERIE_P.clear();
    SERIE_Q.clear();
    CORR_MNP.clear();
    CORR_MNPZ.clear();
    moments_centr_net.clear();
    moments_centr_raw.clear();
    hidden_hours.clear();
    cacheDestroy();

    // Rounding annual correlation coefficients -zero excluded
    for (uint i = 1; i < CORR_YNP.width; ++i)
    {
        auto& column = CORR_YNP[i];
        for (uint j = 0; j < i; ++j)
        {
            if (std::abs(column[j]) < RTZ)
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
    // Rounding annual correlation coefficients -zero included
    for (uint i = 1; i < CORR_YNPZ.width; ++i)
    {
        auto& column = CORR_YNPZ[i];
        for (uint j = 0; j < i; ++j)
        {
            if (std::abs(column[j]) < RTZ)
            {
                column[j] = 0.;
                CORR_YNPZ[j][i] = 0.;
            }
            else if (column[j] > 1.)
            {
                column[j] = 1.;
                CORR_YNPZ[j][i] = 1.;
            }
            else if (column[j] < -1.)
            {
                column[j] = -1.;
                CORR_YNPZ[j][i] = -1.;
            }
        }
    }
    // Annual correlation coefficients -zero excluded
    pStr.clear() << pTemp << SEP << tsName << SEP << "origin-corr-zero_no-year.txt";
    CORR_YNP.saveToCSVFile(pStr);

    // Annual correlation coefficients -zero included
    pStr.clear() << pTemp << SEP << tsName << SEP << "origin-corr-zero_in-year.txt";
    CORR_YNPZ.saveToCSVFile(pStr);

    // Converting original matrix without zero signal into an admissible matrix

    shrink = Solver::MatrixDPMake<double>(tmpNDP.entry,
                                          CORR_YNPZ.entry,
                                          resultNDP.entry,
                                          ID.entry,
                                          ID.width,
                                          tmpArray);
    if (shrink < 1.)
    {
        if (shrink <= -1.)
        {
            logs.error() << "invalid data, can not be processed";
        }
        else
        {
            logs.warning()
              << "TS-Analyzer: annual: correlation matrix was shrinked by " << shrink
              << ". to reach admissibility (stage 1)- Consider using lower trimming threshold";
        }
    }

    // Replace original matrix with zero by the admissible matrix
    for (uint y = 1; y < resultNDP.height; ++y)
    {
        for (uint x = 0; x < y; ++x)
        {
            resultNDP[x][y] = resultNDP[y][x];
        }
    }
    for (uint i = 1; i < CORR_YNPZ.width; ++i)
    {
        for (uint j = 0; j < i; ++j)
        {
            CORR_YNPZ[j][i] = resultNDP[j][i];
        }
    }

    // Converting original matrix without zero signal into an admissible matrix
    shrink = Solver::MatrixDPMake<double>(tmpNDP.entry,
                                          CORR_YNP.entry,
                                          resultNDP.entry,
                                          CORR_YNPZ.entry,
                                          CORR_YNPZ.width,
                                          tmpArray);
    if (shrink < 1.)
    {
        if (shrink <= -1.) // CORR_YNP is too close to sdp boundary, shrink CORR_YNP instead
                           // Note : this default code should be replaced by proper eigen value
                           // analysis (see Higham 2002 / Strabic 2016)
        {
            shrink = Solver::MatrixDPMake<double>(tmpNDP.entry,
                                                  CORR_YNP.entry,
                                                  resultNDP.entry,
                                                  ID.entry,
                                                  ID.width,
                                                  tmpArray);
            if (shrink <= -1.)
            {
                logs.error() << "invalid data, can not be processed";
            }
            else
            {
                logs.warning() << "TS-Analyzer: annual: correlation matrix was shrinked by "
                               << shrink
                               << ". to reach admissibility (stage 2) - Time-series may involve "
                                  "too many hours with no signal";
            }
        }
        else
        {
            logs.info()
              << "TS-Analyzer: annual: some terms of the correlation matrix were shrinked by "
              << shrink << ". to reach admissibility (hidden signal padding)";
        }
    }
    for (uint y = 1; y < resultNDP.height; ++y)
    {
        for (uint x = 0; x < y; ++x)
        {
            resultNDP[x][y] = resultNDP[y][x];
        }
    }

    // Annual correlation coefficients
    pStr.clear() << pTemp << SEP << tsName << SEP << "correlation-year.txt";
    resultNDP.saveToCSVFile(pStr);

    {
        // We won't use the class `Correlation` here
        // because it would be memory hungry
        CString<ant_k_area_name_max_length * 2 + 1, false> key;

        pStr.clear() << pStudyFolder << SEP << "input" << SEP << tsName << SEP << "prepro" << SEP
                     << "correlation.ini";
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
                // const auto& col = CORR_YNP.entry[iZ];
                const auto& col = resultNDP[iZ];
                for (uint jZ = iZ + 1; jZ < realAreaCount; ++jZ)
                {
                    if (!Utils::isZero(col[jZ]))
                    {
                        const uint j = mapping[jZ];
                        f << pArea[i]->name << '%' << pArea[j]->name << " = " << col[jZ] << '\n';
                    }
                }
            }
            f << '\n';
            CORR_YNP.clear();
            CORR_YNPZ.clear();

            for (uint m = 0; m != 12; ++m)
            {
                pStr.clear() << pTemp << SEP << tsName << SEP << "correlation-m";
                if (m < 10)
                {
                    pStr << '0';
                }
                pStr << m << ".txt";

                if (not CORR_MNP.loadFromCSVFile(pStr,
                                                 realAreaCount,
                                                 realAreaCount,
                                                 Matrix<>::optImmediate | Matrix<>::optFixedSize))
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
                        if (!Utils::isZero(col[jZ]))
                        {
                            const uint j = mapping[jZ];
                            f << pArea[i]->name << '%' << pArea[j]->name << " = " << col[jZ]
                              << '\n';
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
        {
            logs.error() << "Impossible to create " << pStr;
        }
    }

    // removing the mapping list
    delete[] mapping;

    return true;
}

} // namespace Antares
