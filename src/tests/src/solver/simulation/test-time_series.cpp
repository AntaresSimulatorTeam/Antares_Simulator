//
// Created by marechaljas on 07/04/23.
//
#define BOOST_TEST_MODULE timeSeries

#include <boost/test/included/unit_test.hpp>
#include "antares/study.h"
#include <filesystem>
#include <fstream>

using namespace Antares::Solver;
using namespace Antares::Data;
namespace fs = std::filesystem;

void initializeStudy(Study& study)
{
    study.parameters.derated = false;

    study.runtime = new StudyRuntimeInfos(1);
    study.runtime->rangeLimits.year[rangeBegin] = 0;
    study.runtime->rangeLimits.year[rangeEnd] = 0;
    study.runtime->rangeLimits.year[rangeCount] = 1;

    study.parameters.renewableGeneration.toAggregated(); // Default

    study.parameters.intraModal = 0;
    study.parameters.interModal = 0;
    study.parameters.timeSeriesToRefresh = 0;
}

template<class Ta, class Tb>
void CheckEqual(const Matrix<Ta>& a, const Matrix<Tb>& b) {
    BOOST_CHECK_EQUAL(a.width, b.width);
    BOOST_CHECK_EQUAL(a.height, b.height);
    if (a.height > 0 && a.width > 0) {
        BOOST_CHECK_EQUAL(a[0][0], b[0][0]);
        BOOST_CHECK_EQUAL(a[a.width-1][a.height-1], b[b.width-1][b.height-1]);
    }
}

struct Fixture {
    Fixture() {

        auto tmp_dir = fs::temp_directory_path();
        working_tmp_dir = tmp_dir / std::tmpnam(nullptr);
        fs::create_directories(working_tmp_dir);

        std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
        constraints << "[1]\n"
                    << "name = dummy_name\n"
                    <<"id = dummy_id\n"
                    << "enabled = false\n"
                    << "type = hourly\n"
                    << "operator = equal\n"
                    << "filter-year-by-year = annual\n"
                    << "filter-synthesis = hourly\n"
                    << "comments = dummy_comment\n"
                    << "group = dummy_group\n"
                ;
        constraints.close();

        initializeStudy(study);

        expected_lower_bound_series.resize(3, 8784);
        expected_upper_bound_series.resize(3, 8784);
        expected_equality_series.resize(3, 8784);

        expected_lower_bound_series.fillColumn(0, 0.3);
        expected_lower_bound_series.fillColumn(1, 0.5);
        expected_lower_bound_series.fillColumn(2, 1);

        expected_upper_bound_series.fillColumn(0, 0.2);
        expected_upper_bound_series.fillColumn(1, 0.6);
        expected_upper_bound_series.fillColumn(2, 0);

        expected_equality_series.fillColumn(0, 0.1);
        expected_equality_series.fillColumn(1, 0.55);
        expected_equality_series.fillColumn(2, 0.9);
        expected_equality_series[0][8763] = 1;

        expected_lower_bound_series.saveToCSVFile((working_tmp_dir / "dummy_name_lt.txt").c_str());
        expected_upper_bound_series.saveToCSVFile((working_tmp_dir / "dummy_name_gt.txt").c_str());
        expected_equality_series.saveToCSVFile((working_tmp_dir / "dummy_name_eq.txt").c_str());
    };
    Study study;
    StudyLoadOptions options;
    std::filesystem::path working_tmp_dir;
    BindingConstraintsList bindingConstraints;
    Matrix<double> expected_lower_bound_series;
    Matrix<double> expected_upper_bound_series;
    Matrix<double> expected_equality_series;
};

BOOST_FIXTURE_TEST_SUITE(BC_TimeSeries, Fixture)

BOOST_AUTO_TEST_CASE(load_binding_constraints_timeseries) {
    bool loading_ok = bindingConstraints.loadFromFolder(study, options, working_tmp_dir.c_str());
    BOOST_CHECK_EQUAL(loading_ok, true);
    CheckEqual(bindingConstraints.find("dummy_id")->TimeSeries(), expected_equality_series);

    {
        std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
        constraints << "[1]\n"
                    << "name = dummy_name\n"
                    << "id = dummy_id\n"
                    << "enabled = false\n"
                    << "type = hourly\n"
                    << "operator = less\n"
                    << "group = dummy_group\n";
        constraints.close();
    }
    loading_ok = bindingConstraints.loadFromFolder(study, options, working_tmp_dir.c_str());
    BOOST_CHECK_EQUAL(loading_ok, true);
    CheckEqual(bindingConstraints.find("dummy_id")->TimeSeries(), expected_lower_bound_series);

    {
        std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
        constraints << "[1]\n"
                    << "name = dummy_name\n"
                    << "id = dummy_id\n"
                    << "enabled = false\n"
                    << "type = hourly\n"
                    << "operator = greater\n"
                    << "group = dummy_group\n";
        constraints.close();
    }
    loading_ok = bindingConstraints.loadFromFolder(study, options, working_tmp_dir.c_str());
    BOOST_CHECK_EQUAL(loading_ok, true);
    CheckEqual(bindingConstraints.find("dummy_id")->TimeSeries(), expected_upper_bound_series);
}

BOOST_AUTO_TEST_CASE(verify_all_constraints_in_a_group_have_the_same_number_of_time_series) {
    //TODO
}

//TODO
BOOST_AUTO_TEST_CASE(Check_empty_file_interpreted_as_all_zeroes) {
    std::vector file_names = {working_tmp_dir / "dummy_name_lt.txt",
                              working_tmp_dir / "dummy_name_gt.txt",
                              working_tmp_dir / "dummy_name_eq.txt"};
    for (auto file_name: file_names) {
        std::ofstream ofs;
        ofs.open(file_name, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
    }

    bool loading_ok = bindingConstraints.loadFromFolder(study, options, working_tmp_dir.c_str());
    BOOST_CHECK_EQUAL(loading_ok, true);
    auto expectation = Matrix(1, 8784);
    expectation.fill(0);
    CheckEqual(bindingConstraints.find("dummy_id")->TimeSeries(), expectation);
}

BOOST_AUTO_TEST_CASE(Check_missing_file) {
    std::vector file_names = {working_tmp_dir / "dummy_name_lt.txt",
                              working_tmp_dir / "dummy_name_gt.txt",
                              working_tmp_dir / "dummy_name_eq.txt"};
    for (auto file_name: file_names) {
        std::filesystem::remove(file_name);
    }

    bool loading_ok = bindingConstraints.loadFromFolder(study, options, working_tmp_dir.c_str());
    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()