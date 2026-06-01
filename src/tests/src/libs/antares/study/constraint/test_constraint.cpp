// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 13/03/23.
//

#define WIN32_LEAN_AND_MEAN
#define BOOST_TEST_MODULE binding_constraints

#include <files-system.h>
#include <filesystem>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/study/study.h>
#include "antares/study/area/area.h"
#include "antares/study/constraint.h"

using namespace Antares::Data;
namespace fs = std::filesystem;

BOOST_AUTO_TEST_SUITE(BindingConstraintTests)

BOOST_AUTO_TEST_CASE(load_basic_attributes)
{
    auto study = std::make_shared<Study>();

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                << "id = dummy_id\n"
                << "enabled = true\n"
                << "type = hourly\n"
                << "operator = equal\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "group = dummy_group\n";
    constraints.close();
    std::ofstream rhs(working_tmp_dir / "dummy_id_eq.txt");
    rhs.close();

    study->header.version = StudyVersion(8, 7);
    const bool loading_ok = bindingConstraints.loadFromFolder(*study,
                                                              options,
                                                              working_tmp_dir.string());

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

BOOST_AUTO_TEST_CASE(BC_load_RHS)
{
    auto study = std::make_shared<Study>();

    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");

    AreaAddLinkBetweenAreas(area1, area2);
    AreaAddLinkBetweenAreas(area2, area3);
    AreaAddLinkBetweenAreas(area3, area1);

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                << "id = dummy_id\n"
                << "enabled = true\n"
                << "type = hourly\n"
                << "operator = equal\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "group = dummy_group\n"
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n";
    constraints.close();
    std::ofstream rhs(working_tmp_dir / "dummy_id_eq.txt");
    for (int i = 0; i < 8784; ++i)
    {
        rhs << "0.2\t0.4\t0.6\n";
    }
    rhs.close();
    study->header.version = StudyVersion(8, 7);
    const bool loading_ok = bindingConstraints.loadFromFolder(*study,
                                                              options,
                                                              working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 1);

    auto constraint = (*bindingConstraints.begin());
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[1][30], 0.4, 0.0001);
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[2][8783], 0.6, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_load_range_type)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");

    AreaAddLinkBetweenAreas(area1, area2);
    AreaAddLinkBetweenAreas(area2, area3);
    AreaAddLinkBetweenAreas(area3, area1);

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                << "id = dummy_id\n"
                << "enabled = true\n"
                << "type = hourly\n"
                << "operator = both\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "group = dummy_group\n"
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n";
    constraints.close();
    std::ofstream lt(working_tmp_dir / "dummy_id_lt.txt");
    for (int i = 0; i < 8784; ++i)
    {
        lt << "0.2\t0.4\t0.6\n";
    }
    lt.close();
    std::ofstream gt(working_tmp_dir / "dummy_id_gt.txt");
    for (int i = 0; i < 8784; ++i)
    {
        gt << "0.4\t0.6\t0.8\n";
    }
    gt.close();
    study->header.version = StudyVersion(8, 7);
    const bool loading_ok = bindingConstraints.loadFromFolder(*study,
                                                              options,
                                                              working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 2);

    auto bc_lt = std::find_if(bindingConstraints.begin(),
                              bindingConstraints.end(),
                              [](auto bc)
                              { return bc->operatorType() == BindingConstraint::opLess; });
    auto bc_gt = std::find_if(bindingConstraints.begin(),
                              bindingConstraints.end(),
                              [](auto bc)
                              { return bc->operatorType() == BindingConstraint::opGreater; });

    BOOST_CHECK(bc_lt != bindingConstraints.end());
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[1][30], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[2][8783], 0.6, 0.0001);

    BOOST_CHECK(bc_gt != bindingConstraints.end());
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][0], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[1][30], 0.6, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[2][8783], 0.8, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_load_legacy)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");

    AreaAddLinkBetweenAreas(area1, area2);
    AreaAddLinkBetweenAreas(area2, area3);
    AreaAddLinkBetweenAreas(area3, area1);
    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                << "id = dummy_id\n"
                << "enabled = true\n"
                << "type = hourly\n"
                << "operator = less\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n";
    constraints.close();

    std::ofstream lt(working_tmp_dir / "dummy_id.txt");
    for (int i = 0; i < 8784; ++i)
    {
        lt << "0.2\t0.4\t0.6\n";
    }
    lt.close();

    study->header.version = StudyVersion(8, 6);
    const bool loading_ok = bindingConstraints.loadFromFolder(*study,
                                                              options,
                                                              working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 1);

    auto bc_lt = (*bindingConstraints.begin());
    BOOST_CHECK_CLOSE(bc_lt->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE(bc_lt->RHSTimeSeries()[0][30], 0.2, 0.0001);
    BOOST_CHECK_CLOSE(bc_lt->RHSTimeSeries()[0][8783], 0.2, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_load_legacy_range)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");

    AreaAddLinkBetweenAreas(area1, area2);
    AreaAddLinkBetweenAreas(area2, area3);
    AreaAddLinkBetweenAreas(area3, area1);

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                << "id = dummy_id\n"
                << "enabled = true\n"
                << "type = hourly\n"
                << "operator = both\n"
                << "filter-year-by-year = annual\n"
                << "filter-synthesis = hourly\n"
                << "comments = dummy_comment\n"
                << "area1%area2 = 1.000000\n"
                << "area2%area3 = -1.000000\n"
                << "area3%area1 = 2.000000\n";
    constraints.close();
    std::ofstream lt(working_tmp_dir / "dummy_id.txt");
    for (int i = 0; i < 8784; ++i)
    {
        lt << "0.2\t0.4\t0.6\n";
    }
    lt.close();

    study->header.version = StudyVersion(8, 6);
    const bool loading_ok = bindingConstraints.loadFromFolder(*study,
                                                              options,
                                                              working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 2);

    auto bc_lt = std::find_if(bindingConstraints.begin(),
                              bindingConstraints.end(),
                              [](auto bc)
                              { return bc->operatorType() == BindingConstraint::opLess; });
    auto bc_gt = std::find_if(bindingConstraints.begin(),
                              bindingConstraints.end(),
                              [](auto bc)
                              { return bc->operatorType() == BindingConstraint::opGreater; });

    BOOST_CHECK(bc_lt != bindingConstraints.end());
    BOOST_CHECK(bc_gt != bindingConstraints.end());

    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][0], 0.2, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][30], 0.2, 0.0001);
    BOOST_CHECK_CLOSE((*bc_lt)->RHSTimeSeries()[0][8783], 0.2, 0.0001);

    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][0], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][30], 0.4, 0.0001);
    BOOST_CHECK_CLOSE((*bc_gt)->RHSTimeSeries()[0][8783], 0.4, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_disabled_returns_constraint_without_timeseries)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                << "id = dummy_id\n"
                << "enabled = false\n"
                << "type = hourly\n"
                << "operator = equal\n"
                << "group = dummy_group\n"
                << "area1%area2 = 1.000000\n";
    constraints.close();
    std::ofstream rhs(working_tmp_dir / "dummy_id_eq.txt");
    for (int i = 0; i < 8784; ++i)
    {
        rhs << "0.2\t0.4\t0.6\n";
    }
    rhs.close();

    study->header.version = StudyVersion(8, 7);
    const bool loading_ok = bindingConstraints.loadFromFolder(*study,
                                                              options,
                                                              working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);

    auto constraint = *bindingConstraints.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->RHSTimeSeries().width, 0);
    BOOST_CHECK_EQUAL(constraint->RHSTimeSeries().height, 0);
}

BOOST_AUTO_TEST_CASE(BC_disabled_both_operator_returns_single_constraint)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);

    StudyLoadOptions options;
    BindingConstraintsRepository bindingConstraints;

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream constraints(working_tmp_dir / "bindingconstraints.ini");
    constraints << "[1]\n"
                << "name = dummy_name\n"
                << "id = dummy_id\n"
                << "enabled = false\n"
                << "type = hourly\n"
                << "operator = both\n"
                << "group = dummy_group\n"
                << "area1%area2 = 1.000000\n";
    constraints.close();
    std::ofstream lt(working_tmp_dir / "dummy_id_lt.txt");
    lt.close();
    std::ofstream gt(working_tmp_dir / "dummy_id_gt.txt");
    gt.close();

    study->header.version = StudyVersion(8, 7);
    const bool loading_ok = bindingConstraints.loadFromFolder(*study,
                                                              options,
                                                              working_tmp_dir.string());

    BOOST_CHECK_EQUAL(loading_ok, true);
    // With enabled=false, the constraint is returned without splitting into lt/gt
    BOOST_CHECK_EQUAL(bindingConstraints.size(), 1);

    auto constraint = *bindingConstraints.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
}

// Write a minimal bindingconstraints.ini with the given body and load it.
// The caller is responsible for creating any RHS time series file.
static void loadConstraints(BindingConstraintsRepository& bindingConstraints,
                            const fs::path& dir,
                            Study& study,
                            const std::string& iniBody,
                            StudyVersion version = StudyVersion(8, 7))
{
    std::ofstream constraints(dir / "bindingconstraints.ini");
    constraints << iniBody;
    constraints.close();

    StudyLoadOptions options;
    study.header.version = version;
    const bool loading_ok = bindingConstraints.loadFromFolder(study, options, dir.string());
    // loadFromFolder always succeeds when the .ini file exists; per-constraint
    // failures are reflected by the constraint being absent from the repository.
    BOOST_CHECK_EQUAL(loading_ok, true);
}

BOOST_AUTO_TEST_CASE(BC_invalid_name_is_rejected)
{
    auto study = std::make_shared<Study>();
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = \n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n");

    BOOST_CHECK_EQUAL(bcs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BC_missing_id_is_rejected)
{
    auto study = std::make_shared<Study>();
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n");

    BOOST_CHECK_EQUAL(bcs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BC_invalid_type_is_rejected)
{
    auto study = std::make_shared<Study>();
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = not_a_type\n"
                    "operator = equal\n");

    BOOST_CHECK_EQUAL(bcs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BC_invalid_operator_is_rejected)
{
    auto study = std::make_shared<Study>();
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = not_an_operator\n");

    BOOST_CHECK_EQUAL(bcs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BC_enabled_without_any_weight_becomes_disabled)
{
    auto study = std::make_shared<Study>();
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "group = dummy_group\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->RHSTimeSeries().width, 0);
}

BOOST_AUTO_TEST_CASE(BC_link_not_found_weight_is_ignored)
{
    auto study = std::make_shared<Study>();
    addAreaToListOfAreas(study->areas, "area1");
    addAreaToListOfAreas(study->areas, "area2");
    // No link is created between area1 and area2 on purpose.
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1%area2 = 1.000000\n");

    // The weight references an unknown link, so it is dropped. With no weight
    // left the constraint is disabled and returned without time series.
    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->linkCount(), 0);
}

BOOST_AUTO_TEST_CASE(BC_cluster_weight_is_loaded)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto cluster = std::make_shared<ThermalCluster>(area1);
    cluster->setName("cluster1");
    area1->thermal.list.addToCompleteList(cluster);

    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream rhs(dir / "dummy_id_eq.txt");
    rhs.close();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1.cluster1 = 2.500000\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->clusterCount(), 1);
    BOOST_CHECK_CLOSE(constraint->weight(cluster.get()), 2.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_cluster_not_found_weight_is_ignored)
{
    auto study = std::make_shared<Study>();
    addAreaToListOfAreas(study->areas, "area1");
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1.unknown_cluster = 2.000000\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->clusterCount(), 0);
}

BOOST_AUTO_TEST_CASE(BC_invalid_key_is_ignored)
{
    auto study = std::make_shared<Study>();
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    // "notakey" is neither a link (no '%') nor a cluster (no '.').
    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "notakey = 1.000000\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
}

BOOST_AUTO_TEST_CASE(BC_invalid_weight_value_is_ignored)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1%area2 = not_a_number\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->linkCount(), 0);
}

BOOST_AUTO_TEST_CASE(BC_weight_and_offset_are_parsed)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto link = AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream rhs(dir / "dummy_id_eq.txt");
    rhs.close();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1%area2 = 2.000000%3\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_CLOSE(constraint->weight(link), 2.0, 0.0001);
    BOOST_CHECK_EQUAL(constraint->offset(link), 3);
}

BOOST_AUTO_TEST_CASE(BC_null_weight_leading_percent_is_rejected)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto link = AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    // A value starting with '%' exercises the "weight is null" (setVal == 0)
    // branch of SeparateValue. Yuni's words() yields an empty first token, so
    // the offset fails to parse and the whole entry is rejected.
    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1%area2 = %5\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->offset(link), 0);
}

BOOST_AUTO_TEST_CASE(BC_invalid_offset_value_is_ignored)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1%area2 = 2.000000%not_an_int\n");

    // Invalid offset rejects the whole entry, leaving the constraint weightless.
    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->linkCount(), 0);
}

BOOST_AUTO_TEST_CASE(BC_load_legacy_greater)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream values(dir / "dummy_id.txt");
    for (int i = 0; i < 8784; ++i)
    {
        values << "0.2\t0.4\t0.6\n";
    }
    values.close();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = greater\n"
                    "area1%area2 = 1.000000\n",
                    StudyVersion(8, 6));

    BOOST_REQUIRE_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    // "greater" reads the superior column (index 1) of the legacy matrix.
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[0][0], 0.4, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_load_legacy_equality_without_comments)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream values(dir / "dummy_id.txt");
    for (int i = 0; i < 8784; ++i)
    {
        values << "0.2\t0.4\t0.6\n";
    }
    values.close();

    // No "comments" key: exercises the empty-comments logging branch.
    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1%area2 = 1.000000\n",
                    StudyVersion(8, 6));

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    // "equal" reads the equality column (index 2) of the legacy matrix.
    BOOST_CHECK_CLOSE(constraint->RHSTimeSeries()[0][0], 0.6, 0.0001);
}

BOOST_AUTO_TEST_CASE(BC_missing_rhs_file_fails_to_load)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    // No dummy_id_eq.txt file is created on purpose.

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1%area2 = 1.000000\n");

    BOOST_CHECK_EQUAL(bcs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BC_both_operator_with_one_missing_rhs_fails_to_load)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream lt(dir / "dummy_id_lt.txt");
    lt.close();
    // dummy_id_gt.txt is missing on purpose.

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = both\n"
                    "area1%area2 = 1.000000\n");

    BOOST_CHECK_EQUAL(bcs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BC_legacy_missing_file_fails_to_load)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    AreaAddLinkBetweenAreas(area1, area2);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    // No dummy_id.txt legacy matrix is created on purpose.

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = less\n"
                    "area1%area2 = 1.000000\n",
                    StudyVersion(8, 6));

    BOOST_CHECK_EQUAL(bcs.size(), 0);
}

BOOST_AUTO_TEST_CASE(BC_cluster_invalid_value_is_ignored)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto cluster = std::make_shared<ThermalCluster>(area1);
    cluster->setName("cluster1");
    area1->thermal.list.addToCompleteList(cluster);
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1.cluster1 = not_a_number\n");

    BOOST_CHECK_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->clusterCount(), 0);
}

BOOST_AUTO_TEST_CASE(BC_cluster_weight_and_offset_are_parsed)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto cluster = std::make_shared<ThermalCluster>(area1);
    cluster->setName("cluster1");
    area1->thermal.list.addToCompleteList(cluster);

    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream rhs(dir / "dummy_id_eq.txt");
    rhs.close();

    BindingConstraintsRepository bcs;
    loadConstraints(bcs,
                    dir,
                    *study,
                    "[1]\n"
                    "name = dummy_name\n"
                    "id = dummy_id\n"
                    "enabled = true\n"
                    "type = hourly\n"
                    "operator = equal\n"
                    "area1.cluster1 = 4.000000%2\n");

    BOOST_REQUIRE_EQUAL(bcs.size(), 1);
    auto constraint = *bcs.begin();
    BOOST_CHECK_CLOSE(constraint->weight(cluster.get()), 4.0, 0.0001);
    BOOST_CHECK_EQUAL(constraint->offset(cluster.get()), 2);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_clusterCount)
{
    auto study = std::make_unique<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    BindingConstraint bc;
    // Add a thermal cluster to area1 and bc
    // return the number of clusters of bc
    const auto addClusterCountClusters = [&area, &bc](bool enabled, bool mustrun)
    {
        auto cluster = std::make_shared<ThermalCluster>(area);
        cluster->enabled = enabled;
        cluster->mustrun = mustrun;
        area->thermal.list.addToCompleteList(cluster);
        area->thermal.list.buildIndexes();
        bc.weight(cluster.get(), 5);
        return bc.clusterCount();
    };

    // enabled, not mustrun, should be counted
    BOOST_CHECK_EQUAL(addClusterCountClusters(true, false), 1);
    // enabled, mustrun should not be counted
    BOOST_CHECK_EQUAL(addClusterCountClusters(true, true), 1);
    // disabled, mustrun should not be counted
    BOOST_CHECK_EQUAL(addClusterCountClusters(false, true), 1);
    // disabled, not mustrun should not be counted
    BOOST_CHECK_EQUAL(addClusterCountClusters(false, false), 1);
}

struct BindingConstraintGroupRepositoryFixture
{
    BindingConstraintsRepository bcRepo;
    BindingConstraintGroupRepository groupRepo;

    void addConstraint(const std::string& name, const std::string& group, uint TSwidth)
    {
        auto bc = bcRepo.add(name);
        bc->group(group);
        bc->RHSTimeSeries().resize(/*width*/ TSwidth, /*height, arbitrary*/ 3);
    }
};

BOOST_FIXTURE_TEST_CASE(buildFrom_Empty, BindingConstraintGroupRepositoryFixture)
{
    BOOST_CHECK_EQUAL(bcRepo.size(), 0);
    BOOST_CHECK_EQUAL(groupRepo.size(), 0);
    BOOST_CHECK(groupRepo.buildFrom(bcRepo));
}

BOOST_FIXTURE_TEST_CASE(buildFrom_Single, BindingConstraintGroupRepositoryFixture)
{
    addConstraint("my-binding-constraint", "group1", 1);
    BOOST_CHECK(groupRepo.buildFrom(bcRepo));
}

BOOST_FIXTURE_TEST_CASE(buildFrom_TwoSameWidth, BindingConstraintGroupRepositoryFixture)
{
    addConstraint("my-binding-constraint", "group1", 10);
    addConstraint("other-binding-constraint", "group1", 10);
    BOOST_CHECK(groupRepo.buildFrom(bcRepo));
}

BOOST_FIXTURE_TEST_CASE(buildFrom_TwoBCDifferentWidth_One, BindingConstraintGroupRepositoryFixture)
{
    addConstraint("my-binding-constraint", "group1", 10);
    addConstraint("other-binding-constraint", "group1", 1);
    BOOST_CHECK(groupRepo.buildFrom(bcRepo));
}

BOOST_FIXTURE_TEST_CASE(buildFrom_TwoBCDifferentWidth, BindingConstraintGroupRepositoryFixture)
{
    addConstraint("my-binding-constraint", "group1", 10);
    addConstraint("other-binding-constraint", "group1", 20);
    BOOST_CHECK(!groupRepo.buildFrom(bcRepo));
}

BOOST_FIXTURE_TEST_CASE(buildFrom_ThreeBCDifferentWidth_One,
                        BindingConstraintGroupRepositoryFixture)
{
    addConstraint("my-binding-constraint", "group1", 10);
    addConstraint("other-bc", "group1", 1);
    addConstraint("another-bc", "group1", 10);
    BOOST_CHECK(groupRepo.buildFrom(bcRepo));
}

BOOST_FIXTURE_TEST_CASE(buildFrom_MultipleGroups, BindingConstraintGroupRepositoryFixture)
{
    addConstraint("my-binding-constraint", "group1", 10);
    addConstraint("other-bc", "group1", 10);
    addConstraint("another-bc", "group2", 20);
    BOOST_CHECK(groupRepo.buildFrom(bcRepo));
}

BOOST_FIXTURE_TEST_CASE(buildFrom_MultipleGroupsDifferentWidth,
                        BindingConstraintGroupRepositoryFixture)
{
    addConstraint("my-binding-constraint", "group1", 10);
    addConstraint("other-bc", "group1", 10);
    addConstraint("another-bc", "group2", 20);
    addConstraint("another-bc2", "group2", 25);
    BOOST_CHECK(!groupRepo.buildFrom(bcRepo));
}

BOOST_AUTO_TEST_SUITE_END()
