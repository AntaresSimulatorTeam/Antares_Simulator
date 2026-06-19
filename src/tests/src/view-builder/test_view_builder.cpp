// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE view_builder_tests
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/study/area/area.h>
#include <antares/study/area/constants.h>
#include <antares/study/area/links.h>
#include <antares/study/parts/renewable/cluster.h>
#include <antares/study/parts/short-term-storage/cluster.h>
#include <antares/study/parts/thermal/cluster.h>
#include <antares/study/study.h>
#include <antares/view-builder/legacyToYaml.h>

#include <yaml-cpp/yaml.h>

using namespace Antares::Data;
using namespace Antares::ViewBuilder;

struct ViewBuilderFixture
{
    std::unique_ptr<Study> study;
    Area* fr;
    Area* de;
    std::shared_ptr<ThermalCluster> thermalCluster;
    std::shared_ptr<RenewableCluster> renewableCluster;

    ViewBuilderFixture()
    {
        study = std::make_unique<Study>();
        study->parameters.reset();

        fr = addAreaToListOfAreas(study->areas, "france");
        de = addAreaToListOfAreas(study->areas, "germany");

        for (auto* area : {fr, de})
        {
            area->createMissingData();
            area->resetToDefaultValues();
        }
        study->areas.rebuildIndexes();

        thermalCluster = std::make_shared<ThermalCluster>(fr);
        thermalCluster->setName("nuc_fr");
        fr->thermal.list.addToCompleteList(thermalCluster);

        renewableCluster = std::make_shared<RenewableCluster>(fr);
        renewableCluster->setName("wind_fr");
        fr->renewable.list.addToCompleteList(renewableCluster);

        {
            ShortTermStorage::STStorageCluster sts;
            sts.properties.name = "battery_fr";
            sts.id = "battery_fr";
            fr->shortTermStorage.storagesByIndex.push_back(sts);
        }

        AreaAddLinkBetweenAreas(fr, de);
    }
};

BOOST_FIXTURE_TEST_SUITE(view_builder_tests, ViewBuilderFixture)

BOOST_AUTO_TEST_CASE(study_to_yaml_structure)
{
    YAML::Node root = studyToSystemYaml(*study);
    BOOST_REQUIRE(root.IsMap());
    BOOST_REQUIRE(root["system"].IsDefined());

    auto sys = root["system"];
    BOOST_CHECK_EQUAL(sys["id"].as<std::string>(), "legacy_converted");

    auto libs = sys["model-libraries"];
    BOOST_REQUIRE(libs.IsSequence());
    BOOST_REQUIRE_EQUAL(libs.size(), 1);
    BOOST_CHECK_EQUAL(libs[0].as<std::string>(), "antares_legacy_models");

    auto components = sys["components"];
    BOOST_REQUIRE(components.IsSequence());

    // fr: area(1) + load(1) + wind(1) + solar(1) + ror(1) + miscGen(8) +
    //     thermal(1) + renewable(1) + sts(1) + hydro(1) + link(1) = 18
    // de: area(1) + load(1) + wind(1) + solar(1) + ror(1) + miscGen(8) +
    //     hydro(1) = 14
    // total: 32
    BOOST_CHECK_EQUAL(components.size(), 32);
}

BOOST_AUTO_TEST_CASE(area_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "area<france>")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.area");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 1);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(load_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "area<france>::Load")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.load");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 1);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(wind_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "area<france>::Wind")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.renewable");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 2);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
            BOOST_CHECK_EQUAL(props[1].as<std::string>(), "technology");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(thermal_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "area<france>::ThermalCluster<nuc_fr>")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.thermal");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 3);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "plant");
            BOOST_CHECK_EQUAL(props[1].as<std::string>(), "carrier");
            BOOST_CHECK_EQUAL(props[2].as<std::string>(), "technology");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(renewable_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "area<france>::RenewableCluster<wind_fr>")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.renewable");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 2);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
            BOOST_CHECK_EQUAL(props[1].as<std::string>(), "technology");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(sts_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "area<france>::ShortTermStorage<battery_fr>")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.short_term_storage");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 2);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
            BOOST_CHECK_EQUAL(props[1].as<std::string>(), "group");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(hydro_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "area<france>::Hydro")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.long_term_storage");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 2);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
            BOOST_CHECK_EQUAL(props[1].as<std::string>(), "group");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(link_component)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp : components)
    {
        if (comp["id"].as<std::string>() == "link<france$$germany>")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.link");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 1);
            BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(misc_gen_components)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    std::vector<std::string> miscGenNames
        = {"CHP", "BioMass", "BioGaz", "Waste", "GeoThermal", "Other", "PSP", "RowBalance"};

    for (const auto& name : miscGenNames)
    {
        std::string expectedId = "area<france>::MiscGen<" + name + ">";
        bool found = false;
        for (const auto& comp : components)
        {
            if (comp["id"].as<std::string>() == expectedId)
            {
                found = true;
                BOOST_CHECK_EQUAL(comp["model"].as<std::string>(),
                                  "antares_legacy_models.miscellaneous_generation");
                auto props = comp["properties"];
                BOOST_REQUIRE(props.IsSequence());
                BOOST_REQUIRE_EQUAL(props.size(), 3);
                BOOST_CHECK_EQUAL(props[0].as<std::string>(), "carrier");
                BOOST_CHECK_EQUAL(props[1].as<std::string>(), "technology");
                BOOST_CHECK_EQUAL(props[2].as<std::string>(), "miscellaneous_type");
            }
        }
        BOOST_CHECK_MESSAGE(found, "MiscGen component not found: " + expectedId);
    }
}

BOOST_AUTO_TEST_CASE(parameters_are_empty)
{
    YAML::Node root = studyToSystemYaml(*study);
    auto components = root["system"]["components"];

    for (const auto& comp : components)
    {
        auto params = comp["parameters"];
        BOOST_REQUIRE(params.IsSequence());
        BOOST_CHECK_EQUAL(params.size(), 0);
    }
}

BOOST_AUTO_TEST_CASE(round_trip_yaml_parse)
{
    YAML::Node root = studyToSystemYaml(*study);
    std::string yamlStr = YAML::Dump(root);

    YAML::Node reparsed = YAML::Load(yamlStr);
    BOOST_REQUIRE(reparsed.IsMap());
    BOOST_REQUIRE(reparsed["system"].IsDefined());

    auto sys = reparsed["system"];
    BOOST_CHECK_EQUAL(sys["id"].as<std::string>(), "legacy_converted");
    BOOST_REQUIRE(sys["model-libraries"].IsSequence());
    BOOST_REQUIRE(sys["components"].IsSequence());
    BOOST_CHECK_EQUAL(sys["components"].size(), 32);
}

BOOST_AUTO_TEST_SUITE_END()
