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
#ifndef __SOLVER_END_OF_LIST_END_OF_LIST_H__
#define __SOLVER_END_OF_LIST_END_OF_LIST_H__

#include <yuni/yuni.h>

#include <antares/study/study.h>

#include "state.h"
#include "surveyresults.h"

// To remove warnings (unused variable) at compile time on linux
#define UNUSED_VARIABLE(x) (void)(x)

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Container
{
class EndOfList
{
public:
    //! Type of the next static variable
    typedef void NextType;

    enum
    {
        //! How many items have we got
        count = 0,
    };

    template<int CategoryDataLevel, int CategoryFile>
    struct Statistics
    {
        enum
        {
            count = 0
        };
    };

public:
    /*!
    ** \brief Retrieve the list of all individual variables
    **
    ** The predicate must implement the method `add(name, unit, comment)`.
    */
    template<class PredicateT>
    static void RetrieveVariableList(PredicateT&)
    {
    }

public:
    //! \name Constructor & Destructor
    //@{
    EndOfList()
    {
    }

    ~EndOfList()
    {
    }

    //@}

    static void initializeFromStudy([[maybe_unused]] Data::Study& study)
    {
    }

    static void initializeFromArea(Data::Study*, Data::Area*)
    {
    }

    static void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*)
    {
    }

    static void initializeFromAreaLink(Data::Study*, Data::AreaLink*)
    {
    }

    void broadcastNonApplicability(bool)
    {
    }

    void getPrintStatusFromStudy([[maybe_unused]] Data::Study& study)
    {
    }

    void supplyMaxNumberOfColumns([[maybe_unused]] Data::Study& study)
    {
    }

    static void simulationBegin()
    {
    }

    static void simulationEnd()
    {
    }

    static void yearBegin(unsigned int, unsigned int)
    {
    }

    static void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, uint numSpace)
    {
        UNUSED_VARIABLE(state);
        UNUSED_VARIABLE(year);
        UNUSED_VARIABLE(numSpace);
    }

    static void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace)
    {
        UNUSED_VARIABLE(state);
        UNUSED_VARIABLE(year);
        UNUSED_VARIABLE(numSpace);
    }

    static void yearEndBuild(State&, unsigned int, unsigned int)
    {
    }

    static void yearEnd(unsigned int, unsigned int)
    {
    }

    static void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                               uint nbYearsForCurrentSummary)
    {
        UNUSED_VARIABLE(numSpaceToYear);
        UNUSED_VARIABLE(nbYearsForCurrentSummary);
    }

    template<class V>
    void yearEndSpatialAggregates(V&, unsigned int, uint)
    {
    }

    template<class V, class SetT>
    void yearEndSpatialAggregates(V&, unsigned int, const SetT&, uint numSpace)
    {
        UNUSED_VARIABLE(numSpace);
    }

    template<class V>
    void computeSpatialAggregatesSummary(V&, std::map<unsigned int, unsigned int>&, unsigned int)
    {
    }

    template<class V>
    void simulationEndSpatialAggregates(V&)
    {
    }

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V&, const SetT&)
    {
    }

    static void weekBegin(State&)
    {
    }

    static void weekForEachArea(State&, uint numSpace)
    {
        UNUSED_VARIABLE(numSpace);
    }

    static void weekEnd(State&)
    {
    }

    static void hourBegin(unsigned int)
    {
    }

    static void hourForEachArea(State&, uint numSpace)
    {
        UNUSED_VARIABLE(numSpace);
    }

    static void hourForEachLink(State&, uint numSpace)
    {
        UNUSED_VARIABLE(numSpace);
    }

    static void hourEnd(State&, unsigned int)
    {
    }

    static void buildSurveyReport(SurveyResults&, int, int, int)
    {
    }

    static void buildAnnualSurveyReport(SurveyResults&, int, int, int, uint)
    {
    }

    static void buildDigest(SurveyResults&, int, int)
    {
    }

    static void beforeYearByYearExport(uint /*year*/, uint)
    {
    }

    static uint64_t memoryUsage()
    {
        return 0;
    }

    template<class I>
    static void provideInformations(I&)
    {
    }

    template<class SearchVCardT, class O>
    static void computeSpatialAggregateWith(O&)
    {
        assert(false);
    }

    template<class SearchVCardT, class O>
    static void computeSpatialAggregateWith(O&, const Data::Area*)
    {
        assert(false);
    }

    template<class VCardToFindT>
    const double* retrieveHourlyResultsForCurrentYear(uint) const
    {
        return nullptr;
    }

    template<class VCardToFindT, class O>
    static void retrieveResultsForArea(O** /*result*/, const Data::Area*)
    {
        // Does nothing - this method may be called from a leaf
        // Consequently we can not throw an error from here if the variable `result`
        // is not initialized.
    }

    template<class VCardToFindT, class O>
    static void retrieveResultsForThermalCluster(O** /*result*/, const Data::ThermalCluster*)
    {
        // Does nothing - this method may be called from a leaf
        // Consequently we can not throw an error from here if the variable `result`
        // is not initialized.
    }

    template<class VCardToFindT, class O>
    static void retrieveResultsForLink(O** /*result*/, const Data::AreaLink*)
    {
        // Does nothing - this method may be called from a leaf
        // Consequently we can not throw an error from here if the variable `result`
        // is not initialized.
    }

    void localBuildAnnualSurveyReport(SurveyResults&, int, int, unsigned int) const
    {
        // Does nothing
    }

}; // class EndOfList

} // namespace Container
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_END_OF_LIST_END_OF_LIST_H__
