// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE input_generation_series

#define WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/study/parts/renewable/cluster.h>
#include <antares/study/study.h>
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/simulation/simulation.h"

using namespace Antares::Data;

namespace
{
constexpr int nbHoursInWeek = 168;

struct Fixture
{
    std::unique_ptr<Study> study;
    Area* area = nullptr;
    PROBLEME_HEBDO problem;

    Fixture()
    {
        study = std::make_unique<Study>();
        study->parameters.reset();

        area = addAreaToListOfAreas(study->areas, "france");
        area->createMissingData();
        area->resetToDefaultValues();
        study->areas.rebuildIndexes();

        problem.year = 0;
        problem.NombreDePasDeTemps = nbHoursInWeek;
    }

    std::shared_ptr<RenewableCluster> addRenewableCluster(const std::string& name)
    {
        auto cluster = std::make_shared<RenewableCluster>(area);
        cluster->setName(name);
        cluster->enabled = true;
        cluster->series.reset(1, HOURS_PER_YEAR);
        area->renewable.list.addToCompleteList(cluster);
        area->renewable.list.buildIndexes();
        return cluster;
    }

    // All components of the study's single area, by name.
    std::vector<std::string> componentNames()
    {
        fillInputGenerationSeries(*study, problem, 0);
        std::vector<std::string> names;
        for (const auto& entry: problem.InputGenerationOfArea.at(0))
        {
            names.push_back(entry.componentName);
        }
        return names;
    }
};

bool contains(const std::vector<std::string>& names, const std::string& name)
{
    return std::find(names.begin(), names.end(), name) != names.end();
}
} // namespace

BOOST_FIXTURE_TEST_SUITE(input_generation_series, Fixture)

BOOST_AUTO_TEST_CASE(renewable_cluster_component_uses_zone_renewable_cluster_naming)
{
    study->parameters.renewableGeneration.toClusters();
    addRenewableCluster("wind onshore");

    const auto names = componentNames();

    // Same convention as the view-builder's system-for-views:
    // {zone}_renewable_{cluster id}
    BOOST_CHECK(contains(names, "france_renewable_wind onshore"));
    BOOST_CHECK(!contains(names, "wind onshore"));
}

BOOST_AUTO_TEST_CASE(several_renewable_clusters_are_all_prefixed)
{
    study->parameters.renewableGeneration.toClusters();
    addRenewableCluster("wind_fr");
    addRenewableCluster("pv_fr");

    const auto names = componentNames();

    BOOST_CHECK(contains(names, "france_renewable_wind_fr"));
    BOOST_CHECK(contains(names, "france_renewable_pv_fr"));
}

BOOST_AUTO_TEST_CASE(disabled_renewable_cluster_produces_no_component)
{
    study->parameters.renewableGeneration.toClusters();
    addRenewableCluster("wind_fr")->enabled = false;

    const auto names = componentNames();

    BOOST_CHECK(!contains(names, "france_renewable_wind_fr"));
}

BOOST_AUTO_TEST_CASE(aggregated_mode_keeps_wind_and_solar_components)
{
    study->parameters.renewableGeneration.toAggregated();
    addRenewableCluster("wind_fr");

    const auto names = componentNames();

    BOOST_CHECK(contains(names, "france_wind"));
    BOOST_CHECK(contains(names, "france_solar"));
    BOOST_CHECK(!contains(names, "france_renewable_wind_fr"));
}

BOOST_AUTO_TEST_SUITE_END()
