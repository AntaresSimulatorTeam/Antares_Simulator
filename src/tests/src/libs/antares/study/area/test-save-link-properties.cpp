/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#define BOOST_TEST_MODULE test save link properties.ini

#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/study/filter.h>
#include <antares/study/study.h>

#include "files-helper.h"

using namespace std;
using namespace Antares::Data;
namespace fs = std::filesystem;

const string generatedIniFileName = "properties.ini";
const string referenceIniFileName = "properties-reference.ini";

class referenceIniFile
{
public:
    referenceIniFile();
    ~referenceIniFile() = default;

    string name() const
    {
        return name_;
    }

    void save();

    void set_property(const string& key, const string& value)
    {
        properties_[key] = value;
    }

private:
    string name_ = referenceIniFileName;

    // Keeps the properties ordered in the file
    vector<string> property_names_ = {"hurdles-cost",
                                      "loop-flow",
                                      "use-phase-shifter",
                                      "transmission-capacities",
                                      "asset-type",
                                      "link-style",
                                      "link-width",
                                      "colorr",
                                      "colorg",
                                      "colorb",
                                      "display-comments",
                                      "filter-synthesis",
                                      "filter-year-by-year"};

    map<string, string, less<>> properties_;
};

referenceIniFile::referenceIniFile()
{
    properties_[property_names_[0]] = "false";
    properties_[property_names_[1]] = "false";
    properties_[property_names_[2]] = "false";
    properties_[property_names_[3]] = "enabled";
    properties_[property_names_[4]] = "ac";
    properties_[property_names_[5]] = "plain";
    properties_[property_names_[6]] = "1";
    properties_[property_names_[7]] = "112";
    properties_[property_names_[8]] = "112";
    properties_[property_names_[9]] = "112";
    properties_[property_names_[10]] = "true";
    properties_[property_names_[11]] = "hourly, daily, weekly, monthly, annual";
    properties_[property_names_[12]] = "hourly, daily, weekly, monthly, annual";
}

void referenceIniFile::save()
{
    ofstream file;
    file.open(name_);
    file << "[area 2]" << endl;
    for (std::size_t i = 0; i < property_names_.size(); ++i)
    {
        file << property_names_[i] << " = " << properties_[property_names_[i]] << endl;
    }
    file << endl;
    file.close();
}

struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;

    Fixture():
        study(std::make_shared<Study>())
    {
        area_1 = study->areaAdd("Area 1");
        area_2 = study->areaAdd("Area 2");
    }

    ~Fixture()
    {
        vector<string> filesToRemove = {generatedIniFileName, referenceIniFileName};
        remove_files(filesToRemove);
    }

    Study::Ptr study;
    Area* area_1;
    Area* area_2;
};

AreaLink* createLinkBetweenAreas(Area* a1, Area* a2)
{
    AreaLink* link = AreaAddLinkBetweenAreas(a1, a2, false);
    return link;
}

void saveAreaLinksOntoDisk(Area* area)
{
    BOOST_CHECK(saveAreaLinksConfigurationFileToFolder(area, fs::current_path().string().c_str()));
}

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(one_link_with_default_values, Fixture)
{
    createLinkBetweenAreas(area_1, area_2);

    saveAreaLinksOntoDisk(area_1);

    referenceIniFile referenceFile;
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(one_link_with_none_default_values, Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->useHurdlesCost = true;
    link->useLoopFlow = true;
    link->usePST = true;
    link->transmissionCapacities = LocalTransmissionCapacities::infinite;
    link->assetType = atDC;
    link->style = stDash;
    link->linkWidth = 3;
    link->color[0] = 10;
    link->color[1] = 9;
    link->color[2] = 8;
    link->displayComments = false;
    link->filterSynthesis = filterNone;
    link->filterYearByYear = filterHourly | filterDaily;

    saveAreaLinksOntoDisk(area_1);

    referenceIniFile referenceFile;
    referenceFile.set_property("hurdles-cost", "true");
    referenceFile.set_property("loop-flow", "true");
    referenceFile.set_property("use-phase-shifter", "true");
    referenceFile.set_property("transmission-capacities", "infinite");
    referenceFile.set_property("asset-type", "dc");
    referenceFile.set_property("link-style", "dash");
    referenceFile.set_property("link-width", "3");
    referenceFile.set_property("colorr", "10");
    referenceFile.set_property("colorg", "9");
    referenceFile.set_property("colorb", "8");
    referenceFile.set_property("display-comments", "false");
    referenceFile.set_property("filter-synthesis", "");
    referenceFile.set_property("filter-year-by-year", "hourly, daily");
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(
  one_link_with_transmission_capacity_to_ignore__all_others_properties_are_default,
  Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->transmissionCapacities = LocalTransmissionCapacities::null;

    saveAreaLinksOntoDisk(area_1);

    referenceIniFile referenceFile;
    referenceFile.set_property("transmission-capacities", "ignore");
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(one_link_with_asset_type_to_gas__ini_file_contains_matching_line, Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->assetType = atGas;

    saveAreaLinksOntoDisk(area_1);

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "asset-type = gaz"));
}

BOOST_FIXTURE_TEST_CASE(one_link_with_asset_type_to_virtual__ini_file_contains_matching_line,
                        Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->assetType = atVirt;

    saveAreaLinksOntoDisk(area_1);

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "asset-type = virt"));
}

BOOST_FIXTURE_TEST_CASE(one_link_with_asset_type_to_other__ini_file_contains_matching_line, Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->assetType = atOther;

    saveAreaLinksOntoDisk(area_1);

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "asset-type = other"));
}

BOOST_FIXTURE_TEST_CASE(one_link_with_style_to_dot__ini_file_contains_matching_line, Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->style = stDot;

    saveAreaLinksOntoDisk(area_1);

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "link-style = dot"));
}

BOOST_FIXTURE_TEST_CASE(one_link_with_style_to_dotdash__ini_file_contains_matching_line, Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->style = stDotDash;

    saveAreaLinksOntoDisk(area_1);

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "link-style = dotdash"));
}

BOOST_FIXTURE_TEST_CASE(
  one_link_with_synthesis_to_hourly_monthly_annual__ini_file_contains_matching_line,
  Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->filterSynthesis = filterWeekly | filterMonthly | filterAnnual;

    saveAreaLinksOntoDisk(area_1);

    BOOST_CHECK(
      fileContainsLine(generatedIniFileName, "filter-synthesis = weekly, monthly, annual"));
}

BOOST_FIXTURE_TEST_CASE(
  one_link_with_year_by_year_to_daily_monthly__ini_file_contains_matching_line,
  Fixture)
{
    AreaLink* link = createLinkBetweenAreas(area_1, area_2);
    link->filterYearByYear = filterDaily | filterMonthly;

    saveAreaLinksOntoDisk(area_1);

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "filter-year-by-year = daily, monthly"));
}

BOOST_AUTO_TEST_SUITE_END()
