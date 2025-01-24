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
#ifndef __SOLVER_VARIABLE_SET_OF_AREAS_H__
#define __SOLVER_VARIABLE_SET_OF_AREAS_H__

#include <antares/study/study.h>

#include "state.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
struct VCardAllSetsOfAreas
{
    //! Caption
    static std::string Caption()
    {
        return "Sets of Areas";
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
    typedef Results<> ResultsType;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & Category::FileLevel::de;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +1;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 0;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 0;

}; // class VCard

template<class NextT = Container::EndOfList>
class SetsOfAreas
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardAllSetsOfAreas VCardType;
    //! Ancestor
    typedef Variable::IVariable<SetsOfAreas<NextT>, NextT, VCardType> AncestorType;

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
    ** \brief Default Constructor
    */
    SetsOfAreas() = default;
    //! Destructor
    ~SetsOfAreas() = default;
    //@}

    void initializeFromStudy(Data::Study& study);
    void initializeFromArea(Data::Study*, Data::Area*);
    void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*);
    void initializeFromAreaLink(Data::Study*, Data::AreaLink*);

    void simulationBegin();
    void simulationEnd();

    void yearBegin(unsigned int year, unsigned int numSpace);

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace);

    void yearEnd(unsigned int year, unsigned int numSpace);

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary);

    void hourBegin(unsigned int hourInTheYear);
    void hourForEachArea(State& state, unsigned int numSpace);
    void hourForEachLink(State& state);
    void hourEnd(State& state, unsigned int hourInTheYear);

    void weekBegin(State&);
    void weekForEachArea(State&, unsigned int numSpace);
    void weekEnd(State&);

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 unsigned int numSpace) const;

    void buildDigest(SurveyResults&, int digestLevel, int dataLevel) const;

    void beforeYearByYearExport(uint year, uint numSpace);

    template<class I>
    static void provideInformations(I& infos);

    template<class V>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, unsigned int numSpace);

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars,
                                         std::map<unsigned int, unsigned int>& numSpaceToYear,
                                         unsigned int);

    template<class V>
    void simulationEndSpatialAggregates(V& allVars);

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

public:
    //! Area list
    typedef std::vector<std::unique_ptr<NextType>> SetOfAreasVector;
    //! Area list
    SetOfAreasVector pSetsOfAreas;
    //! Reference to the origina set
    std::vector<const Data::Sets::SetAreasType*> pOriginalSets;
    //! The study
    const Data::Study* pStudy;

    typename Data::Study::SetsOfAreas::IDType::Vector pNames;

}; // class SetsOfAreas

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "setofareas.hxx"

#endif // __SOLVER_VARIABLE_SET_OF_AREAS_H__
