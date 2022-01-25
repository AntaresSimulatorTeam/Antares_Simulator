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
#ifndef __PREPROCESSOR_ATSP_H__
#define __PREPROCESSOR_ATSP_H__

#include <yuni/yuni.h>
#include <memory>
#include <yuni/core/string.h>
#include <antares/study/xcast/xcast.h>
#include <antares/study.h>

namespace Antares
{
class ATSP final
{
public:
    //! The most suitable smart pointer for the class
    using Ptr = std::shared_ptr<ATSP>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ATSP();
    //! Destructor
    ~ATSP();
    //@}

    /*!
    ** \brief Load settings from an INI file
    */
    bool loadFromINIFile(const Yuni::String& filename);

    /*!
    ** \brief Print a summary
    */
    void printSummary() const;

    bool preflight();

    bool computeMonthlyCorrelations();

private:
    class AreaInfo final
    {
    public:
        //! Vector
        typedef std::vector<AreaInfo*> Vector;

    public:
        bool enabled;
        Data::AreaName name;
        Yuni::String filename;
        bool rawData;
        Data::XCast::Distribution distribution;
    };

    struct MomentCentrSingle
    {
        double data[12][4];
    };
    typedef std::vector<MomentCentrSingle> MomentCentr;

    struct HiddenHoursSingle
    {
        int data[12][24];
    };
    typedef std::vector<HiddenHoursSingle> HiddenHours;

private:
    // range dans les nblig première lignes et nbcol premières colonnes de OUT
    // le bloc d'élements de MTRX de taille nblig x nbcol commençant aux indices indlig et indcol
    static void Extrait_bloc(Matrix<>& out,
                             uint indlig,
                             uint indcol,
                             const Matrix<>& MTRX,
                             uint nblig,
                             uint nbcol);

    // range dans OUT la moyenne des colonnes de MTRX
    static void Colonne_moyenne(double* out, const Matrix<>& MTRX, uint nblig, uint nbcol);

    // retourne le maximum ou le minimum de MTRX selon le code
    static void Mtrx_bound(double& rmin,
                           double& rmax,
                           const Matrix<>& MTRX,
                           uint nblig,
                           uint nbcol);

    // met dans OUT la valeur absolue de MTRX
    static void Mtrx_abs(Matrix<>& out, const Matrix<>& MTRX, uint nblig, uint nbcol);

    // retourne la moyenne des elements de A
    static double Moyenne_generale(double* A, uint nblig);

    // retranche de MTRX le vecteur A
    static void Retranche_mtrx(Matrix<>& MTRX, const double* A, uint nblig, uint nbcol);

    // range dans OUT les nblig première lignes de la colonne de MTRX d'indice indcol
    static void Extrait_col(double* out, const Matrix<>& MTRX, uint nblig, uint indcol);

    // met dans A le carre de B
    static void Square(double* A, const double* B, uint nblig);

    // met (B- le scalaire x) dans A
    static void Retranche_scalaire(Matrix<>& A,
                                   const Matrix<>& B,
                                   double x,
                                   uint nblig,
                                   uint nbcol);

    // met (B /scalaire x) dans A
    static void Divise_scalaire(Matrix<>& A, const Matrix<>& B, double x, uint nblig, uint nbcol);

    // met dans A le cube de B
    static void Cube(double* A, const double* B, uint nblig);

    // met dans A B^4
    static void Dsquare(double* A, const double* B, uint nblig);

    // range dans OUT  nblig elements de MTRX pris a partir de l'indice indlig
    static void Extrait_seg(double* out, const double* src, uint nblig, uint indlig);

    // retourne le coefficient de corrélation entre A et B  (retourne 999 si paramètre "code"
    // aberrant)
    //   si code = 0 : les espérances et écarts-types des variables représentées par  A et B sont à
    //   calculer si code = 1 : des estimations des espérances et écarts-types sont fournis dans
    //   EA,EB,SA,SB
    static double Correlation(double* A,
                              double* B,
                              uint nblig,
                              double EA,
                              double EB,
                              double SA,
                              double SB,
                              int code);

    static double GammaEuler(double z);

    static double Ecart(double T1, double P1, double T2, double P2, int M, double T);

    static double autocorr_average(int H, int M, double R);

    static double Standard_shrinkage(int M, double R);

private:
    //! Check the study version
    bool checkStudyVersion() const;

    bool preflight(const uint areaIndex);

    void Analyse_auto(double* A,
                      int nblig,
                      double auc,
                      double aum,
                      double hor,
                      double& theta,
                      double& mu);

    // ajustement de la densité de probabilité en fonction des bornes L, U, de l'espérance E
    // et de l'écart-type S
    // selon une loi "type" : calcule alpha=A, beta = B, gamma = C, delta = D
    // retourne 0 si le calage est possible et 1 sinon
    bool Probab_density_funct(double L,
                              double U,
                              double E,
                              double S,
                              Data::XCast::Distribution law,
                              double& A,
                              double& B,
                              double& C,
                              double& D);

    void roundMatrixValues(Matrix<>& m);

    bool writeMoments() const;

    void cacheCreate();
    void cacheDestroy();
    void cacheClear();
    bool cacheFetch(uint index, Matrix<>& out) const;

    bool cachePreload(uint index,
                      const AnyString& filename,
                      uint height,
                      Matrix<>::BufferType& buffer);

private:
    AreaInfo::Vector pArea;
    YString pStudyFolder;
    YString pTemp;
    uint pTimeseriesCount;
    uint pMHeight;
    uint pTimeseries;
    double pMediumTermAutoCorrAdjustment;
    double pShortTermAutoCorrAdjustment;
    double pRoundOff;
    double pUpperBound;
    double pUpperBound80percent;
    double pLowerBound;
    bool pUseUpperBound;
    bool pUseLowerBound;
    yuint64 pRoundingCount;
    yuint64 pRounding80percentCount;
    yuint64 pRoundingCountTotal;
    Yuni::ShortString16 tsName;

    uint pEnabledAreaCount;

    //! The total number of zareas
    uint NBZ;
    //! The total number of timeseries to analyze
    uint NBS;

    //! The target timeseries (1: wind, 2: solar, 3: load)
    char TDS;
    //! The target timeseries (W: wind, S: solar, L: load)
    char code;

    double RTZ;
    //! Short-term auto-correlation adjustment
    double AUC;
    //! Medium-term auto-correlation adjustment
    double AUM;

    /*!
    ** \brief Horizon degenere
    **
    ** Autocorrelation minimale requise entre X(0) et X(0+mu), ou mu est la periode des
    ** moyennes glissantes de X(t)
    **
    ** valeur >= 0.92 pour eviter la degenerescence de X en loi normale
    ** Ce seuil n'est pas utilise quand l'analyse porte sur une loi normale
    */
    const double HOR; // 0.92 by default
    enum
    {
        //! nombre de valeurs d'autocorrelation calculees (de X0-X0 à X0-X119)
        PRA = 120,
    };

    MomentCentr moments_centr_net;
    MomentCentr moments_centr_raw;

    HiddenHours hidden_hours;

    Matrix<> SERIE_N;
    Matrix<> SERIE_P;
    Matrix<> SERIE_Q;

    double buffer_n[744];
    double buffer_p[744];
    double buffer_q[744];

    static const uint lonmois[12];
    static const uint durmois[12];
    static const uint posmois[12];
    enum
    {
        durjour = 24
    };

    yuint64 pLimitMemory;
    yuint64 pCacheMemoryUsed;
    uint pCacheLastValidIndex;
    Matrix<>* pCacheMatrix;

    Yuni::String::Vector folderPerArea;
    //! Temporary string mainly used for filename manipulation
    Yuni::CString<512> pStr;

    bool pAutoClean;

}; // class ATSP

} // namespace Antares

#include "atsp.hxx"

#endif // __PREPROCESSOR_ATSP_H__
