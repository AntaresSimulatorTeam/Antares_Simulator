//
// Created by marechaljas on 13/03/23.
//
#define BOOST_TEST_MODULE constraints
#include <boost/test/unit_test.hpp>
#include "antares/study/constraint.h"
#include "antares/study/area/area.h"
#include "antares/study/study.h"

using namespace Antares::Data;
BOOST_AUTO_TEST_CASE( load_basic_attributes ) {
    Study study;

    StudyLoadOptions options;
    BindConstList bindingConstraints;
    const bool loading_ok = bindingConstraints.loadFromFolder(study, options, ".");

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