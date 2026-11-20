// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test near price cap variables"
#define WIN32_LEAN_AND_MEAN

#include <array>
#include <memory>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/variable/commons/spatial-aggregate.h"
#include "antares/solver/variable/economy/nearPriceCap.h"
#include "antares/writer/in_memory_writer.h"

using namespace Antares::Solver::Variable;
using namespace Antares::Solver::Variable::Economy;

namespace
{
using Variables = Container::TupleVariableList<NearPriceCap, NearPriceCapCSR>;

struct Fixture
{
    Fixture():
        study(std::make_unique<Antares::Data::Study>()),
        writer(durationCollector)
    {
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->parameters.nbYears = 1;
        study->maxNbYearsInParallel = 1;
        study->initializeRuntimeInfos();
        study->parameters.resetPlaylist(1);
        area = Antares::Data::addAreaToListOfAreas(study->areas, "area");
        area->thermal.unsuppliedEnergyCost = 100.;

        Antares::Data::VariablePrintInfo info(Category::FileLevel::va,
                                              Category::DataLevel::area
                                                | Category::DataLevel::setOfAreas);
        info.setMaxColumns(4);
        study->parameters.variablesPrintInfo.add("NPCAP HOURS", info);
        study->parameters.variablesPrintInfo.add("NPCAP HOURS CSR", info);
        study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
        study->parameters.variablesPrintInfo.prepareForSimulation(false);
    }

    void feed(Variables& variable,
              const std::vector<double>& prices,
              const std::vector<double>& csrPrices)
    {
        variable.initializeFromStudy(*study);
        variable.initializeFromArea(study.get(), area);
        variable.simulationBegin();
        variable.yearBegin(0, 0);
        RESULTATS_HORAIRES hourly;
        for (double price: prices)
        {
            hourly.CoutsMarginauxHoraires.push_back(-price);
        }
        for (double price: csrPrices)
        {
            hourly.CoutsMarginauxHorairesCSR.push_back(-price);
        }
        State state(*study);
        state.hourlyResults = &hourly;
        for (unsigned int hour = 0; hour < prices.size(); ++hour)
        {
            state.hourInTheYear = hour;
            state.hourInTheWeek = hour;
            variable.hourForEachArea(state, 0);
        }
        variable.yearEnd(0, 0);
    }

    Benchmarking::DurationCollector durationCollector;
    std::unique_ptr<Antares::Data::Study> study;
    Antares::Solver::InMemoryWriter writer;
    Antares::Data::Area* area;
};

struct AreaVariables
{
    Variables& first;
    Variables& second;

    template<class Card, class Output>
    void computeSpatialAggregateWith(Output& output, const Antares::Data::Area* area, uint numSpace)
    {
        auto& variable = area->index == 0 ? first : second;
        variable.template computeSpatialAggregateWith<Card>(output, numSpace);
    }
};
} // namespace

BOOST_FIXTURE_TEST_CASE(csr_price_and_threshold, Fixture)
{
    Variables variables;
    feed(variables, {100., 90., 100., 90., 90.}, {90., 100., 95., 95.01, 95.02});
    IntermediateValues regular;
    IntermediateValues csr;
    regular.initializeFromStudy(*study);
    csr.initializeFromStudy(*study);
    regular.reset();
    csr.reset();
    variables.computeSpatialAggregateWith<NearPriceCap::VCardType>(regular, 0u);
    variables.computeSpatialAggregateWith<NearPriceCapCSR::VCardType>(csr, 0u);
    const std::array<double, 5> expectedRegular{1., 0., 1., 0., 0.};
    const std::array<double, 5> expectedCsr{0., 1., 0., 0., 1.};
    for (unsigned int hour = 0; hour < expectedRegular.size(); ++hour)
    {
        BOOST_CHECK_EQUAL(regular[hour], expectedRegular[hour]);
        BOOST_CHECK_EQUAL(csr[hour], expectedCsr[hour]);
    }
    variables.getPrintStatusFromStudy(*study);
    SurveyResults report(*study, 4, "out", writer);
    report.data.columnIndex = 0;
    variables.buildAnnualSurveyReport(report,
                                      Category::DataLevel::area,
                                      Category::FileLevel::va,
                                      Category::annual,
                                      0);
    BOOST_REQUIRE_EQUAL(report.data.columnIndex, 2);
    BOOST_CHECK_EQUAL(report.values[0][0], 2.);
    BOOST_CHECK_EQUAL(report.values[1][0], 2.);

    variables.yearBegin(1, 0);
    regular.reset();
    csr.reset();
    variables.computeSpatialAggregateWith<NearPriceCap::VCardType>(regular, 0u);
    variables.computeSpatialAggregateWith<NearPriceCapCSR::VCardType>(csr, 0u);
    for (unsigned int hour = 0; hour < expectedRegular.size(); ++hour)
    {
        BOOST_CHECK_EQUAL(regular[hour], 0.);
        BOOST_CHECK_EQUAL(csr[hour], 0.);
    }
}

BOOST_FIXTURE_TEST_CASE(csr_spatial_and_temporal_aggregation, Fixture)
{
    Variables first;
    Variables second;
    feed(first, {100., 100., 100., 100.}, {100., 100., 90., 90.});
    feed(second, {100., 100., 100., 100.}, {100., 90., 100., 90.});
    area->index = 0;
    auto* otherArea = Antares::Data::addAreaToListOfAreas(study->areas, "other");
    otherArea->index = 1;
    std::vector<const Antares::Data::Area*> areas{area, otherArea};
    AreaVariables areaVariables{first, second};
    Common::SpatialAggregate<NearPriceCapCSR> aggregate;
    aggregate.initializeFromStudy(*study);
    aggregate.getPrintStatusFromStudy(*study);
    aggregate.yearEndSpatialAggregates(areaVariables, 0, areas, 0);
    SurveyResults report(*study, 4, "out", writer);
    for (const auto precision:
         {Category::hourly, Category::daily, Category::weekly, Category::monthly, Category::annual})
    {
        report.data.columnIndex = 0;
        aggregate.buildAnnualSurveyReport(report,
                                          Category::DataLevel::setOfAreas,
                                          Category::FileLevel::va,
                                          precision,
                                          0);
        BOOST_REQUIRE_EQUAL(report.data.columnIndex, 1);
        if (precision == Category::hourly)
        {
            for (unsigned int hour = 0; hour < 4; ++hour)
            {
                BOOST_CHECK_EQUAL(report.values[0][hour], hour < 3 ? 1. : 0.);
            }
        }
        else
        {
            BOOST_CHECK_EQUAL(report.values[0][0], 3.);
        }
    }
}
