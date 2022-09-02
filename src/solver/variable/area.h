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
#ifndef __SOLVER_VARIABLE_AREA_H__
#define __SOLVER_VARIABLE_AREA_H__

#include "state.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
struct VCardAllAreas
{
    //! Caption
    static const char* Caption()
    {
        return "Areas";
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
    typedef Results<> ResultsType;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & Category::de,
        //! Indentation (GUI)
        nodeDepthForGUI = +1,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = 0,
        //! The Spatial aggregation
        spatialAggregate = Category::noSpatialAggregate,
        //! Intermediate values
        hasIntermediateValues = 0,
    };

}; // class VCard

template<class NextT = Container::EndOfList>
class Areas //: public Variable::IVariable<Areas<NextT>, NextT, VCardAllAreas>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardAllAreas VCardType;
    //! Ancestor
    // typedef Variable::IVariable<Areas<NextT>, NextT, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    enum
    {
        //! How many items have we got
        count = NextT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = NextType::template Statistics<CDataLevel, CFile>::count
        };
    };

public:
    /*!
    ** \brief Try to estimate the memory footprint that the solver will require to make a simulation
    */
    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u);

    /*!
    ** \brief Retrieve the list of all individual variables
    **
    ** The predicate must implement the method `add(name, unit, comment)`.
    */
    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Areas();
    //! Destructor
    ~Areas();
    //@}

    void initializeFromStudy(Data::Study& study);
    void initializeFromArea(Data::Study*, Data::Area*);
    void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*);
    void initializeFromAreaLink(Data::Study*, Data::AreaLink*);

    void simulationBegin();
    void simulationEnd();

    void yearBegin(uint year, uint numSpace);
    //	void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year);
    //	void yearEndBuildForEachThermalCluster(State& state, uint year);

    void yearEndBuild(State& state, uint year, uint numSpace);

    void yearEnd(uint year, uint numSpace);

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary);

    void hourBegin(uint hourInTheYear);

    void hourForEachArea(State& state, uint numSpace);
    void hourForEachLink(State& state, uint numSpace);
    void hourForEachThermalCluster(State& state, uint numSpace);

    void hourEnd(State& state, uint hourInTheYear);

    void weekBegin(State& state);
    void weekForEachArea(State& state, uint numSpace);
    void weekEnd(State& state);

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 uint numSpace) const;

    void beforeYearByYearExport(uint year, uint numSpace);

    Yuni::uint64 memoryUsage() const;

    template<class I>
    static void provideInformations(I& infos);

    template<class V>
    void yearEndSpatialAggregates(V&, uint, uint)
    {
        // do nothing
    }

    template<class V>
    void computeSpatialAggregatesSummary(V&, std::map<unsigned int, unsigned int>&, unsigned int)
    {
        // do nothing
    }

    template<class V>
    void simulationEndSpatialAggregates(V&)
    {
        // do nothing
    }

    template<class VCardSearchT, class O>
    void computeSpatialAggregateWith(O& out);

    template<class VCardSearchT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace);

    template<class VCardToFindT>
    const double* retrieveHourlyResultsForCurrentYear() const;

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area);

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType** result,
                                          const Data::ThermalCluster* cluster);

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link);

private:
    //! Area list
    NextType* pAreas;
    //! The number of areas
    uint pAreaCount;

}; // class Areas

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "area.hxx"

#endif // __SOLVER_VARIABLE_AREA_H__
