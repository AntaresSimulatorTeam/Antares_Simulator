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
#ifndef __SOLVER_VARIABLE_ECONOMY_Join_H__
#define __SOLVER_VARIABLE_ECONOMY_Join_H__

#include "antares/solver/variable/variable.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
struct VCardJoin
{
    //! Caption
    static std::string Caption()
    {
        return "";
    }

    //! Unit
    static std::string Unit()
    {
        return "";
    }

    //! The short description of the variable
    static std::string Description()
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

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 0;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 0;

}; // class VCard

/*!
** \brief Join
*/
template<class LeftT, class RightT>
class Join: public Variable::IVariable<Join<LeftT, RightT>, Yuni::Default, VCardJoin>,
            public LeftT,
            public RightT
{
public:
    typedef LeftT LeftType;
    typedef RightT RightType;

    //! VCard
    typedef VCardJoin VCardType;
    //! Ancestor
    typedef Variable::IVariable<Join<LeftT, RightT>, Yuni::Default, VCardType> AncestorType;

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
            count = (int)LeftType::template Statistics < CDataLevel,
            CFile > ::count + (int)RightType::template Statistics < CDataLevel,
            CFile > ::count,
        };
    };

public:
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
    }

public:
    void initializeFromStudy(Data::Study& study)
    {
        LeftType::initializeFromStudy(study);
        RightType::initializeFromStudy(study);
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
        RightType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        LeftType::yearEnd(year, numSpace);
        RightType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        LeftType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
        RightType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void weekBegin(State& state)
    {
        LeftType::weekBegin(state);
        RightType::weekBegin(state);
    }

    void weekForEachArea(State& state, unsigned int numSpace)
    {
        LeftType::weekForEachArea(state, numSpace);
        RightType::weekForEachArea(state, numSpace);
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
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        LeftType::hourForEachArea(state, numSpace);
        RightType::hourForEachArea(state, numSpace);
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
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        LeftType::buildSurveyReport(results, dataLevel, fileLevel, precision);
        RightType::buildSurveyReport(results, dataLevel, fileLevel, precision);
    }

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 uint numSpace) const
    {
        LeftType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
        RightType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
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
        RightType::template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace);
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
        LeftType ::template computeSpatialAggregatesSummary(allVars,
                                                            numSpaceToYear,
                                                            nbYearsForCurrentSummary);
        RightType::template computeSpatialAggregatesSummary(allVars,
                                                            numSpaceToYear,
                                                            nbYearsForCurrentSummary);
    }

    template<class V>
    void simulationEndSpatialAggregates(V& allVars)
    {
        LeftType ::template simulationEndSpatialAggregates(allVars);
        RightType::template simulationEndSpatialAggregates(allVars);
    }

    uint64_t memoryUsage() const
    {
        return LeftType::memoryUsage() + RightType::memoryUsage();
    }

    template<class I>
    static void provideInformations(I& infos)
    {
        LeftType ::provideInformations(infos);
        RightType::provideInformations(infos);
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
    }

}; // class Join

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_Join_H__
