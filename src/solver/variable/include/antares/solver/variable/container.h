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
#ifndef __SOLVER_VARIABLE_LIST_H__
#define __SOLVER_VARIABLE_LIST_H__

#include <yuni/yuni.h>
#include <yuni/core/static/types.h>
#include <yuni/core/string.h>

#include <antares/logs/logs.h>

#include "categories.h"
#include "endoflist.h"
#include "info.h"
#include "surveyresults.h"

namespace Antares::Solver::Variable::Container
{
/*!
** \brief Static list for all output variables
**
** This structure is merely a static linked list with all variables
*/
template<class NextT = Container::EndOfList>
class List: public NextT
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
                             IResultWriter& writer);

    /*!
    ** \brief Ask to all variables to fullfil the digest
    */
    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;
    //@}

private:
    //! Pointer to the current study
    Data::Study* pStudy;

}; // class List

} // namespace Antares::Solver::Variable::Container

#include "container.hxx"
#include "surveyresults/reportbuilder.hxx"

#endif // __SOLVER_VARIABLE_LIST_H__
