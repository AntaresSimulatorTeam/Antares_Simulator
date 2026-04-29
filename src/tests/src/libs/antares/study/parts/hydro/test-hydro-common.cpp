// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test hydro common

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <antares/study/study.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

// =================
// The fixture
// =================
struct CommonFixture
{
    std::filesystem::path path;
    StudyLoadOptions options;
    StudyVersion version = StudyVersion::latest();

    CommonFixture():
        tmp(fs::temp_directory_path()),
        hydroIni(tmp / "hydro.ini"),
        study(std::make_unique<Study>())
    {
        east = new Area("east"); // freed by ~AreaList
        study->areas.add(east);
        west = new Area("west"); // freed by ~AreaList
        study->areas.add(west);
    }

    bool load()
    {
        return east->hydro.LoadIniFile(*study, tmp) && west->hydro.LoadIniFile(*study, tmp);
    }

    ~CommonFixture()
    {
        fs::remove(hydroIni);
    }

    void writeValidFile();
    void writeInvalidFile();

public:
    Area* east;
    Area* west;

private:
    fs::path tmp;
    fs::path hydroIni;
    std::unique_ptr<Study> study;
};

void CommonFixture::writeValidFile()
{
    std::ofstream outfile(hydroIni);
    outfile <<
      R"([overflow spilled cost difference]
east = 1.00000
west = 2.31000

[reservoir]
east = true)";
}

void CommonFixture::writeInvalidFile()
{
    std::ofstream outfile(hydroIni);
    outfile <<
      R"([overflow spilled cost difference]
east = 1.00000
west = 2.31000
wrongarea = 1.414

[reservoir]
east = true)";
}

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(test_read_valid_file, CommonFixture)
{
    writeValidFile();
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.overflowSpilledCostDifference, 1.0);
    BOOST_CHECK_EQUAL(west->hydro.overflowSpilledCostDifference, 2.31000);
}

BOOST_FIXTURE_TEST_CASE(test_read_invalid_file, CommonFixture)
{
    writeInvalidFile();
    BOOST_CHECK(!load());
}
BOOST_AUTO_TEST_SUITE_END()
