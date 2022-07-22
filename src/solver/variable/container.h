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
#ifndef __SOLVER_VARIABLE_LIST_H__
#define __SOLVER_VARIABLE_LIST_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/static/types.h>

#include <antares/logs.h>

#include "endoflist.h"
#include "categories.h"
#include "surveyresults.h"
#include "info.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Container
{
/*!
** \brief Static list for all output variables
**
** This structure is merely a static linked list with all variables
*/
template<class NextT = Container::EndOfList>
class List : public NextT
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! The full type of the class
    typedef List<NextT> ListType;

    enum
    {
        //! How many items have we got
        count = NextT::count,
    };

public:
    /*!
    ** \brief Try to estimate the memory footprint that the solver will require to make a simulation
    */
    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    List();
    /*!
    ** \brief Destructor
    */
    ~List();
    //@}

    //! \name Variable initialization
    //@{
    /*!
    ** \brief Initialize all output variables
    */
    void initializeFromStudy(Data::Study& study);

    /*!
    ** \brief Initialize all output variables according a given area
    */
    void initializeFromArea(Data::Study* study, Data::Area* area);

    /*!
    ** \brief Initialize all output variables according a given link
    */
    void initializeFromLink(Data::Study* study, Data::AreaLink* link);

    /*!
    ** \brief Initialize all output variables according a given thermal cluster
    */
    void initializeFromThermalCluster(Data::Study* study,
                                      Data::Area* area,
                                      Data::ThermalCluster* cluster);
    //@}

    //! \name Simulation events
    //@{
    /*!
    ** \brief Notify to all variables that the simulation is about to begin
    */
    void simulationBegin();

    /*!
    ** \brief Notify to all variables that the simulation has finished
    */
    void simulationEnd();
    //@}

    //! \name Years events
    //@{
    /*!
    ** \brief Notify to all variables that a new year is about to start
    **
    ** \param year The current year
    */
    void yearBegin(unsigned int year, unsigned int numSpace);

    /*!
    ** \brief Notify to all variables that the year is now over
    **
    ** That mainly means that all variables should perform the monthly
    ** aggragations.
    ** \param year The current year
    */
    void yearEnd(unsigned int year, unsigned int numSpace);

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary);

    template<class V>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, unsigned int numSpace);

    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, const SetT& set);

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars,
                                         std::map<unsigned int, unsigned int>& numSpaceToYear,
                                         unsigned int);

    template<class V>
    void simulationEndSpatialAggregates(V& allVars);

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V& allVars, const SetT& set);
    //@}

    //! \name Hourly events
    //@{
    /*!
    ** \brief Notify to all variables that a new hour is about to begin
    */
    void hourBegin(unsigned int hourInTheYear);

    void hourForEachArea(State& state, unsigned int numSpace);

    void hourForEachThermalCluster(State& state);

    void hourForEachLink(State& state);

    void hourEnd(State& state, unsigned int hourInTheYear);
    //@}

    //! \name Weekly events
    //@{
    void weekBegin(State& state);

    void weekEnd(State& state);
    //@}

    //! \name Spatial aggregation
    //@{
    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out);

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area, unsigned int numSpace);

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area);

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(
      typename Variable::Storage<VCardToFindT>::ResultsType** result,
      const Data::ThermalCluster* cluster);

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link);
    //@}

    //! \name User reports
    //@{
    /*!
    ** \brief Ask to all variables to fullfil the report
    */
    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 unsigned int numSpace) const;

    /*!
    ** \brief Ask to all variables to fullfil additional reports (like the digest for example)
    **
    ** \tparam GlobalT True to write down the results of the simulation, false
    **   for the results of the current year
    */
    void exportSurveyResults(bool global,
                             const Yuni::String& output,
                             unsigned int numSpace,
                             Antares::Solver::ZipWriter& writer);

    /*!
    ** \brief Ask to all variables to fullfil the digest
    */
    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;
    //@}

    //! \name Memory management
    //@{
    //! Get the amount of memory currently used by the class
    Yuni::uint64 memoryUsage() const;
    //@}

private:
    //! Pointer to the current study
    Data::Study* pStudy;

}; // class List

} // namespace Container
} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "surveyresults/reportbuilder.hxx"
#include "container.hxx"

#endif // __SOLVER_VARIABLE_LIST_H__
