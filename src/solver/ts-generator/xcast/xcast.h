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
#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_H__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_H__

#include <yuni/yuni.h>
#include <antares/study/fwd.h>
#include <antares/mersenne-twister/mersenne-twister.h>
#include <i_writer.h>
#include "studydata.h"
#include <yuni/core/noncopyable.h>

namespace Antares
{
namespace Solver
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
class XCast final : private Yuni::NonCopyable<XCast>
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
    XCast(Data::Study& study, Data::TimeSeries ts, IResultWriter::Ptr writer);
    /*!
    ** \brief Destructor
    */
    ~XCast();
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

    //! The number of time-series
    uint nbTimeseries;
    //! The current year
    uint year;
    //! The time-series type
    const Data::TimeSeries timeSeriesType;

    //! The random generator to use
    MersenneTwister* random;

private:
    void allocateTemporaryData();
    void destroyTemporaryData();

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
    //! Some data after transformation
    StudyData pData;

    //! The correlation matrix for the current month
    const Matrix<float>* pCorrMonth;

    bool pNeverInitialized;
    uint Nombre_points_intermediaire;

    //! True when starting a new month (some data may have to be reinitialized)
    bool pNewMonth;

    // Statistics
    //! The number of computed points a day
    uint pComputedPointCount;
    uint pNDPMatrixCount;
    uint pLevellingCount;

    bool pAccuracyOnCorrelation;
    bool All_normal; // all processes are Normal

    float* A; // les variables de A à CO sont des vues de ALPH à CORR pour un mois particulier
    float* B;
    float* G;
    float* D;
    int* M;
    float* T;
    Data::XCast::Distribution* L;
    bool* BO;
    float* MA;
    float* MI;
    float** FO; // contrainte : FO >=0

    float STEP;
    float SQST;
    float* POSI;
    float** CORR;
    float* MAXI;
    float* MINI;
    float* Presque_maxi;
    float* Presque_mini;
    float* ESPE;
    float* STDE;
    float** LISS;
    float** DATL;

    float* DIFF;
    float* TREN;
    float* WIEN;
    float* BROW;

    float* BASI; // used only if all processes are Normal
    float* ALPH; // used only if all processes are Normal
    float* BETA; // used only if all processes are Normal

    float** Triangle_reference;
    float** Triangle_courant;
    float** Carre_reference;
    float** Carre_courant;

    float* D_COPIE;

    float** DATA;

    // cholesky temporary data
    float* pQCHOLTotal;

    //!
    bool* pUseConversion;

    //! Name of the current timeseries
    Yuni::CString<32, false> pTSName;

    IResultWriter::Ptr pWriter;
}; // class XCast

} // namespace XCast
} // namespace TSGenerator
} // namespace Solver
} // namespace Antares

#include "xcast.hxx"

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_XCAST_H__
