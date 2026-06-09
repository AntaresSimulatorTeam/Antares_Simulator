// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_AREA_H__
#define __SOLVER_VARIABLE_AREA_H__

#include <vector>

#include "state.h"

namespace Antares::Solver::Variable
{
struct VCardAllAreas
{
    //! Caption
    static std::string Caption()
    {
        return "Areas";
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

    //! Expected results configuration
    typedef Results<> ResultsType;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & Category::FileLevel::de;
    //! Number of columns used by the variable (one results configuration per column)
    static constexpr int columnCount = 0;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;

}; // class VCard

template<class VariableList>
class Areas
{
public:
    //! VCard
    typedef VCardAllAreas VCardType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    enum
    {
        //! How many items have we got
        count = VariableList::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = VariableList::template Statistics < CDataLevel,
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
    void initializeFromStudy(Data::Study& study);
    void initializeFromArea(Data::Study*, Data::Area*);
    void initializeFromLink(Data::Study*, Data::AreaLink*);

    void simulationBegin();
    void simulationEnd();

    void yearBegin(uint year, uint numSpace);
    //	void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year);
    //	void yearEndBuildForEachThermalCluster(State& state, uint year);

    void buildThermalClusterYearEndResults(State& state, uint year, uint numSpace);

    void yearEnd(uint year, uint numSpace);

    void computeSummary(unsigned int year, unsigned int numSpace);

    void hourBegin(uint hourInTheYear);

    void hourForEachArea(State& state, uint numSpace);
    void hourForEachLink(State& state, uint numSpace);

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

    void buildDigest(SurveyResults&, int digestLevel, int dataLevel) const;

    void beforeYearByYearExport(uint year, uint numSpace);

    template<class V>
    void yearEndSpatialAggregates(V&, uint, uint)
    {
        // do nothing
    }

    template<class V>
    void computeSpatialAggregatesSummary(V&, unsigned int, unsigned int)
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
    std::vector<VariableList> pAreas;

}; // class Areas

} // namespace Antares::Solver::Variable

#include "area.hxx"

#endif // __SOLVER_VARIABLE_AREA_H__
