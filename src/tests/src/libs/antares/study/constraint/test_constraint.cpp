//
// Created by marechaljas on 13/03/23.
//
#define BOOST_TEST_MODULE constraints
#include <boost/test/unit_test.hpp>
#include <fstream>
#include "antares/study/constraint.h"
#include "antares/study/area/area.h"
#include "antares/study/study.h"
#include <filesystem>

using namespace Antares::Data;
namespace fs = std::filesystem;

BOOST_AUTO_TEST_CASE( load_basic_attributes ) {
    Study study;

    StudyLoadOptions options;
    BindConstList bindingConstraints;

    auto tmp_dir = fs::temp_directory_path();
    auto working_tmp_dir = tmp_dir / std::tmpnam(nullptr);
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
    std::ofstream rhs(working_tmp_dir / "dummy_id.txt");
    rhs.close();

    const bool loading_ok = bindingConstraints.loadFromFolder(study, options, working_tmp_dir.c_str());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 1);

    BindingConstraint* constraint = (*bindingConstraints.begin());
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