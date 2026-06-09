// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_INC_LINK_H__
#define __SOLVER_VARIABLE_INC_LINK_H__

#include <vector>

#include "antares/solver/variable/info.h"
#include "antares/solver/variable/storage/results.h"
#include "antares/solver/variable/tuple_variable_list.h"

#include "../../state.h"

namespace Antares::Solver::Variable
{

struct VCardAllLinks
{
    static std::string Caption()
    {
        return "Links";
    }

    static std::string Unit()
    {
        return "";
    }

    static const char* Description()
    {
        return "";
    }

    using ResultsType = Results<>;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile;
    static constexpr int columnCount = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

}; // class VCard

template<class VariablePerLinkList>
class Links
{
public:
    using VariablePerLink = VariablePerLinkList;
    using VCardType = VCardAllLinks;
    using ResultsType = VCardType::ResultsType;

    static constexpr std::size_t count = VariablePerLink::count;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = VariablePerLink::template Statistics<CDataLevel, CFile>::count;
    };

public:
    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate)
    {
        VariablePerLink::RetrieveVariableList(predicate);
    }

    void initializeFromStudy(Data::Study& study);
    void initializeFromArea(Data::Study*, Data::Area*);
    void initializeFromLink(Data::Study*, Data::AreaLink*);

    void broadcastNonApplicability(bool applyNonApplicable);
    void getPrintStatusFromStudy(Data::Study& study);
    void supplyMaxNumberOfColumns(Data::Study& study);

    void simulationBegin();
    void simulationEnd();

    void yearBegin(uint year, unsigned int numSpace);

    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, uint numSpace);
    void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace);

    void buildThermalClusterYearEndResults(State& state, uint year, uint numSpace);

    void yearEnd(uint year, uint numSpace);

    void computeSummary(unsigned int year, unsigned int numSpace);

    void weekBegin(State& state);

    void weekForEachArea(State& state, uint numSpace);
    void weekEnd(State& state);

    void hourBegin(uint hourInTheYear);
    void hourForEachArea(State& state, unsigned int numSpace);
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

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;

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
    }

public:
    std::vector<VariablePerLink> pLinks;

}; // class Links

} // namespace Antares::Solver::Variable

#include "links.hxx"

#endif // __SOLVER_VARIABLE_INC_LINK_H__
