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
#ifndef __SOLVER_VARIABLE_INC_LINK_H__
#define __SOLVER_VARIABLE_INC_LINK_H__

// NOTE : template includes are used here to reduce template instanciation
// which still seems to be really cpu/memory consuming

#include <vector>

#include "antares/solver/variable/info.h"
#include "antares/solver/variable/storage/results.h"

#include "../../state.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{

struct VCardAllLinks
{
    //! Caption
    static std::string Caption()
    {
        return "Links";
    }

    //! Unit
    static std::string Unit()
    {
        return "";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "";
    }

    //! The expecte results
    using ResultsType = Results<>;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +1;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 0;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 0;

}; // class VCard

template<class VariablePerLink>
class Links
{
public:
    //! Type of the next static variable
    using NextType = VariablePerLink;
    //! VCard
    using VCardType = VCardAllLinks;
    //! Ancestor
    // using AncestorType = Variable::IVariable<Links<NextT>, NextT, VCardType>;

    //! List of expected results
    using ResultsType = VCardType::ResultsType;

    enum
    {
        //! How many items have we got
        count = NextType::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = NextType::template Statistics < CDataLevel,
            CFile > ::count
        };
    };

public:
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
    ** \brief Default constructor
    */
    Links();
    //! Destructor
    ~Links();
    //@}

    void initializeFromStudy(Data::Study& study);
    void initializeFromArea(Data::Study*, Data::Area*);
    void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*);
    void initializeFromAreaLink(Data::Study*, Data::AreaLink*);

    void broadcastNonApplicability(bool applyNonApplicable);
    void getPrintStatusFromStudy(Data::Study& study);
    void supplyMaxNumberOfColumns(Data::Study& study);

    void simulationBegin();
    void simulationEnd();

    void yearBegin(uint year, unsigned int numSpace);

    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, uint numSpace);
    void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace);

    void yearEndBuild(State& state, uint year);

    void yearEnd(uint year, uint numSpace);

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary);

    void weekBegin(State& state);

    void weekForEachArea(State& state, uint numSpace);
    void weekEnd(State& state);

    void hourBegin(uint hourInTheYear);
    void hourForEachArea(State& state, uint numSpace);
    void hourForEachLink(State& state, uint numSpace);

    void hourEnd(State& state, uint hourInTheYear);

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

    uint64_t memoryUsage() const;

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;

    template<class I>
    static void provideInformations(I& infos);

    template<class VCardToFindT>
    inline const double* retrieveHourlyResultsForCurrentYear(uint) const
    {
        return nullptr;
    }

    template<class VCardToFindT>
    void retrieveResultsForArea(Storage<VCardToFindT>::ResultsType** result, const Data::Area*)
    {
        *result = NULL;
    }

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(Storage<VCardToFindT>::ResultsType** result,
                                          const Data::ThermalCluster*)
    {
        *result = NULL;
    }

    template<class VCardToFindT>
    void retrieveResultsForLink(Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link)
    {
        pLinks[link->indexForArea].template retrieveResultsForLink<VCardToFindT>(result, link);
    }

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O&, uint)
    {
        // Do nothing
    }

public:
    //! Area list
    NextType* pLinks;
    //! The total number of links
    uint pLinkCount;

}; // class Links

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "links.hxx"

#endif // __SOLVER_VARIABLE_INC_LINK_H__
