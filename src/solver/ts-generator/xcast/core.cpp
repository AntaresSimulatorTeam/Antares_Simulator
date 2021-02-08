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
#include <yuni/core/math.h>
#include <antares/study.h>
#include <antares/logs.h>
#include "xcast.h"
#include "constants.h"
#include "../../misc/cholesky.h"
#include "../../misc/matrix-dp-make.h"
#include "math.hxx"

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
{
bool XCast::generateValuesForTheCurrentDay()
{
    enum
    {
        nbHoursADay = 24,
    };

    // The number of processes
    uint processCount = (uint)pData.localareas.size();
    // shrink
    float shrink;

    // temporary variables
    float x;

    // NDP
    uint Compteur_ndp = 0;

    // si le code est appele pour la premiere fois, on initialise tous les
    // processus par l'esperance des lois marginales
    if (pNeverInitialized)
    {
        pNeverInitialized = false;
        pNewMonth = true;
        for (uint s = 0; s != processCount; ++s)
        {
            if (!verification(A[s], B[s], G[s], D[s], L[s], T[s]))
            {
                logs.error() << "TS " << pTSName << " generator: invalid local parameters ("
                             << (s + 1) << ')';
                return false;
            }
            // Il s'agit d'une position relative par rapport a l'esperance
            POSI[s] = 0.f;
        }
        // if all processes involve normal law and "accuracy" is high, special simplications will be
        // made further on
        All_normal = false;
        if (pAccuracyOnCorrelation)
        {
            All_normal = true;
            for (uint s = 0; s != processCount; ++s)
            {
                if (L[s] != 3)
                    All_normal = false; // s is not a Normal process
            }
        }
        if (All_normal)
            pAccuracyOnCorrelation = false; // standard accuracy is high accuracy
    }

    // si les parametres ont change on reinitialise certaines variables intermediaires
    if (pNewMonth)
    {
        if (Cholesky<float>(Triangle_courant, pCorrMonth->entry, processCount, pQCHOLTotal))
        {
            // C n'est pas sdp, mais peut-etre proche de sdp
            // on tente un abattement de 0.999
            for (uint i = 0; i != processCount; ++i)
            {
                // on ne traite qu'en dessous de la diagonale et celle-ci n'a pas change (=1
                // partout)
                for (uint j = 0; j < i; ++j)
                    pCorrMonth->entry[i][j] *= 0.999f;
            }

            if (Cholesky<float>(Triangle_courant, pCorrMonth->entry, processCount, pQCHOLTotal))
            {
                // la matrice C n'est pas admissible, on abandonne
                logs.error() << "TS " << pTSName << " generator: invalid correlation matrix";
                return false;
            }
        }

        for (uint s = 0; s != processCount; ++s)
        {
            MAXI[s] = maximum(A[s], B[s], G[s], D[s], L[s]);
            MINI[s] = 0.f; // minimum(   A[s], B[s], G[s], D[s], L[s]);
            ESPE[s] = esperance(A[s], B[s], G[s], D[s], L[s]);
            STDE[s] = standard(A[s], B[s], G[s], D[s], L[s]);

            Presque_maxi[s] = ESPE[s] + (1.f - EPSIBOR) * (MAXI[s] - ESPE[s]);
            Presque_mini[s] = ESPE[s] + (1.f - EPSIBOR) * (MINI[s] - ESPE[s]);

            if (Presque_mini[s] > Presque_maxi[s])
            {
                // les bornes d'ecretement du processus n'encadrent pas l'esperance
                // de sa loi marginale
                logs.error() << "TS " << pTSName << " generator: invalid local parameters";
                return false;
            }
            D_COPIE[s] = diffusion(A[s], B[s], G[s], D[s], L[s], T[s], ESPE[s]);
        }

        if (All_normal) // special initialization
        {
            for (uint s = 0; s != processCount; ++s)
            {
                ALPH[s] = float(exp(-T[s]));
                BETA[s] = float(sqrt(1 - ALPH[s] * ALPH[s]));
                BASI[s] = (1.f - ALPH[s]) * ESPE[s];
            }
        }
        if (All_normal)
        {
            // assessement of a correlation matrix suitable for the month
            for (uint s = 0; s != processCount; ++s)
            {
                for (uint t = 0; t < s; ++t)
                {
                    x = T[s] * T[t] * STDE[s] * STDE[t];
                    float z = D_COPIE[t] * STDE[s];
                    if (Math::Zero(x))
                        CORR[s][t] = 0.f;
                    else
                    {
                        x = 1.f - ALPH[s] * ALPH[t];
                        x /= BETA[s];
                        x /= BETA[t];
                        CORR[s][t] = (*pCorrMonth)[s][t] * x;
                        if (CORR[s][t] > 1.f)
                        {
                            CORR[s][t] = 1.f;
                            ++pLevellingCount;
                        }
                        else
                        {
                            if (CORR[s][t] < -1.f)
                            {
                                CORR[s][t] = -1.f;
                                ++pLevellingCount;
                            }
                        }
                    }
                }

                // plus loin Mtrx_dp_make  a besoin de savoir que la diagonale vaut 1
                CORR[s][s] = 1.f;
            }
        }
        else
        {
            // on calcule une ebauche de matrice utilisable pour tout le mois dans le cas ou
            // accuracy =0
            for (uint s = 0; s != processCount; ++s)
            {
                for (uint t = 0; t < s; ++t)
                {
                    x = T[s] * T[t] * STDE[s] * STDE[t];
                    float z = D_COPIE[t] * STDE[s];
                    if (Math::Zero(x))
                        CORR[s][t] = 0.f;
                    else
                    {
                        x = D_COPIE[s] * STDE[t] / z;
                        CORR[s][t] = (*pCorrMonth)[s][t] * (x + 1.f / x) / 2.f;
                        if (CORR[s][t] > 1.f)
                        {
                            CORR[s][t] = 1.f;
                            ++pLevellingCount;
                        }
                        else
                        {
                            if (CORR[s][t] < -1.f)
                            {
                                CORR[s][t] = -1.f;
                                ++pLevellingCount;
                            }
                        }
                    }
                }

                // plus loin Mtrx_dp_make  a besoin de savoir que la diagonale vaut 1
                CORR[s][s] = 1.f;
            }
        }

        // calcul et factorisation de la matrice  du mois
        shrink = MatrixDPMake<float>(
          Triangle_courant, CORR, Carre_reference, pCorrMonth->entry, processCount, pQCHOLTotal);
        if (shrink == -1.f)
        {
            // sortie impossible  car on a vérifié que C est d.p
            logs.error() << "TS " << pTSName << " generator: invalid correlation matrix";
            return false;
        }
        // sert pour le decompte final des matrices ndp quand accuracy=0
        Compteur_ndp = (shrink < 1.f) ? 100 : 0;

        // calcul du pas de temps
        STEP = 1.f;
        if (!All_normal)
        {
            for (uint s = 0; s != processCount; ++s)
            {
                x = 1.f;
                if (T[s] > PETIT)
                    x = PETIT / T[s];
                if (x < STEP)
                {
                    // plafonne le terme lineaire de retour à la moyenne a PETIT *(ecart à la
                    // moyenne)
                    STEP = x;
                }

                x = maxiDiffusion(A[s], B[s], G[s], D[s], L[s], T[s]);
                if (x > 0.f)
                {
                    x = STDE[s] / x;
                    x *= x;
                    // plafonne l'amplitude de la diffusion à 2*sqrt(PETIT)*STDE (pour brown=1)
                    x *= 4.f * PETIT;
                    if (x < STEP)
                        STEP = x;
                }
            }
        }
        if (STEP < float(1e-2))
        {
            // on borne pour prevenir l'overflow
            STEP = float(1e-2);
            Nombre_points_intermediaire = 100;
        }
        else
        {
            // 1e-2 <= STEP <= 1.
            Nombre_points_intermediaire = (uint)(1.f / STEP);
            STEP = 1.f / float(Nombre_points_intermediaire);
        }

        SQST = sqrt(STEP);

        // traduction en position absolue (MINI,MAXI) des positions relatives (-1,+1)
        // des processus issues du dernier appel (ou de l'initialisation si premier
        // appel) en fonction des donnees du mois courant
        for (uint s = 0; s != processCount; ++s)
        {
            if (POSI[s] > 0.f)
                POSI[s] *= (MAXI[s] - ESPE[s]);
            else
                POSI[s] *= (ESPE[s] - MINI[s]);

            POSI[s] += ESPE[s];

            if (POSI[s] >= MAXI[s])
                POSI[s] = Presque_maxi[s];
            if (POSI[s] <= MINI[s])
                POSI[s] = Presque_mini[s];

            // on reinitialise la memoire du lissage pour eviter l'accumulation des derives
            if (M[s] > 1.f)
            {
                for (uint i = 0; i < nbHoursADay; ++i)
                {
                    LISS[s][i] = POSI[s] / M[s];
                }
                DATL[s][nbHoursADay - 1] = POSI[s];
            }
        }
    } // end new month
    else
    {
        for (uint s = 0; s != processCount; ++s)
        {
            if (POSI[s] > 0.f)
                POSI[s] *= (MAXI[s] - ESPE[s]);
            else
                POSI[s] *= (ESPE[s] - MINI[s]);

            POSI[s] += ESPE[s];

            if (POSI[s] >= MAXI[s])
                POSI[s] = Presque_maxi[s];
            if (POSI[s] <= MINI[s])
                POSI[s] = Presque_mini[s];
        }
    }

    // debut du calcul de la serie de points
    // pComputedPointCount = 0;
    // pNDPMatrixCount     = 0;
    // pLevellingCount     = 0;

    for (uint i = 0; i != nbHoursADay; ++i)
    {
        // recherche du prochain point horaire
        for (uint l = 0; l != Nombre_points_intermediaire; ++l)
        {
            ++pComputedPointCount;
            if (All_normal) // special simple case
            {
                // draw independent Nomal Variables
                uint j = processCount;
                if ((processCount - 2 * (processCount / 2)) != 0)
                    ++j;
                for (uint k = 0; k < j; ++k)
                    normal(WIEN[k], WIEN[j - (1 + k)]);

                // correlated brownian motions
                for (uint s = 0; s != processCount; ++s)
                {
                    BROW[s] = 0.f;
                    for (uint t = 0; t < s + 1; ++t)
                        BROW[s] += Triangle_courant[s][t] * WIEN[t];
                }

                // update processes positions
                for (uint s = 0; s != processCount; ++s)
                {
                    POSI[s] *= ALPH[s];
                    POSI[s] += BASI[s];
                    POSI[s] += BETA[s] * STDE[s] * BROW[s];

                    if (POSI[s] >= MAXI[s])
                        POSI[s] = Presque_maxi[s];
                    if (POSI[s] <= MINI[s])
                        POSI[s] = Presque_mini[s];
                }
            }
            else // standard case
            {
                // calcul des coefficients de diffusion
                for (uint s = 0; s != processCount; ++s)
                    DIFF[s] = diffusion(A[s], B[s], G[s], D[s], L[s], T[s], POSI[s]);

                // on calcule une matrice pour chaque point de passage
                if (pAccuracyOnCorrelation)
                {
                    // temporary variable for CORR[s][t]
                    float c;
                    float z;

                    for (uint s = 0; s != processCount; ++s)
                    {
                        float* corr_s = CORR[s];
                        auto& userMonthlyCorr = pCorrMonth->column(s);
                        for (uint t = 0; t < s; ++t)
                        {
                            if (Math::Zero(DIFF[s]) || Math::Zero(DIFF[t]))
                                corr_s[t] = 0;
                            else
                            {
                                z = DIFF[t] * STDE[s];
                                x = DIFF[s] * STDE[t] / z;
                                c = userMonthlyCorr[t] * (x + 1.f / x) / 2.f;

                                if (c > 1.f)
                                {
                                    c = 1.f;
                                    ++pLevellingCount;
                                }
                                else
                                {
                                    if (c < -1.f)
                                    {
                                        c = -1.f;
                                        ++pLevellingCount;
                                    }
                                }
                                corr_s[t] = c;
                            }
                        }
                        // plus loin Mtrx_dp_make  a besoin de savoir que la diagonale vaut 1
                        corr_s[s] = 1.f;
                    }

                    shrink = MatrixDPMake<float>(Triangle_courant,
                                                 CORR,
                                                 Carre_courant,
                                                 Carre_reference,
                                                 processCount,
                                                 pQCHOLTotal);
                    if (shrink <= 1.f)
                    {
                        if (shrink == -1.f)
                        {
                            // sortie impossible  car on a vérifié que C est d.p
                            logs.error()
                              << "TS " << pTSName << " generator: invalid correlation matrix";
                            return false;
                        }
                        if (shrink < 1.f)
                            ++pNDPMatrixCount;
                    }
                } // accuracy

                // tirage au sort de lois normales independantes
                uint j = processCount;
                if ((processCount - 2 * (processCount / 2)) != 0)
                    ++j;
                for (uint k = 0; k < j; ++k)
                    normal(WIEN[k], WIEN[j - (1 + k)]);

                // calcul des mouvements browniens correles
                for (uint s = 0; s != processCount; ++s)
                {
                    BROW[s] = 0.f;
                    for (uint t = 0; t < s + 1; ++t)
                        BROW[s] += Triangle_courant[s][t] * WIEN[t]; // chg 060610
                }

                // calcul des parametres directeurs du prochain mouvement
                for (uint s = 0; s != processCount; ++s)
                {
                    TREN[s] = T[s] * (ESPE[s] - POSI[s]);
                    DIFF[s] = DIFF[s] * BROW[s];
                }

                // mise a jour des positions
                for (uint s = 0; s != processCount; ++s)
                {
                    POSI[s] += (TREN[s] * STEP) + (DIFF[s] * SQST);
                    if (POSI[s] >= MAXI[s])
                        POSI[s] = Presque_maxi[s];
                    if (POSI[s] <= MINI[s])
                        POSI[s] = Presque_mini[s];
                }
            } // end of standard case
        }     // fin du point horaire

        for (uint s = 0; s != processCount; ++s)
        {
            float data_si;
            data_si = POSI[s] + G[s]; // translation pour revenir a l'origine reelle
            data_si *= FO[s][i];

            // si l'ecretement est active il faut envisager de corriger la trajectoire
            if (BO[s] == true)
            {
                if (data_si > MA[s])
                {
                    data_si = MA[s];
                    if (Math::Abs(FO[s][i]) > 0.f)
                    {
                        POSI[s] = MA[s] / FO[s][i];
                        POSI[s] -= G[s];
                        if (POSI[s] >= MAXI[s])
                            POSI[s] = Presque_maxi[s];
                        if (POSI[s] <= MINI[s])
                            POSI[s] = Presque_mini[s];
                    }
                }
                if (data_si < MI[s])
                {
                    data_si = MI[s];
                    if (Math::Abs(FO[s][i]) > 0.f)
                    {
                        POSI[s] = MI[s] / FO[s][i];
                        POSI[s] -= G[s];
                        if (POSI[s] >= MAXI[s])
                            POSI[s] = Presque_maxi[s];
                        if (POSI[s] <= MINI[s])
                            POSI[s] = Presque_mini[s];
                    }
                }
            }

            // si du lissage est a faire il faut evaluer DATL en valeurs glissantes
            if (M[s] > 1)
            {
                LISS[s][i] = POSI[s] / M[s];
                DATL[s][i] = DATL[s][(nbHoursADay + i - 1) % nbHoursADay];
                DATL[s][i] -= LISS[s][(nbHoursADay + i - M[s]) % nbHoursADay];
                DATL[s][i] += LISS[s][i];
                // translation pour revenir a l'origine puis homothetie
                data_si = FO[s][i] * (DATL[s][i] + G[s]);

                if (BO[s]) // si ecretement active
                {
                    if (data_si > MA[s])
                        data_si = MA[s];
                    if (data_si < MI[s])
                        data_si = MI[s];
                }
            }

            assert(0 == Math::Infinite(data_si) && "Infinite value");
            DATA[s][i] = data_si;
        }
    }

    // Reexpression des dernieres positions des processus en relatif par rapport
    // a l'intervalle mini, esperance,maxi cette reexpression permet d'assurer
    // la continuite avec l'appel suivant s'il correspond au debut d'un nouveau
    // mois de caractéristiques différentes.
    //
    // si espe <= posi <  maxi  on retient posi =  (posi - espe) / (maxi - espe)
    // si mini <  posi <= espe  on retient posi = -(posi - espe) / (mini - espe)
    //
    for (uint s = 0; s != processCount; ++s)
    {
        // >
        if (POSI[s] > ESPE[s])
            POSI[s] = (POSI[s] - ESPE[s]) / (MAXI[s] - ESPE[s]);
        else
        {
            // <
            if (POSI[s] < ESPE[s])
                POSI[s] = (ESPE[s] - POSI[s]) / (MINI[s] - ESPE[s]);
            else // =
                POSI[s] = 0;
        }
    }

    // fin de la serie
    if (!pAccuracyOnCorrelation && Compteur_ndp == 100)
        ++pNDPMatrixCount;

    return true;
}

} // namespace XCast
} // namespace TSGenerator
} // namespace Solver
} // namespace Antares
