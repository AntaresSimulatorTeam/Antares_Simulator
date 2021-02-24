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
#include <assert.h>
#include <yuni/core/math.h>

using namespace Yuni;

//! Our value of PI
#define PI 3.141592653589793

namespace Antares
{
void ATSP::roundMatrixValues(Matrix<>& m)
{
    if (pUseUpperBound)
    {
        for (uint x = 0; x < m.width; ++x)
        {
            auto& src = m.entry[x];
            for (uint y = 0; y < m.height; ++y)
            {
                if (src[y] > pUpperBound)
                {
                    src[y] = pUpperBound;
                    ++pRoundingCount;
                }
                if (src[y] > pUpperBound80percent)
                    ++pRounding80percentCount;
            }
        }
        pRoundingCountTotal += m.width * m.height;
    }
    if (pUseLowerBound)
    {
        for (uint x = 0; x < m.width; ++x)
        {
            auto& src = m.entry[x];
            for (uint y = 0; y < m.height; ++y)
            {
                if (src[y] < pLowerBound)
                {
                    src[y] = pLowerBound;
                    ++pRoundingCount;
                }
            }
        }
        pRoundingCountTotal += m.width * m.height;
    }
}

void ATSP::Extrait_bloc(Matrix<>& out,
                        uint indlig,
                        uint indcol,
                        const Matrix<>& MTRX,
                        uint nblig,
                        uint nbcol)
{
    for (uint j = 0; j < nbcol; ++j)
    {
        auto& outcol = out.entry[j];
        const auto& srccol = MTRX.entry[j + indcol];
        for (uint i = 0; i < nblig; ++i)
            outcol[i] = srccol[i + indlig];
    }
}

void ATSP::Colonne_moyenne(double* out, const Matrix<>& MTRX, uint nblig, uint nbcol)
{
    assert(nbcol > 0 && "Invalid column count, it will produce a division by 0");

    double d;
    for (uint i = 0; i < nblig; ++i)
    {
        d = 0.;
        for (uint j = 0; j < nbcol; ++j)
            d += MTRX[j][i];
        out[i] = d / nbcol;
    }
}

/* retourne le maximum ou le minimum de MTRX selon le code */
void ATSP::Mtrx_bound(double& rmin, double& rmax, const Matrix<>& MTRX, uint nblig, uint nbcol)
{
    rmax = -1e+30;
    rmin = +1e+30;

    for (uint j = 0; j < nbcol; ++j)
    {
        auto& col = MTRX.entry[j];
        for (uint i = 0; i < nblig; ++i)
        {
            if (col[i] > rmax)
                rmax = col[i];
            if (col[i] < rmin)
                rmin = col[i];
        }
    }
}

// met dans OUT la valeur absolue de MTRX
void ATSP::Mtrx_abs(Matrix<>& out, const Matrix<>& MTRX, uint nblig, uint nbcol)
{
    for (uint j = 0; j < nbcol; ++j)
    {
        for (uint i = 0; i < nblig; ++i)
            out[j][i] = Math::Abs(MTRX[j][i]);
    }
}

// retourne la moyenne des éléments de A
double ATSP::Moyenne_generale(double* A, uint nblig)
{
    double x = 0.;
    for (uint i = 0; i < nblig; ++i)
        x += A[i];
    return x / nblig;
}

// retranche de MTRX le vecteur A
void ATSP::Retranche_mtrx(Matrix<>& MTRX, const double* A, uint nblig, uint nbcol)
{
    for (uint j = 0; j < nbcol; ++j)
    {
        Matrix<>::ColumnType& col = MTRX.entry[j];
        for (uint i = 0; i < nblig; ++i)
        {
            col[i] -= A[i];
        }
    }
}

// range dans OUT les nblig première lignes de la colonne de MTRX d'indice indcol
void ATSP::Extrait_col(double* out, const Matrix<>& MTRX, uint nblig, uint indcol)
{
    const Matrix<>::ColumnType& col = MTRX.entry[indcol];
    for (uint i = 0; i < nblig; ++i)
        out[i] = col[i];
}

// met dans A le carre de B
void ATSP::Square(double* A, const double* B, uint nblig)
{
    for (uint i = 0; i < nblig; ++i)
        A[i] = B[i] * B[i];
}

// met (B- le scalaire x) dans A
void ATSP::Retranche_scalaire(Matrix<>& A, const Matrix<>& B, double x, uint nblig, uint nbcol)
{
    for (uint j = 0; j < nbcol; ++j)
    {
        const Matrix<>::ColumnType& src = B.entry[j];
        auto& out = A.entry[j];
        for (uint i = 0; i < nblig; ++i)
            out[i] = src[i] - x;
    }
}

// met (B /scalaire x) dans A
void ATSP::Divise_scalaire(Matrix<>& A, const Matrix<>& B, double x, uint nblig, uint nbcol)
{
    for (uint j = 0; j < nbcol; ++j)
    {
        const auto& src = B.entry[j];
        auto& out = A.entry[j];

        for (uint i = 0; i < nblig; ++i)
            out[i] = src[i] / x;
    }
}

// met dans A le cube de B
void ATSP::Cube(double* A, const double* B, uint nblig)
{
    for (uint i = 0; i < nblig; ++i)
        A[i] = B[i] * B[i] * B[i];
}

// met dans A B^4
void ATSP::Dsquare(double* A, const double* B, uint nblig)
{
    for (uint i = 0; i < nblig; ++i)
        A[i] = B[i] * B[i] * B[i] * B[i];
}

// range dans OUT  nblig elements de MTRX pris a partir de l'indice indlig
void ATSP::Extrait_seg(double* out, const double* src, uint nblig, uint indlig)
{
    for (uint i = 0; i < nblig; ++i)
        out[i] = src[i + indlig];
}

// retourne le coefficient de corrélation entre A et B  (retourne 999 si paramètre "code" aberrant)
//   si code = 0 : les espérances et écarts-types des variables représentées par  A et B sont à
//   calculer si code = 1 : des estimations des espérances et écarts-types sont fournis dans
//   EA,EB,SA,SB
double ATSP::Correlation(double* A,
                         double* B,
                         uint nblig,
                         double EA,
                         double EB,
                         double SA,
                         double SB,
                         int code)
{
    double sigma_A = 0.;
    double sigma_B = 0.;
    double expec_A = 0.;
    double expec_B = 0.;
    double buffer[744];

    switch (code)
    {
    case 0:
    {
        expec_A = Moyenne_generale(A, nblig);
        expec_B = Moyenne_generale(B, nblig);

        // sigma A
        {
            Square(buffer, A, nblig);
            double x = Moyenne_generale(buffer, nblig) - expec_A * expec_A;
            sigma_A = (x > 1e-9) ? sqrt(x) : 0.;
        }
        // sigma B
        {
            Square(buffer, B, nblig);
            double x = Moyenne_generale(buffer, nblig) - expec_B * expec_B;
            sigma_B = (x > 1e-9) ? sqrt(x) : 0.;
        }
        break;
    }
    case 1:
    {
        expec_A = EA;
        expec_B = EB;
        sigma_A = SA;
        sigma_B = SB;
        break;
    }
    default:
        assert("ATSP::Correlation: invalid code");
        return 999.; // should never happen
    }

    if (Math::Abs(sigma_A) < 1e-4 && Math::Abs(sigma_B) < 1e-4)
        return 1.;
    if (Math::Abs(sigma_A) < 1e-4 || Math::Abs(sigma_B) < 1e-4)
        return 0.;

    double rho = 0.;

    if (code == 1)
    {
        for (uint i = 0; i < nblig; ++i)
            rho += (A[i] - expec_A) * (B[i] - expec_B);

        rho /= nblig;
        rho /= sigma_A;
        rho /= sigma_B;
    }
    else
    {
        // if (code == 0)

        for (uint i = 0; i < nblig; ++i)
            rho += A[i] * B[i];

        rho /= nblig;
        rho -= expec_A * expec_B;
        rho /= sigma_A;
        rho /= sigma_B;
    }

    if (rho > 1)
        rho = 1;
    if (rho < -1)
        rho = -1;

    return rho;
}

double ATSP::Ecart(double T1, double P1, double T2, double P2, int M, double T)
{
    const double R = exp(-T);
    const double PP1 = autocorr_average((int)T1, M, R);
    const double PP2 = autocorr_average((int)T2, M, R);

    double y = (P2 - PP2);
    double x = y * y;

    y = (P1 - PP1);
    x += y * y;

    return x;
}

double ATSP::autocorr_average(int H, int M, double R)
{
    if (R < 0.) // ceci ne doit jamais se produire
    {
        R = 0.;
    }

    double x = 0.;
    for (int i = 1; i < M + 1; ++i)
    {
        for (int j = 1 + H; j < M + 1 + H; ++j)
            x += pow(R, abs(j - i));
    }

    double y = 0.;
    for (int i = 1; i < M + 1; ++i)
    {
        for (int j = i + 1; j < M + 1; ++j)
            y += pow(R, j - i);
    }

    y *= 2.;
    y += M;

    return x / y;
}

void ATSP::Analyse_auto(double* A,
                        int nblig,
                        double auc,
                        double aum,
                        double hor,
                        double& theta,
                        double& mu)
{
    // calcul des heures TC et TM

    // initialisation necessaire au cas ou la fonction experimentale serait aberrante
    // heure pour laquelle A[TM] >= aum & A[TM+1] < aum
    int TM = ((int)nblig) - 1;
    // heure pour laquelle A[TC] >= auc & A[TC+1] < auc
    int TC = TM / 2;

    int max_mu = 24;

    enum
    {
        dicho = 20
    };

    // check thresholds aum & auc

    if (aum <= double(0.01))
        aum = double(0.01);
    if (auc < aum)
        auc = aum;

    // 1 address special cases

    // thresholds are different and experimental data lies strictly in-between
    if (aum < auc && A[1] < auc && A[nblig - 1] > aum)
    {
        theta = -log(A[TC]) / TC; // TC= int(0.5*(nblig-1))
        mu = 1;
        return;
    }

    // Experimental data lies below both thresholds
    if (A[1] <= aum)
    {
        theta = -log(A[1]) / double(1.);
        mu = 1;
        return;
    }
    // Experimental data lies above both thresholds
    if (A[nblig - 1] >= auc)
    {
        theta = -log(A[nblig - 1]) / double(nblig - 1);
        if (theta < double(0.0001))
            theta = 0;
        mu = 1;
        return;
    }
    // Experimental data crosses only the short-term threshold
    if (A[1] > auc && A[nblig - 1] > aum)
    {
        aum = auc; // keep only short-term and process later
    }
    // Experimental data crosses only long-term threshold
    if (A[1] < auc && A[nblig - 1] < aum)
    {
        auc = aum; // keep only long-term and process later
    }
    // Thresholds are identical and crossed by experimental data
    if (aum == auc)
    {
        double auto_threshold = aum;
        for (int l = nblig - 2; l > -1; --l)
        {
            //  long-term threshold crossed at l
            if (A[l] >= auto_threshold && A[l + 1] < auto_threshold)
            {
                theta = -log(A[l]) / double(l);
                mu = 1;
                return;
            }
        }
    }

    // 2 address regular case :thresholds are different and experimental data crosses both of them

    for (int l = nblig - 2; l > 0; --l)
    {
        // on trouve une bonne estimation pour TM
        if (A[l] >= aum && A[l + 1] < aum)
            TM = l;
        // on trouve une bonne estimation pour TC
        if (A[l] >= auc && A[l + 1] < auc)
            TC = l;
        // on cherchera ensuite une valeur de mu comprise entre 1 et max_mu
        if (A[l] >= hor && A[l + 1] < hor)
            max_mu = l + 1;
    }

    if (max_mu > 24)
        max_mu = 24;
    if (max_mu == 0)
        max_mu = 1;

    // both thresholds crossed at the same time

    if (TM == TC)
    {
        theta = -log(A[TC]) / double(TC);
        mu = 1;
        return;
    }

    // cas general : recherche des valeurs ad hoc pour T= theta et M= int(mu)
    // initialisation standard
    int M = 1;
    double T = -log(aum) / TM;
    mu = 1.; // M
    theta = T;
    double R = 0.;

    // on mesure l'ecart à la cible au moyen d'une fonction admissible
    double delta = Ecart(double(TC), A[TC], double(TM), A[TM], M, T);
    double refer = delta;
    if (delta == 0)
    {
        // calage parfait qui ne se produira jamais en pratique
        theta = T;
        mu = (double)M;
        return;
    }
    else
    {
        // sinon on cherche le meilleur couple (T,M )possible en testant les valeurs de
        // M comprises entre 1 et max_mu et en cherchant à chaque fois la valeur critique de
        // R=exp(-T) pour laquelle on a : autocorr_average(int(TM),M,R) = autocorr experimentale(TM)
        // on calcule ensuite  la fonction d'écart et si elle est meilleure que la
        // precedente, -log(R) et M deviennent
        // les meilleures estimations courantes de THE et MUL

        for (int M = 1; M < max_mu + 1; ++M)
        {
            double top = 1;
            double bot = 0;
            for (int i = 1; i < dicho + 1; ++i)
            {
                R = (top + bot) / 2;
                if (autocorr_average(TM, M, R) >= A[TM])
                    top = R;
                else
                    bot = R;
            }

            // on a converge vers R tq autocorr_average(TM,M,R) = A[TM]
            delta = Ecart(double(TC), A[TC], double(TM), A[TM], M, -log(R));
            if (delta < refer)
            {
                mu = M;
                theta = -log(R);
                refer = delta;
            }
        }
    }
}

double ATSP::GammaEuler(double z)
{
    // checking for parameter validity
    if (Yuni::Logs::Verbosity::Debug::enabled) // static check
    {
        // in Debug mode, it would be easier to assert the condition
        // (gdb)
        assert((z > 0. && z <= 100.) && "error in the Gamma function (invalid parameter)");
    }
    else
    {
        if (z <= 0. || z > 100.)
        {
            logs.error() << "Internal error in the Gamma function (invalid parameter)";
            return 0.;
        }
    }

    enum
    {
        g = 6,
    };
    double x;
    const double t = z + double(g) + 0.5;
    double rho;
    static const double p[9] = {0.99999999999980993,
                                676.5203681218851,
                                -1259.1392167224028,
                                771.32342877765313,
                                -176.61502916214059,
                                12.507343278686905,
                                -0.13857109526572012,
                                9.9843695780195716e-6,
                                1.5056327351493116e-7};

    if (z < 0.5)
    {
        z = 1. - z;
        rho = PI / sin(PI * z);
        z -= 1.;
        x = p[0];

        for (int i = 1; i < g + 3; ++i)
        {
            x += p[i] / (z + double(i));
        }
        // x = 1. / (sqrt(2. * PI) * pow(t, z + 0.5) * exp(-t) * x);
        x = 1. / (2.506628275 * pow(t, z + 0.5) * exp(-t) * x);
    }
    else
    {
        rho = 1.;
        z -= 1.;
        x = p[0];

        for (int i = 1; i < g + 3; ++i)
        {
            x += p[i] / (z + double(i));
        }
        // x = sqrt(2. * PI) * pow(t, z + 0.5) * exp(-t) * x;
        x = 2.506628275 * pow(t, z + 0.5) * exp(-t) * x;
    }

    return (rho * x);
}

// ajustement de la densité de probabilité en fonction des bornes L, U, de l'espérance E et de
// l'écart-type S selon une loi "type" : calcule alpha=A, beta = B, gamma = C, delta = D retourne 0
// si le calage est possible et 1 sinon */
bool ATSP::Probab_density_funct(double L,
                                double U,
                                double E,
                                double S,
                                Data::XCast::Distribution law,
                                double& A,
                                double& B,
                                double& C,
                                double& D)
{
    enum
    {
        dicho = 20
    };

    A = 1.;
    B = 1.;
    C = 0.;
    D = 1.;

    if (Math::Abs(S) < 1e-40)
    {
        //	double fe = Math::Abs(E);
        //	S = (fe < 1e-40) ? (1e-4) : (1e-4 / fe);
        S = 0;
    }

    switch (law)
    {
    case Data::XCast::dtUniform:
    {
        if (S < 0.)
            return false;
        // au lieu de prendre simplement C=L et D=U, on procede
        // par la methode des moments
        C = (2. * E - 3.4641 * S) / 2.;
        D = (2. * E + 3.4641 * S) / 2.;
        A = L;
        B = U;

        // (note : si le calage est plausible on doit avoir A proche de
        // C et B proche de  D ... )
        return true;
    }

    case Data::XCast::dtBeta:
    {
        if (U <= L)
            return false;
        C = L;
        D = U;
        double V = E - C;
        double W = D - C;
        if (S == W)
            return false;

        A = (V / W) * (V / W * (1. - (V / W)) / ((S * S) / (W * W)) - 1.);
        B = (1. - V / W) * (V / W * (1. - (V / W)) / ((S * S) / (W * W)) - 1.);
        return (A > 0. && B > 0.);
    }

    case Data::XCast::dtNormal:
    {
        if (S < 0.)
            return false;
        C = 0.;
        D = 0.;
        A = E;
        B = S;
        return true;
    }

    case Data::XCast::dtWeibullShapeA:
    {
        if (S < 0.)
            return false;

        // Since 3.5: No translation
        // E = E - L;

        if (E == 0 || S == 0)
            return false;
        // pour une loi de weibull de paramètre a>=1, le calage n'est possible que si
        // S/E < 1
        if (S / E > 1.)
            return false;

        double bot = 1.;
        double top = 50.;

        double cible = S / E;
        cible *= cible;

        // Since 3.5, C = 0
        // C = L;
        C = 0;

        D = 0; // D n'est pas utilise par le generateur stochastique
        double a;
        double g;
        for (uint i = 1; i < dicho + 1; ++i)
        {
            a = (top + bot) / 2.;
            g = GammaEuler(1. + 1. / a);
            g = g * g;
            double niveau = GammaEuler(1. + 2. / a) - g;
            niveau /= g;
            if (niveau >= cible)
                bot = a;
            else
                top = a;
        }
        A = a;
        B = E / GammaEuler(1. + 1. / a);

        // calage mathematiquement possible mais hors des limites antares
        return !(A < 1. || A > 50.);
    }

    case Data::XCast::dtGammaShapeA:
    {
        if (S <= 0.)
            return false;

        // Since 3.5, no translation
        // E = E - L;

        if (E == 0.)
            return false;

        // Since 3.5, C = 0
        // C  = L;
        C = 0;

        D = 0; // D n'est pas utilise par le generateur stochastique
        A = (E / S);
        A *= A;
        // Since 3.5, B' = 1/B, it was B = A / E
        // Now :
        B = E / A;

        return !(A < 1. || A > 50.);
    }

    case Data::XCast::dtNone:
    case Data::XCast::dtMax:
        break;
    }

    return false;
}

double ATSP::Standard_shrinkage(int M, double R)
{
    if (R < 0.) // ceci ne doit jamais se produire
        R = 0.;

    double x = 0.;
    for (int i = 1; i < M + 1; ++i)
    {
        for (int j = i + 1; j < M + 1; ++j)
            x += pow(R, j - i);
    }

    x *= 2;
    x += M;
    x /= (M * M);
    x = sqrt(x);

    return x;
}

} // namespace Antares
