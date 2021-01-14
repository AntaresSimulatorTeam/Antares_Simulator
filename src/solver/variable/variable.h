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
#ifndef __SOLVER_VARIABLE_VARIABLE_H__
#define __SOLVER_VARIABLE_VARIABLE_H__

// Remove the visual studio warning: decorated name length exceeded, name was truncated
#ifdef YUNI_OS_MSVC
#pragma warning(disable : 4503)
#endif

#include <yuni/yuni.h>
#include <yuni/core/static/if.h>
#include "container.h"
#include "surveyresults.h"
#include "storage/intermediate.h"
#include "storage/results.h"
#include "endoflist.h"
#include "constants.h"
#include "categories.h"
#include "info.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
/*!
** \brief Interface for any variable
*/
template<class ChildT, class NextT, class VCardT>
class IVariable : protected NextT
{
public:
    //! Child
    typedef ChildT ChildType;
    //! Type of the next static variable
    typedef NextT NextType;
    //! Variable
    typedef IVariable<ChildT, NextT, VCardT> VariableType;
    //! VCard
    typedef VCardT VCardType;
    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;
    //! Proxy for accessing to the intermediate results
    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;
    //! Results
    typedef typename Storage<VCardT>::ResultsType StoredResultType;

    // Default values
    enum
    {
        categoryDataLevel = VCardType::categoryDataLevel,
        categoryFileLevel = VCardType::categoryFileLevel,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count
            = ((categoryDataLevel & CDataLevel && categoryFileLevel & CFile)
                 ? (NextType::template Statistics<CDataLevel, CFile>::count + ResultsType::count)
                 : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    /*!
    ** \brief Try to estimate the memory footprint that the solver will require to make a simulation
    */
    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u);

    /*!
    ** \brief Initialize the results of the variable
    **
    ** \param results The results
    ** \param study The attached study
    */
    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study);

    /*!
    ** \brief Retrieve the list of all individual variables
    **
    ** The predicate must implement the method `add(name, unit, comment)`.
    */
    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate);

public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    IVariable();
    //@}

    ~IVariable();

    //! \name Initialize the variable
    //@{
    /*!
    ** \brief Initialize the variable
    **
    ** \param study The attached study
    */
    void initializeFromStudy(Data::Study& study);

    /*!
    ** \brief Initialize the variable with a specific area
    **
    ** \param study The attached study
    ** \param area The area for this variable
    */
    void initializeFromArea(Data::Study* study, Data::Area* area);

    /*!
    ** \brief Initialize the variable with a specific link between two areas
    **
    ** \param study The attached study
    ** \param link The link
    */
    void initializeFromLink(Data::Study* study, Data::AreaLink* link);

    /*!
    ** \brief Initialize the variable with a specific thermal cluster
    **
    ** \param study The attached study
    ** \param area The area
    ** \param cluster The thermal cluster
    */
    void initializeFromThermalCluster(Data::Study* study,
                                      Data::Area* area,
                                      Data::ThermalCluster* cluster);
    //@}

    void broadcastNonApplicability(bool applyNonApplicable);

    void getPrintStatusFromStudy(Data::Study& study);

    //! \name Simulation
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

    //! \name Year
    //@{
    /*!
    ** \brief Notify to all variables that a new year is about to start
    **
    ** \param year The current year
    */
    void yearBegin(uint year);

    /*!
    ** \brief Notify to all variables that the year is close to end
    **
    ** All variables that have specific updates after a whole year calculation
    ** can now get their results
    ** \param state The current thermal cluster
    ** \param year The current year
    */
    void yearEndBuild(State& state, uint year);

    /*!
    ** \brief Notify to all variables to prepare data for the close to year end calculations for
    *each thermal cluster
    **
    ** That mainly means that all variables that have build for the whole year to do
    ** should do it now
    ** \param state The current thermal cluster
    ** \param year The current year
    */
    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year);

    /*!
    ** \brief Notify to all variables that the year is close to end
    **
    ** All variables that have specific updates after a whole year calculation
    ** can now get their results
    ** \param state The current thermal cluster
    ** \param year The current year
    */
    void yearEndBuildForEachThermalCluster(State& state, uint year);

    /*!
    ** \brief Notify to all variables that the year is now over
    **
    ** That mainly means that all variables should perform the monthly
    ** aggragations.
    ** \param year The current year
    */
    void yearEnd(uint year);

    template<class V>
    void yearEndSpatialAggregates(V& allVars, uint year);

    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars, uint year, const SetT& set);

    template<class V>
    void simulationEndSpatialAggregates(V& allVars);

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V& allVars, const SetT& set);
    //@}

    //! \name Simulation Hours in the year
    //@{
    //! Event: A new hour in the year has just began
    void hourBegin(uint hourInTheYear);
    //! Event: For a given hour in the year, walking through all areas
    void hourForEachArea(State& state);
    //! Event: For a given hour in the year, walking through all thermal clusters
    // for a given area
    void hourForEachThermalCluster(State& state);
    //! Event: For a given hour in the year, walking through all links
    // for a given area
    void hourForEachLink(State& state, uint numSpace);
    //! Event: A new hour in the year has just ended
    void hourEnd(State& state, uint hourInTheYear);
    //@}

    //! \name Week
    //@{
    void weekBegin(State& state);
    void weekForEachArea(State& state, uint numSpace);

    void weekEnd(State& state);
    //@}

    //! \name User Reports
    //@{
    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 uint numSpace) const;

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;

    /*!
    ** \brief Event triggered before exporting a year-by-year survey report
    */
    void beforeYearByYearExport(uint year, uint numSpace);
    //@}

    //! \name Misc
    //@{
    /*!
    ** \brief Get the memory used by this variable and all other in the static list
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief "Print" informations about the variable tree
    */
    template<class I>
    static void provideInformations(I& infos);

    /*!
    ** \brief Compute the spatial cluster with the results of a single variable
    **
    ** \code
    ** variables.computeSpatialAggregateWith<CO2<>::VCardType>(out);
    ** \endcode
    */
    template<class VCardSearchT, class O>
    void computeSpatialAggregateWith(O& out, uint numSpace);

    /*!
    ** \brief Compute the spatial cluster with the results of a single variable and a given area
    **
    ** \code
    ** variables.computeSpatialAggregateWith<CO2<>::VCardType>(out);
    ** \endcode
    */
    template<class VCardSearchT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area);

    template<class VCardToFindT>
    const double* retrieveHourlyResultsForCurrentYear(uint numSpace) const;

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area);

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType** result,
                                          const Data::ThermalCluster* cluster);

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link);

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint column,
      uint) const;
    //@}

    //! The results
    const StoredResultType& results() const;

protected:
    //! All the results about this variable
    StoredResultType pResults;
    // Does current output variable appears non applicable in all output files.
    // Following is an array of size :
    // + 1 if variable is a single variable
    // + > 1 if variable is a multi-variable
    bool* isNonApplicable;
    // Do we print results regarding the current variable in output files ? Or do we skip them ?
    bool* isPrinted;
    // Positive column count (original column count can be < 0 for some variable [see variables "by
    // plant"])
    uint pColumnCount;

}; // class Variable

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "variable.hxx"

#endif // __SOLVER_VARIABLE_VARIABLE_H__
