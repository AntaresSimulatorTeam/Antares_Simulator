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
#ifndef __SOLVER_VARIABLE_SET_OF_AREAS_H__
#define __SOLVER_VARIABLE_SET_OF_AREAS_H__

#include "state.h"
#include <antares/study.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
struct VCardAllSetsOfAreas
{
    //! Caption
    static const char* Caption()
    {
        return "Sets of Areas";
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
            count = NextType::template Statistics<CDataLevel, CFile>::count
        };
    };

public:
    /*!
    ** \brief Try to estimate the memory footprint that the solver will require to make a simulation
    */
    static void EstimateMemoryUsage(Data::StudyMemoryUsage&);

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
    SetsOfAreas();
    //! Destructor
    ~SetsOfAreas();
    //@}

    void initializeFromStudy(Data::Study& study);
    void initializeFromArea(Data::Study*, Data::Area*);
    void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*);
    void initializeFromAreaLink(Data::Study*, Data::AreaLink*);

    void simulationBegin();
    void simulationEnd();
    void logCaption(std::map<const char*, int>&);

    void yearBegin(unsigned int year, unsigned int numSpace);

    void yearEndBuild(State& state, unsigned int year);

    void yearEnd(unsigned int year, unsigned int numSpace);

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary);

    void hourBegin(unsigned int hourInTheYear);
    void hourForEachArea(State& state);
    void hourForEachLink(State& state);
    void hourForEachThermalCluster(State& state);
    void hourEnd(State& state, unsigned int hourInTheYear);

    void weekBegin(State&);
    void weekForEachArea(State&);
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

    Yuni::uint64 memoryUsage() const;

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
    void computeSpatialAggregateWith(O& out, const Data::Area* area);

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
    typedef std::vector<NextType*> SetOfAreasVector;
    //! Area list
    SetOfAreasVector pSetsOfAreas;
    //! Reference to the origina set
    std::vector<const Data::Study::SingleSetOfAreas*> pOriginalSets;
    //! An iterator for the begining of the list
    typename SetOfAreasVector::iterator pBegin;
    //! An iterator to the end of the list
    typename SetOfAreasVector::iterator pEnd;
    //! The study
    const Data::Study* pStudy;

    typename Data::Study::SetsOfAreas::IDType::Vector pNames;

}; // class SetsOfAreas

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "setofareas.hxx"

#endif // __SOLVER_VARIABLE_SET_OF_AREAS_H__
