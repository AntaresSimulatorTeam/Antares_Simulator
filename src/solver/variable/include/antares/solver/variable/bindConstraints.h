// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "state.h"

namespace Antares::Solver::Variable
{
struct VCardAllBindingConstraints
{
    //! Caption
    static std::string Caption()
    {
        return "Binding constraints";
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
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::bindingConstraint;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & Category::FileLevel::bc;
    //! Number of columns used by the variable (one results configuration per column)
    static constexpr int columnCount = 0;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;

}; // class VCardAllBindingConstraints

template<class VariableList>
class BindingConstraints
{
public:
    //! VCard
    typedef VCardAllBindingConstraints VCardType;

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
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    BindingConstraints() = default;

    BindingConstraints(BindingConstraints& other) = delete;
    BindingConstraints(BindingConstraints&& other) = delete;
    const BindingConstraints& operator=(BindingConstraints&& other) = delete;

    //! Destructor
    ~BindingConstraints() = default;
    //@}

    void initializeFromStudy(Data::Study& study);

    void computeSummary(unsigned int year, unsigned int numSpace);

    void simulationBegin();
    void simulationEnd();

    void yearBegin(uint year, uint numSpace);
    void yearEnd(uint year, uint numSpace);

    void buildThermalClusterYearEndResults(State& state, uint year, uint numSpace);

    void weekBegin(State& state);
    void weekEnd(State& state);
    void weekForEachArea(State&, unsigned int numSpace);
    void hourForEachArea(State&, unsigned int numSpace);

    void hourBegin(uint hourInTheYear);
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

    template<class V>
    void yearEndSpatialAggregates(V&, uint, uint)
    {
        // do nothing
    }

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area);
    void buildDigest(SurveyResults&, int digestLevel, int dataLevel) const;

    template<class V>
    void simulationEndSpatialAggregates(V& allVars);

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link);

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType** result,
                                          const Data::ThermalCluster* cluster);
    template<class VCardSearchT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace);
    template<class V>
    void computeSpatialAggregatesSummary(V& allVars, unsigned int year, unsigned int numSpace);

    void beforeYearByYearExport(uint year, uint numSpace);

private:
    // For each binding constraint, output variable static list associated.
    std::vector<VariableList> pBindConstraints;
    // The number of counted binding constraints
    uint pBCcount;

}; // class Areas

} // namespace Antares::Solver::Variable

#include "bindConstraints.hxx"
