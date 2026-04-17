// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test set of areas reports"

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/container.h"
#include "antares/solver/variable/setofareas.h"
#include "antares/solver/variable/variable.h"
#include "antares/study/area/area.h"
#include "antares/study/parts/common/cluster.h"
#include "antares/study/sets.h"
#include "antares/study/variable-print-info.h"
#include "antares/writer/in_memory_writer.h"

using namespace Antares::Solver::Variable;

namespace
{

struct VCardSetReportVariable
{
    static std::string Caption()
    {
        return "Set Report Variable";
    }

    static std::string Unit()
    {
        return "MW";
    }

    static std::string Description()
    {
        return "Test-only set report variable";
    }

    using ResultsType = Results<>;
};

template<class NextT = Container::EndOfList>
class SetReportVariable: public NextT
{
public:
    using NextType = NextT;

    enum
    {
        count = 1,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = 0,
        };
    };

    void initializeFromStudy(Antares::Data::Study& study)
    {
        (void)study;
        NextT::initializeFromStudy(study);
    }

    void broadcastNonApplicability(bool)
    {
    }

    void getPrintStatusFromStudy(Antares::Data::Study& study)
    {
        (void)study;
        isPrinted_[0] = true;
        isNonApplicable_[0] = false;
        NextT::getPrintStatusFromStudy(study);
    }

    void buildSurveyReport(SurveyResults& results, int, int, int) const
    {
        results.isPrinted = const_cast<bool*>(isPrinted_);
        results.isCurrentVarNA = const_cast<bool*>(isNonApplicable_);
        NextT::buildSurveyReport(results, 0, 0, 0);
    }

    void buildAnnualSurveyReport(SurveyResults& results, int, int, int, unsigned int) const
    {
        results.isPrinted = const_cast<bool*>(isPrinted_);
        results.isCurrentVarNA = const_cast<bool*>(isNonApplicable_);
        NextT::buildAnnualSurveyReport(results, 0, 0, 0, 0);
    }

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars, unsigned int year, unsigned int numSpace)
    {
        NextT::computeSpatialAggregatesSummary(allVars, year, numSpace);
    }

private:
    bool isPrinted_[1] = {true};
    bool isNonApplicable_[1] = {false};
};

using TestVariableTree = Container::List<SetsOfAreas<SetReportVariable<>>>;

struct DistrictDefinition
{
    std::string id;
    std::string caption;
    bool output = true;
};

std::unique_ptr<Antares::Data::Study> makeStudyWithDistricts(
  const std::vector<DistrictDefinition>& districts)
{
    auto study = std::make_unique<Antares::Data::Study>();
    study->parameters.simulationDays.first = 0;
    study->parameters.simulationDays.end = 7;
    study->parameters.nbYears = 1;
    study->maxNbYearsInParallel = 1;
    study->initializeRuntimeInfos();
    study->parameters.resetPlaylist(study->parameters.nbYears);

    auto* area = Antares::Data::addAreaToListOfAreas(study->areas, "area1");
    area->index = 0;

    auto cluster = std::make_shared<Antares::Data::ThermalCluster>(area);
    cluster->setName("thermal1");
    cluster->setGroup("group1");
    cluster->index = 0;
    cluster->enabled = true;
    area->thermal.list.addToCompleteList(cluster);

    for (const auto& district: districts)
    {
        auto set = std::make_shared<Antares::Data::Sets::SetAreasType>();
        set->insert(area);

        Antares::Data::Sets::Options options;
        options.caption = district.caption;
        options.output = district.output;
        options.resultSize = 1;

        study->setsOfAreas.add(district.id, set, options);
    }
    study->setsOfAreas.rebuildIndexes();

    Antares::Data::VariablePrintInfo variableInfo(Category::FileLevel::va,
                                                  Category::DataLevel::area);
    variableInfo.setMaxColumns(0);
    study->parameters.variablesPrintInfo.add("dummy", variableInfo);

    Antares::Data::VariablePrintInfo setVariableInfo(Category::FileLevel::va,
                                                     Category::DataLevel::setOfAreas);
    setVariableInfo.setMaxColumns(1);
    study->parameters.variablesPrintInfo.add("Set Report Variable", setVariableInfo);

    Antares::Data::VariablePrintInfo flowLinInfo(Category::FileLevel::va,
                                                 Category::DataLevel::link);
    flowLinInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW LIN.", flowLinInfo);

    Antares::Data::VariablePrintInfo flowQuadInfo(Category::FileLevel::va,
                                                  Category::DataLevel::link);
    flowQuadInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW QUAD.", flowQuadInfo);

    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    study->parameters.variablesPrintInfo.prepareForSimulation(false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW LIN.", false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW QUAD.", false);
    study->parameters.variablesPrintInfo.computeMaxColumnsCountInReports(study->setsOfAreas);

    return study;
}

PROBLEME_HEBDO makeProblemHebdo()
{
    PROBLEME_HEBDO pb;
    pb.HeureDansLAnnee = 0;
    pb.year = 0;
    pb.NombreDePays = 1;
    pb.ResultatsHoraires.resize(1);

    auto& areaResults = pb.ResultatsHoraires[0];
    areaResults.ProductionThermique.resize(Antares::Constants::nbHoursInAWeek);

    for (unsigned h = 0; h < Antares::Constants::nbHoursInAWeek; ++h)
    {
        PRODUCTION_THERMIQUE_OPTIMALE thermalProd;
        thermalProd.ProductionThermiqueDuPalier.resize(1);
        thermalProd.ProductionThermiqueDuPalier[0] = 100.0 + h;
        areaResults.ProductionThermique[h] = thermalProd;
    }

    return pb;
}

void feedDynamicAggregation(TestVariableTree& variables, Antares::Data::Study& study)
{
    State state(study);
    PROBLEME_HEBDO pb = makeProblemHebdo();
    state.problemeHebdo = &pb;
    state.numSpace = 0;
    variables.weekBegin(state);
    variables.computeSpatialAggregatesSummary(variables, 0, 0);
}

} // namespace

BOOST_AUTO_TEST_SUITE(setofareas_reports)

BOOST_AUTO_TEST_CASE(exports_one_enabled_district)
{
    auto study = makeStudyWithDistricts({{"district-1", "District 1", true}});

    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    TestVariableTree variables;

    variables.initializeFromStudy(*study);
    feedDynamicAggregation(variables, *study);
    variables.exportSurveyResults(true, "out", 0, writer);

    const auto& files = writer.getMap();
    const auto fileIt = files.find("out/areas/@ district-1/values-hourly.txt");
    BOOST_REQUIRE(fileIt != files.end());
    BOOST_CHECK_NE(fileIt->second.find(
                     "\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD"),
                   std::string::npos);
    BOOST_CHECK_NE(fileIt->second.find("\tEXP\tstd\tmin\tmax"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_two_enabled_districts)
{
    auto study = makeStudyWithDistricts(
      {{"district-1", "District 1", true}, {"district-2", "District 2", true}});

    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    TestVariableTree variables;

    variables.initializeFromStudy(*study);
    feedDynamicAggregation(variables, *study);
    variables.exportSurveyResults(true, "out", 0, writer);

    const auto& files = writer.getMap();
    const auto firstFileIt = files.find("out/areas/@ district-1/values-hourly.txt");
    BOOST_REQUIRE(firstFileIt != files.end());
    BOOST_CHECK_NE(firstFileIt->second.find(
                     "\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD"),
                   std::string::npos);
    BOOST_CHECK_NE(firstFileIt->second.find("\tEXP\tstd\tmin\tmax"), std::string::npos);
    const auto secondFileIt = files.find("out/areas/@ district-2/values-hourly.txt");
    BOOST_REQUIRE(secondFileIt != files.end());
    BOOST_CHECK_NE(secondFileIt->second.find(
                     "\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD"),
                   std::string::npos);
    BOOST_CHECK_NE(secondFileIt->second.find("\tEXP\tstd\tmin\tmax"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(skips_disabled_district_and_exports_enabled_one)
{
    auto study = makeStudyWithDistricts({{"district-disabled", "Disabled district", false},
                                         {"district-enabled", "Enabled district", true}});

    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    TestVariableTree variables;

    variables.initializeFromStudy(*study);
    feedDynamicAggregation(variables, *study);
    variables.exportSurveyResults(true, "out", 0, writer);

    const auto& files = writer.getMap();
    BOOST_CHECK(files.find("out/areas/@ district-disabled/values-hourly.txt") == files.end());
    const auto fileIt = files.find("out/areas/@ district-enabled/values-hourly.txt");
    BOOST_REQUIRE(fileIt != files.end());
    BOOST_CHECK_NE(fileIt->second.find(
                     "\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD\tGROUP1_TH_PROD"),
                   std::string::npos);
    BOOST_CHECK_NE(fileIt->second.find("\tEXP\tstd\tmin\tmax"), std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
