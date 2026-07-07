// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_SET_OF_AREAS_H__
#define __SOLVER_VARIABLE_SET_OF_AREAS_H__

#include <antares/study/study.h>

#include "container-scope-defaults.h"
#include "state.h"
#include "variable.h"

namespace Antares::Solver::Variable
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
class SetsOfAreas: public ContainerScopeDefaults
{
public:
    //! VCard
    typedef VCardAllSetsOfAreas VCardType;

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
    SetsOfAreas() = default;
    //! Destructor
    ~SetsOfAreas() = default;
    //@}

    void initializeFromStudy(Data::Study& study);

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

    template<class V>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, unsigned int numSpace);

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars, unsigned int year, unsigned int numSpace);

    template<class V>
    void simulationEndSpatialAggregates(V& allVars);

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

    const VariableList* findSetById(const Data::Study::SetsOfAreas::IDType& setId) const;

public:
    //! Area list
    typedef std::vector<std::unique_ptr<VariableList>> SetOfAreasVector;
    //! Area list
    SetOfAreasVector pSetsOfAreas;
    //! Reference to the original set
    std::vector<const Data::Sets::SetAreasType*> pOriginalSets;
    //! The study
    const Data::Study* pStudy;

    typename Data::Study::SetsOfAreas::IDType::Vector pIds;
    typename Data::Study::SetsOfAreas::IDType::Vector pNames;

}; // class SetsOfAreas

} // namespace Antares::Solver::Variable

#include "setofareas.hxx"

#endif // __SOLVER_VARIABLE_SET_OF_AREAS_H__
