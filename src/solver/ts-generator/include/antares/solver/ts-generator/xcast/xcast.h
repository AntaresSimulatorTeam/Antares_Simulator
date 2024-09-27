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
#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_H__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/study/fwd.h>
#include <antares/study/progression/progression.h>
#include <antares/writer/i_writer.h>
#include "antares/solver/ts-generator/xcast/studydata.h"

using namespace Antares::Solver;

namespace Antares
{
namespace TSGenerator
{
namespace XCast
{
/*!
** \brief XCast Generator for Load, Wind and solar timeseries
**
** \see predicate.hxx for specializations
*/
class XCast final: private Yuni::NonCopyable<XCast>
{
public:
    /*!
    ** \brief Compute
    */
    static double GammaEuler(double z);

    /*!
    ** \brief Calcul de  la forme inférieure de la fonction gamma incomplete
    **
    ** Calcul de  la forme inférieure de la fonction gamma incomplete (s,z) = somme{0;z}
    *t^(s-1)exp(-t)dt
    ** On utilise un developpement en serie jusqu'à l'ordre 30, ce qui garantit
    ** une precision suffisante dans le domaine 1 <= s <= 2 et 0 <= z <= 10
    ** \warning Dans les lois de Weibull, la valeur maximale que z peut atteindre
    **   est Log(1 / NEGLI_WEIB). Il faut donc regler NEGLI_WEIB en conséquence
    **
    */
    static double GammaInc(double s, double z);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    XCast(Data::Study& study, Data::TimeSeriesType ts, IResultWriter& writer);
    /*!
    ** \brief Destructor
    */
    ~XCast() = default;
    //@}

    //! \name Loading
    //@{
    /*!
    ** \brief initialize all XCast indicators from the current simulation parameters
    */
    template<class PredicateT>
    void loadFromStudy(const Data::Correlation& correlation, PredicateT& predicate);
    //@}

    /*!
    ** \brief Run the TS Generator
    */
    bool run();

public:
    //! The current study
    Data::Study& study;

    //! The current year
    uint year;
    //! The time-series type
    const Data::TimeSeriesType timeSeriesType;

    //! The random generator to use
    MersenneTwister* random;

private:
    void allocateTemporaryData();

    template<class PredicateT>
    void updateMissingCoefficients(PredicateT& predicate);
    template<class PredicateT>
    bool runWithPredicate(PredicateT& predicate, Progression::Task& progression);

    /*!
    ** \brief Export all time-series for each process into the output folder
    */
    template<class PredicateT>
    void exportTimeSeriesToTheOutput(Progression::Task& progression, PredicateT& predicate);

    /*!
    ** \brief Perform the generation of the time-series on a single day for all processes
    **
    ** Ce code effectue dans l'ordre :
    ** a) elaboration de series de valeurs horaires X(a) de processus "purs"
    **    definis par A,B,G,D,T,L,C
    ** b) transformation de ces series en X(b) par un lissage en moyenne glissante
    **    defini par M (permet de regler l'autocorrelation a court terme)
    ** c) Modulation des series en X(c) en fonction de coefficients FO de
    **    variations quotidienne des valeurs horaires
    ** d) Ecretement (haut, bas) de la serie en X(d) de façon a la contenir dans
    **    des bornes imposees MI,MA
    **
    ** ATTENTION : les 4 étapes sont enchainees pour chaque point horaire.
    ** On ne passe donc pas de t à t+1 en faisant
    **   X(a)(t) -> X(a)(t+1)
    **   X(b)(t) -> X(b)(t+1)
    **
    ** Le processus X(a) peut donc s'ecarter de sa loi marginale et de son
    ** autocorrelation theoriques si les modulations FO et les bornes MI,MA
    ** conduisent frequemment a rectifier sa trajectoire.
    ** Ce cas peut se presenter si les bornes d'ecretement MI,MA sont egales a
    ** G,D (voire plus etroites) et si les modulations FO sont marquees.
    **
    **
    ** LOIS MODELISEES
    ** regle generale pour tous les types de loi : t>=0
    ** regles particulieres selon les types de lois :
    **
    ** \code
    ** code loi		a				b			g
    *d autres contraintes Loi marginale
    ** 1		quelconque     quelconque  g> INFININ/2   d<INFINIP/2			g<=d
    *Uniforme sur l'intervalle	(g,d)
    ** 2			a>0				b>0    g> INFININ/2   d<INFINIP/2
    *g<=d Bêta (a,b)sur l'intervalle (g,d)
    ** 3	    quelconque          b>=0   g=  a-6*b	  d= a+6*b Normale d'espérance a et
    *d'écart-type b
    ** 4			a>=1		    b>0	   g> INFININ/2   d= gamma_euler(1+1/a)
    *Weibull de forme a, d'échelle b et définie pour x>g
    ** 5			a>=1			b>0	   g> INFININ/2   d non défini
    *Gamma de forme a, d'échelle b et définie pour x>g
    ** \endcode
    */
    bool generateValuesForTheCurrentDay();

    template<class PredicateT>
    void applyTransferFunction(PredicateT& predicate);

    //! Tirage de deux variables normales centrees et reduites
    void normal(float& x, float& y);

private:
    //! The number of time-series
    uint nbTimeseries_;

    //! Some data after transformation
    StudyData pData;

    //! The correlation matrix for the current month
    const Matrix<float>* pCorrMonth;

    bool pNeverInitialized = true;
    uint Nombre_points_intermediaire;

    //! True when starting a new month (some data may have to be reinitialized)
    bool pNewMonth;

    // Statistics
    //! The number of computed points a day
    uint pComputedPointCount;
    uint pNDPMatrixCount;
    uint pLevellingCount;

    bool pAccuracyOnCorrelation = false;
    bool All_normal; // all processes are Normal

    // les variables de A à CO sont des vues de ALPH à CORR pour un mois particulier
    std::vector<float> A;
    std::vector<float> B;
    std::vector<float> G;
    std::vector<float> D;
    std::vector<int> M;
    std::vector<float> T;
    std::vector<Data::XCast::Distribution> L;
    std::vector<bool> BO;
    std::vector<float> MA;
    std::vector<float> MI;
    std::vector<std::vector<float>> FO; // contrainte : FO >=0

    float STEP;
    float SQST;
    std::vector<float> POSI;
    std::vector<std::vector<float>> CORR;
    std::vector<float> MAXI;
    std::vector<float> MINI;
    std::vector<float> Presque_maxi;
    std::vector<float> Presque_mini;
    std::vector<float> ESPE;
    std::vector<float> STDE;
    std::vector<std::vector<float>> LISS;
    std::vector<std::vector<float>> DATL;

    std::vector<float> DIFF;
    std::vector<float> TREN;
    std::vector<float> WIEN;
    std::vector<float> BROW;

    std::vector<float> BASI; // used only if all processes are Normal
    std::vector<float> ALPH; // used only if all processes are Normal
    std::vector<float> BETA; // used only if all processes are Normal

    std::vector<std::vector<float>> Triangle_reference;
    std::vector<std::vector<float>> Triangle_courant;
    std::vector<std::vector<float>> Carre_reference;
    std::vector<std::vector<float>> Carre_courant;

    std::vector<float> D_COPIE;

    std::vector<std::vector<float>> DATA;

    // cholesky temporary data
    std::vector<float> pQCHOLTotal;

    //!
    std::vector<bool> pUseConversion;

    //! Name of the current timeseries
    Yuni::CString<32, false> pTSName;

    IResultWriter& pWriter;
}; // class XCast

} // namespace XCast
} // namespace TSGenerator
} // namespace Antares

#include "xcast.hxx"

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_H__
