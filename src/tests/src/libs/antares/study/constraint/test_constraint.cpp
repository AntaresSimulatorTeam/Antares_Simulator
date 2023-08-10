//
// Created by marechaljas on 13/03/23.
//

#define WIN32_LEAN_AND_MEAN
#define BOOST_TEST_MODULE binding_constraints

// Why this header ? See here
// https://www.boost.org/doc/libs/1_82_0/libs/test/doc/html/boost_test/adv_scenarios/single_header_customizations/multiple_translation_units.html
#include <boost/test/unit_test.hpp>

#include <fstream>
#include "antares/study/constraint.h"
#include "antares/study/area/area.h"
#include "antares/study.h"
#include <filesystem>
#include "utils.h"

using namespace Antares::Data;
namespace fs = std::filesystem;

BOOST_AUTO_TEST_SUITE(BindingConstraintTests)

BOOST_AUTO_TEST_CASE( load_basic_attributes ) {
    auto study = std::make_shared<Study>();

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

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
    std::ofstream rhs(working_tmp_dir / "dummy_id_eq.txt");
    rhs.close();

    study->header.version = version870;
    const bool loading_ok = bindingConstraints.loadFromFolder(*study, options, working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 1);

    auto constraint = *bindingConstraints.begin();
    BOOST_CHECK_EQUAL(constraint->name(), "dummy_name");
    BOOST_CHECK_EQUAL(constraint->id(), "dummy_id");
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->type(), BindingConstraint::Type::typeHourly);
    BOOST_CHECK_EQUAL(constraint->operatorType(), BindingConstraint::Operator::opEquality);
    BOOST_CHECK_EQUAL(constraint->yearByYearFilter(), FilterFlag::filterAnnual);
    BOOST_CHECK_EQUAL(constraint->synthesisFilter(), FilterFlag::filterHourly);
    BOOST_CHECK_EQUAL(constraint->comments(), "dummy_comment");
    BOOST_CHECK_EQUAL(constraint->group(), "dummy_group");
}

BOOST_AUTO_TEST_CASE(BC_load_RHS) {
    auto study = std::make_shared<Study>();
    study->areaAdd("area1");
    study->areaAdd("area2");
    study->areaAdd("area3");

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

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
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n"
            ;
    constraints.close();
    std::ofstream rhs(working_tmp_dir / "dummy_id_eq.txt");
    for (int i = 0; i < 8784; ++i) {
        rhs << "0.2\t0.4\t0.6\n";
    }
    rhs.close();
    study->header.version = version870;
    const bool loading_ok = bindingConstraints.loadFromFolder(*study, options, working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 1);

    auto constraint = (*bindingConstraints.begin());
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[1][30], 0.4, 0.0001);
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[2][8783], 0.6, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_load_range_type) {
    auto study = std::make_shared<Study>();
    study->areaAdd("area1");
    study->areaAdd("area2");
    study->areaAdd("area3");

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                <<"id = dummy_id\n"
                << "enabled = false\n"
                << "type = hourly\n"
                << "operator = both\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "group = dummy_group\n"
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n"
            ;
    constraints.close();
    std::ofstream lt(working_tmp_dir / "dummy_id_lt.txt");
    for (int i = 0; i < 8784; ++i) {
        lt << "0.2\t0.4\t0.6\n";
    }
    lt.close();
    std::ofstream gt(working_tmp_dir / "dummy_id_gt.txt");
    for (int i = 0; i < 8784; ++i) {
        gt << "0.4\t0.6\t0.8\n";
    }
    gt.close();
    study->header.version = version870;
    const bool loading_ok = bindingConstraints.loadFromFolder(*study, options, working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 2);

    auto bc_lt = std::find_if(bindingConstraints.begin(), bindingConstraints.end(), [](auto bc){
            return bc->operatorType() == BindingConstraint::opLess;
        });
    auto bc_gt = std::find_if(bindingConstraints.begin(), bindingConstraints.end(), [](auto bc){
            return bc->operatorType() == BindingConstraint::opGreater;
        });


    BOOST_CHECK(bc_lt != bindingConstraints.end());
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[1][30], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[2][8783], 0.6, 0.0001);

    BOOST_CHECK(bc_gt != bindingConstraints.end());
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][0], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[1][30], 0.6, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[2][8783], 0.8, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_load_legacy) {
    auto study = std::make_shared<Study>();
    study->areaAdd("area1");
    study->areaAdd("area2");
    study->areaAdd("area3");

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                <<"id = dummy_id\n"
                << "enabled = false\n"
                << "type = hourly\n"
                << "operator = less\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n"
            ;
    constraints.close();
    std::ofstream lt(working_tmp_dir / "dummy_id.txt");
    for (int i = 0; i < 8784; ++i) {
        lt << "0.2\t0.4\t0.6\n";
    }
    lt.close();

    study->header.version = version860;
    const bool loading_ok = bindingConstraints.loadFromFolder(*study, options, working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 1);

    auto bc_lt = (*bindingConstraints.begin());
    BOOST_CHECK_CLOSE(bc_lt->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE(bc_lt->RHSTimeSeries()[0][30], 0.2, 0.0001);
    BOOST_CHECK_CLOSE(bc_lt->RHSTimeSeries()[0][8783], 0.2, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_load_legacy_range) {
    auto study = std::make_shared<Study>();
    study->areaAdd("area1");
    study->areaAdd("area2");
    study->areaAdd("area3");

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                <<"id = dummy_id\n"
                << "enabled = false\n"
                << "type = hourly\n"
                << "operator = both\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n"
            ;
    constraints.close();
    std::ofstream lt(working_tmp_dir / "dummy_id.txt");
    for (int i = 0; i < 8784; ++i) {
        lt << "0.2\t0.4\t0.6\n";
    }
    lt.close();

    study->header.version = version860;
    const bool loading_ok = bindingConstraints.loadFromFolder(*study, options, working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 2);

    auto bc_lt = std::find_if(bindingConstraints.begin(), bindingConstraints.end(), [](auto bc){
                                  return bc->operatorType() == BindingConstraint::opLess;
                              });
    auto bc_gt = std::find_if(bindingConstraints.begin(), bindingConstraints.end(), [](auto bc){
                                  return bc->operatorType() == BindingConstraint::opGreater;
                              });

    BOOST_CHECK(bc_lt != bindingConstraints.end());
    BOOST_CHECK(bc_gt != bindingConstraints.end());

    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][30], 0.2, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][8783], 0.2, 0.0001);

    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][0], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][30], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][8783], 0.4, 0.0001);
}

BOOST_AUTO_TEST_SUITE_END()
