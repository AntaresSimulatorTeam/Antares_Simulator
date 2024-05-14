/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE test save area optimization.ini

#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>
#include <string>

#include <boost/test/included/unit_test.hpp>

#include <antares/study/area/constants.h>
#include <antares/study/filter.h>
#include <antares/study/study.h>

#include "files-helper.h"

using namespace Antares::Data;
namespace fs = std::filesystem;

const std::string generatedIniFileName = "optimization.ini";
const std::string referenceIniFileName = "optimization-reference.ini";

struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;

    Fixture():
        area(study->areaAdd("Area"))
    {
        path_to_generated_file << fs::current_path().append(generatedIniFileName).string();
    }

    ~Fixture()
    {
        std::vector<std::string> filesToRemove = {generatedIniFileName, referenceIniFileName};
        remove_files(filesToRemove);
    }

    Study::Ptr study = std::make_shared<Study>();
    Area* area;
    Yuni::Clob path_to_generated_file;
};

class referenceIniFile
{
public:
    referenceIniFile();
    ~referenceIniFile() = default;

    std::string name() const
    {
        return name_;
    }

    void save();

    void set_property(const std::string& key, const std::string& value)
    {
        properties_[key] = value;
    }

private:
    void save_section(std::string_view sectionTitle,
                      std::vector<std::string>& section_properties,
                      std::ofstream& file);

    std::string name_ = referenceIniFileName;
    std::vector<std::string> nodal_property_names_ = {"non-dispatchable-power",
                                                      "dispatchable-hydro-power",
                                                      "other-dispatchable-power",
                                                      "spread-unsupplied-energy-cost",
                                                      "spread-spilled-energy-cost"};
    std::vector<std::string> filtering_property_names_ = {"filter-synthesis",
                                                          "filter-year-by-year"};
    std::map<std::string, std::string, std::less<>> properties_;
};

referenceIniFile::referenceIniFile()
{
    properties_[nodal_property_names_[0]] = "true";
    properties_[nodal_property_names_[1]] = "true";
    properties_[nodal_property_names_[2]] = "true";
    properties_[nodal_property_names_[3]] = "0.000000";
    properties_[nodal_property_names_[4]] = "0.000000";
    properties_[filtering_property_names_[0]] = "hourly, daily, weekly, monthly, annual";
    properties_[filtering_property_names_[1]] = "hourly, daily, weekly, monthly, annual";
}

void referenceIniFile::save()
{
    std::ofstream file;
    file.open(name_);
    save_section("[nodal optimization]", nodal_property_names_, file);
    save_section("[filtering]", filtering_property_names_, file);
    file.close();
}

void referenceIniFile::save_section(std::string_view sectionTitle,
                                    std::vector<std::string>& sectionProperties,
                                    std::ofstream& file)
{
    file << sectionTitle << std::endl;
    for (std::size_t i = 0; i < sectionProperties.size(); ++i)
    {
        file << sectionProperties[i] << " = " << properties_[sectionProperties[i]] << std::endl;
    }
    file << std::endl;
}

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(one_area_with_default_params, Fixture)
{
    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    referenceIniFile referenceFile;
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_none_default_params, Fixture)
{
    area->nodalOptimization = 0;
    area->spreadUnsuppliedEnergyCost = 2.;
    area->spreadSpilledEnergyCost = 3.;
    area->filterSynthesis = filterNone;
    area->filterYearByYear = filterHourly | filterDaily;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    referenceIniFile referenceFile;
    referenceFile.set_property("non-dispatchable-power", "false");
    referenceFile.set_property("dispatchable-hydro-power", "false");
    referenceFile.set_property("other-dispatchable-power", "false");
    referenceFile.set_property("spread-unsupplied-energy-cost", "2.000000");
    referenceFile.set_property("spread-spilled-energy-cost", "3.000000");
    referenceFile.set_property("filter-synthesis", "");
    referenceFile.set_property("filter-year-by-year", "hourly, daily");
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_nodal_opt_to_nonDispatchPower__other_params_to_default,
                        Fixture)
{
    area->nodalOptimization = anoNonDispatchPower;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    referenceIniFile referenceFile;
    referenceFile.set_property("non-dispatchable-power", "true");
    referenceFile.set_property("dispatchable-hydro-power", "false");
    referenceFile.set_property("other-dispatchable-power", "false");
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_nodal_opt_to_dispatchHydroPower__other_params_to_default,
                        Fixture)
{
    area->nodalOptimization = anoDispatchHydroPower;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    referenceIniFile referenceFile;
    referenceFile.set_property("non-dispatchable-power", "false");
    referenceFile.set_property("dispatchable-hydro-power", "true");
    referenceFile.set_property("other-dispatchable-power", "false");
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_nodal_opt_to_otherDispatchablePower__other_params_to_default,
                        Fixture)
{
    area->nodalOptimization = anoOtherDispatchPower;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    referenceIniFile referenceFile;
    referenceFile.set_property("non-dispatchable-power", "false");
    referenceFile.set_property("dispatchable-hydro-power", "false");
    referenceFile.set_property("other-dispatchable-power", "true");
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_FIXTURE_TEST_CASE(
  one_area_with_nodal_opt_to_non_or_other_DispatchPower__other_params_to_default,
  Fixture)
{
    area->nodalOptimization = anoOtherDispatchPower | anoNonDispatchPower;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    referenceIniFile referenceFile;
    referenceFile.set_property("non-dispatchable-power", "true");
    referenceFile.set_property("dispatchable-hydro-power", "false");
    referenceFile.set_property("other-dispatchable-power", "true");
    referenceFile.save();

    BOOST_CHECK(files_identical(generatedIniFileName, referenceIniFileName));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_unsupplied_energy_cost_negative__other_params_to_default,
                        Fixture)
{
    area->spreadUnsuppliedEnergyCost = -1.;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    BOOST_CHECK(
      fileContainsLine(generatedIniFileName, "spread-unsupplied-energy-cost = -1.000000"));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_spilled_energy_cost_negative__other_params_to_default,
                        Fixture)
{
    area->spreadSpilledEnergyCost = -1.;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "spread-spilled-energy-cost = -1.000000"));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_synthesis_to_hourly_monthly_annual__other_params_to_default,
                        Fixture)
{
    area->filterSynthesis = filterWeekly | filterMonthly | filterAnnual;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    BOOST_CHECK(
      fileContainsLine(generatedIniFileName, "filter-synthesis = weekly, monthly, annual"));
}

BOOST_FIXTURE_TEST_CASE(one_area_with_year_by_year_to_daily_monthly__other_params_to_default,
                        Fixture)
{
    area->filterYearByYear = filterDaily | filterMonthly;

    BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

    BOOST_CHECK(fileContainsLine(generatedIniFileName, "filter-year-by-year = daily, monthly"));
}

BOOST_AUTO_TEST_SUITE_END()
