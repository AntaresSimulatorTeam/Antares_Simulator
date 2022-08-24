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
#ifndef __SOLVER_VARIABLE_ECONOMY_Join_H__
#define __SOLVER_VARIABLE_ECONOMY_Join_H__

#include "../variable.h"
#include "../../simulation/sim_extern_variables_globales.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
struct VCardJoin
{
    //! Caption
    static const char* Caption()
    {
        return "";
    }
    //! Unit
    static const char* Unit()
    {
        return "";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::id | Category::va),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = 0,
        //! The Spatial aggregation
        spatialAggregate = Category::noSpatialAggregate,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 0,

    };

}; // class VCard

/*!
** \brief Join
*/
template<class LeftT, class RightT, class BindConstT>
class Join : public Variable::IVariable<Join<LeftT, RightT, BindConstT>, Yuni::Default, VCardJoin>,
             public LeftT,
             public RightT,
             public BindConstT
{
public:
    typedef LeftT LeftType;
    typedef RightT RightType;
    typedef BindConstT BindConstType;

    //! VCard
    typedef VCardJoin VCardType;
    //! Ancestor
    typedef Variable::IVariable<Join<LeftT, RightT, BindConstT>, Yuni::Default, VCardType>
      AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
        //! How many items have we got
        count = 1 + RightT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = LeftType::template Statistics<CDataLevel, CFile>::count
                    + RightType::template Statistics<CDataLevel, CFile>::count 
                    + BindConstType::template Statistics<CDataLevel, CFile>::count,
        };
    };

public:
    /*!
    ** \brief Try to estimate the memory footprint that the solver will require to make a simulation
    */
    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u)
    {
        LeftType::EstimateMemoryUsage(u);
        RightType::EstimateMemoryUsage(u);
        BindConstType::EstimateMemoryUsage(u);
    }

    /*!
    ** \brief Retrieve the list of all individual variables
    **
    ** The predicate must implement the method `add(name, unit, comment)`.
    */
    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate)
    {
        LeftType::RetrieveVariableList(predicate);
        RightType::RetrieveVariableList(predicate);
        BindConstType::RetrieveVariableList(predicate);
    }

public:
    void initializeFromStudy(Data::Study& study)
    {
        LeftType::initializeFromStudy(study);
        RightType::initializeFromStudy(study);
        BindConstType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        LeftType::initializeFromArea(study, area);
        RightType::initializeFromArea(study, area);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        LeftType::initializeFromAreaLink(study, link);
        RightType::initializeFromAreaLink(study, link);
    }

    void initializeFromThermalCluster(Data::Study* study,
                                      Data::Area* area,
                                      Data::ThermalCluster* cluster)
    {
        LeftType::initializeFromThermalCluster(study, area, cluster);
        RightType::initializeFromThermalCluster(study, area, cluster);
    }

    void simulationBegin()
    {
        LeftType::simulationBegin();
        RightType::simulationBegin();
    }

    void simulationEnd()
    {
        LeftType::simulationEnd();
        RightType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        LeftType::yearBegin(year, numSpace);
        RightType::yearBegin(year, numSpace);
        BindConstType::yearBegin(year, numSpace);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year)
    {
        // Next variable
        LeftType::yearEndBuildPrepareDataForEachThermalCluster(state, year);
        RightType::yearEndBuildPrepareDataForEachThermalCluster(state, year);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year)
    {
        // Next variable
        LeftType::yearEndBuildForEachThermalCluster(state, year);
        RightType::yearEndBuildForEachThermalCluster(state, year);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        LeftType::yearEndBuild(state, year, numSpace);
        RightType::yearEndBuild(state, year);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        LeftType::yearEnd(year, numSpace);
        RightType::yearEnd(year, numSpace);
        BindConstType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        LeftType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
        RightType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
        BindConstType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void weekBegin(State& state)
    {
        LeftType::weekBegin(state);
        RightType::weekBegin(state);
        BindConstType::weekBegin(state);
    }

    void weekForEachArea(State& state, unsigned int numSpace)
    {
        LeftType::weekForEachArea(state, numSpace);
        RightType::weekForEachArea(state);
    }

    void weekEnd(State& state)
    {
        LeftType::weekEnd(state);
        RightType::weekEnd(state);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        LeftType::hourBegin(hourInTheYear);
        RightType::hourBegin(hourInTheYear);
        BindConstType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        LeftType::hourForEachArea(state, numSpace);
        RightType::hourForEachArea(state);
    }

    void hourForEachThermalCluster(State& state)
    {
        LeftType::hourForEachThermalCluster(state);
        RightType::hourForEachThermalCluster(state);
    }

    void hourForEachLink(State& state)
    {
        LeftType::hourForEachLink(state);
        RightType::hourForEachLink(state);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        LeftType::hourEnd(state, hourInTheYear);
        RightType::hourEnd(state, hourInTheYear);
        BindConstType::hourEnd(state, hourInTheYear);
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        LeftType::buildSurveyReport(results, dataLevel, fileLevel, precision);
        RightType::buildSurveyReport(results, dataLevel, fileLevel, precision);
        BindConstType::buildSurveyReport(results, dataLevel, fileLevel, precision);
    }

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 uint numSpace) const
    {
        LeftType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
        RightType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
        BindConstType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Building the digest
        LeftType ::buildDigest(results, digestLevel, dataLevel);
        RightType::buildDigest(results, digestLevel, dataLevel);
    }

    void beforeYearByYearExport(uint year, uint numSpace)
    {
        // Building the digest
        LeftType ::beforeYearByYearExport(year, numSpace);
        RightType::beforeYearByYearExport(year, numSpace);
    }

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out)
    {
        LeftType ::template computeSpatialAggregateWith<SearchVCardT, O>(out);
        RightType::template computeSpatialAggregateWith<SearchVCardT, O>(out);
    }

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace)
    {
        LeftType ::template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace);
        RightType::template computeSpatialAggregateWith<SearchVCardT, O>(out, area);
    }

    template<class V>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, unsigned int numSpace)
    {
        LeftType ::template yearEndSpatialAggregates(allVars, year, numSpace);
        RightType::template yearEndSpatialAggregates(allVars, year, numSpace);
    }

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars,
                                         std::map<unsigned int, unsigned int>& numSpaceToYear,
                                         unsigned int nbYearsForCurrentSummary)
    {
        LeftType ::template computeSpatialAggregatesSummary(
          allVars, numSpaceToYear, nbYearsForCurrentSummary);
        RightType::template computeSpatialAggregatesSummary(
          allVars, numSpaceToYear, nbYearsForCurrentSummary);
    }

    template<class V>
    void simulationEndSpatialAggregates(V& allVars)
    {
        LeftType ::template simulationEndSpatialAggregates(allVars);
        RightType::template simulationEndSpatialAggregates(allVars);
    }

    Yuni::uint64 memoryUsage() const
    {
        return LeftType::memoryUsage() + RightType::memoryUsage() + BindConstType::memoryUsage();
    }

    template<class I>
    static void provideInformations(I& infos)
    {
        LeftType ::provideInformations(infos);
        RightType::provideInformations(infos);
        BindConstType::provideInformations(infos);  // gp : do we need this ?
    }

    template<class VCardToFindT>
    const double* retrieveHourlyResultsForCurrentYear() const
    {
        // Is this function ever called ?
        auto* result = LeftType::template retrieveHourlyResultsForCurrentYear<VCardToFindT>();
        return (!result) ? RightType::template retrieveHourlyResultsForCurrentYear<VCardToFindT>()
                         : result;
    }

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area)
    {
        LeftType::template retrieveResultsForArea<VCardToFindT>(result, area);
        RightType::template retrieveResultsForArea<VCardToFindT>(result, area);
    }

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(
      typename Variable::Storage<VCardToFindT>::ResultsType** result,
      const Data::ThermalCluster* cluster)
    {
        LeftType::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
        RightType::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
    }

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link)
    {
        LeftType::template retrieveResultsForLink<VCardToFindT>(result, link);
        RightType::template retrieveResultsForLink<VCardToFindT>(result, link);
    }

    void localBuildAnnualSurveyReport(SurveyResults& results, int fileLevel, int precision) const
    {
        LeftType::localBuildAnnualSurveyReport(results, fileLevel, precision);
        RightType::localBuildAnnualSurveyReport(results, fileLevel, precision);
        BindConstType::localBuildAnnualSurveyReport(results, fileLevel, precision);
    }

}; // class Join

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_Join_H__
