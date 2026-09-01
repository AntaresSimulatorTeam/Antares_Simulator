// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 13/03/23.
//

#define WIN32_LEAN_AND_MEAN
#define BOOST_TEST_MODULE binding_constraints

#include <algorithm>
#include <files-system.h>
#include <filesystem>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/study/study.h>
#include "antares/study/area/area.h"
#include "antares/study/constraint.h"
#include "antares/utils/utils.h"

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

// ---------------------------------------------------------------------------
// The following tests exercise BindingConstraint's public API directly
// (no ini loading), targeting methods a coverage run showed as never
// executed by the rest of the test suite.
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(BindingConstraint_StringToOperator_aliases)
{
    using Operator = BindingConstraint::Operator;
    for (const std::string& s: {"both", "<>", "><", "< and >", "BOTH"})
    {
        BOOST_CHECK_EQUAL(BindingConstraint::StringToOperator(s), Operator::opBoth);
    }
    for (const std::string& s: {"less", "<", "<="})
    {
        BOOST_CHECK_EQUAL(BindingConstraint::StringToOperator(s), Operator::opLess);
    }
    for (const std::string& s: {"greater", ">", ">="})
    {
        BOOST_CHECK_EQUAL(BindingConstraint::StringToOperator(s), Operator::opGreater);
    }
    for (const std::string& s: {"equal", "=", "=="})
    {
        BOOST_CHECK_EQUAL(BindingConstraint::StringToOperator(s), Operator::opEquality);
    }
    BOOST_CHECK_EQUAL(BindingConstraint::StringToOperator("nonsense"), Operator::opUnknown);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_StringToType_aliases)
{
    using Type = BindingConstraint::Type;
    for (const std::string& s: {"hourly", "hour", "h"})
    {
        BOOST_CHECK_EQUAL(BindingConstraint::StringToType(s), Type::typeHourly);
    }
    for (const std::string& s: {"daily", "day", "d"})
    {
        BOOST_CHECK_EQUAL(BindingConstraint::StringToType(s), Type::typeDaily);
    }
    for (const std::string& s: {"weekly", "week", "w"})
    {
        BOOST_CHECK_EQUAL(BindingConstraint::StringToType(s), Type::typeWeekly);
    }
    BOOST_CHECK_EQUAL(BindingConstraint::StringToType(""), Type::typeUnknown);
    BOOST_CHECK_EQUAL(BindingConstraint::StringToType("nonsense"), Type::typeUnknown);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_TypeToCString_allValues)
{
    using Type = BindingConstraint::Type;
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::TypeToCString(Type::typeHourly)), "hourly");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::TypeToCString(Type::typeDaily)), "daily");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::TypeToCString(Type::typeWeekly)), "weekly");
}

BOOST_AUTO_TEST_CASE(BindingConstraint_OperatorToCString_allValues)
{
    using Operator = BindingConstraint::Operator;
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToCString(Operator::opEquality)),
                      "equal");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToCString(Operator::opLess)), "less");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToCString(Operator::opGreater)),
                      "greater");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToCString(Operator::opBoth)), "both");
}

BOOST_AUTO_TEST_CASE(BindingConstraint_OperatorToShortCString_allValues)
{
    using Operator = BindingConstraint::Operator;
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToShortCString(Operator::opEquality)),
                      "equality");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToShortCString(Operator::opLess)),
                      "bounded above");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToShortCString(Operator::opGreater)),
                      "bounded below");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::OperatorToShortCString(Operator::opBoth)),
                      "bounded on both sides");
}

BOOST_AUTO_TEST_CASE(BindingConstraint_MathOperatorToCString_allValues)
{
    using Operator = BindingConstraint::Operator;
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::MathOperatorToCString(Operator::opEquality)),
                      "=");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::MathOperatorToCString(Operator::opLess)), "<");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::MathOperatorToCString(Operator::opGreater)),
                      ">");
    BOOST_CHECK_EQUAL(std::string(BindingConstraint::MathOperatorToCString(Operator::opBoth)),
                      "< and >");
}

BOOST_AUTO_TEST_CASE(BindingConstraint_pId_slugifiesName)
{
    BindingConstraint bc;
    bc.pId("My Constraint Name!");
    BOOST_CHECK_EQUAL(bc.id(), Antares::transformNameIntoID("My Constraint Name!"));
}

BOOST_AUTO_TEST_CASE(BindingConstraint_resetToDefaultValues)
{
    BindingConstraint bc;
    bc.enabled(false);
    bc.comments("some comment");
    bc.RHSTimeSeries().resize(2, 5);

    bc.resetToDefaultValues();

    BOOST_CHECK_EQUAL(bc.enabled(), true);
    BOOST_CHECK(bc.comments().empty());
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().width, 0u);
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().height, 0u);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_clearAndReset_hourly)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto link = AreaAddLinkBetweenAreas(area1, area2);

    BindingConstraint bc;
    bc.weight(link, 3.0);
    bc.offset(link, 2);

    bc.clearAndReset("my constraint", BindingConstraint::typeHourly, BindingConstraint::opEquality);

    BOOST_CHECK_EQUAL(bc.name(), "my constraint");
    BOOST_CHECK_EQUAL(bc.id(), Antares::transformNameIntoID("my constraint"));
    BOOST_CHECK_EQUAL(bc.type(), BindingConstraint::typeHourly);
    BOOST_CHECK_EQUAL(bc.operatorType(), BindingConstraint::opEquality);
    BOOST_CHECK_EQUAL(bc.weight(link), 0.);
    BOOST_CHECK_EQUAL(bc.offset(link), 0);
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().width, (unsigned int)BindingConstraint::columnMax);
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().height, 8784u);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_clearAndReset_daily)
{
    BindingConstraint bc;
    bc.clearAndReset("c", BindingConstraint::typeDaily, BindingConstraint::opLess);
    BOOST_CHECK_EQUAL(bc.type(), BindingConstraint::typeDaily);
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().width, (unsigned int)BindingConstraint::columnMax);
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().height, 366u);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_clearAndReset_weekly)
{
    BindingConstraint bc;
    bc.clearAndReset("c", BindingConstraint::typeWeekly, BindingConstraint::opGreater);
    BOOST_CHECK_EQUAL(bc.type(), BindingConstraint::typeWeekly);
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().width, (unsigned int)BindingConstraint::columnMax);
    BOOST_CHECK_EQUAL(bc.RHSTimeSeries().height, 366u);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_iterators)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");
    auto link1 = AreaAddLinkBetweenAreas(area1, area2);
    auto link2 = AreaAddLinkBetweenAreas(area1, area3);

    BindingConstraint bc;
    bc.weight(link1, 1.5);
    bc.weight(link2, 2.5);

    std::map<const AreaLink*, double> visited;
    for (const auto& [link, weight]: bc)
    {
        visited[link] = weight;
    }

    BOOST_REQUIRE_EQUAL(visited.size(), 2);
    BOOST_CHECK_CLOSE(visited[link1], 1.5, 0.0001);
    BOOST_CHECK_CLOSE(visited[link2], 2.5, 0.0001);

    const BindingConstraint& constBc = bc;
    std::size_t constCount = 0;
    for (auto it = constBc.begin(); it != constBc.end(); ++it)
    {
        ++constCount;
    }
    BOOST_CHECK_EQUAL(constCount, 2u);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_skipped_isActive_enabled)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto link = AreaAddLinkBetweenAreas(area1, area2);

    BindingConstraint bc;
    bc.enabled(true);
    BOOST_CHECK_EQUAL(bc.enabled(), true);
    BOOST_CHECK_EQUAL(bc.skipped(), true);
    BOOST_CHECK_EQUAL(bc.isActive(), false);

    bc.weight(link, 1.0);
    BOOST_CHECK_EQUAL(bc.skipped(), false);
    BOOST_CHECK_EQUAL(bc.isActive(), true);

    bc.enabled(false);
    BOOST_CHECK_EQUAL(bc.enabled(), false);
    BOOST_CHECK_EQUAL(bc.isActive(), false);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_RHSTimeSeries_constOverload)
{
    BindingConstraint bc;
    bc.RHSTimeSeries().resize(2, 4);
    bc.RHSTimeSeries()[0][0] = 42.0;

    const BindingConstraint& constBc = bc;
    const Antares::Matrix<>& rhs = constBc.RHSTimeSeries();
    BOOST_CHECK_EQUAL(rhs.width, 2u);
    BOOST_CHECK_EQUAL(rhs.height, 4u);
    BOOST_CHECK_CLOSE(rhs[0][0], 42.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_removeAllWeights)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto link = AreaAddLinkBetweenAreas(area1, area2);
    auto cluster = std::make_shared<ThermalCluster>(area1);
    area1->thermal.list.addToCompleteList(cluster);
    area1->thermal.list.buildIndexes();

    BindingConstraint bc;
    bc.weight(link, 1.0);
    bc.offset(link, 3);
    bc.weight(cluster.get(), 2.0);

    bc.removeAllWeights();

    BOOST_CHECK_EQUAL(bc.linkCount(), 0u);
    BOOST_CHECK_EQUAL(bc.clusterCount(), 0u);
    // removeAllWeights() must not touch offsets
    BOOST_CHECK_EQUAL(bc.offset(link), 3);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_offset_setters_eraseOnZero)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto link = AreaAddLinkBetweenAreas(area1, area2);
    auto cluster = std::make_shared<ThermalCluster>(area1);
    area1->thermal.list.addToCompleteList(cluster);
    area1->thermal.list.buildIndexes();

    BindingConstraint bc;

    bc.offset(link, 5);
    BOOST_CHECK_EQUAL(bc.offset(link), 5);
    bc.offset(link, 0);
    BOOST_CHECK_EQUAL(bc.offset(link), 0);

    bc.offset(cluster.get(), 7);
    BOOST_CHECK_EQUAL(bc.offset(cluster.get()), 7);
    bc.offset(cluster.get(), 0);
    BOOST_CHECK_EQUAL(bc.offset(cluster.get()), 0);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_contains_bindingConstraint)
{
    BindingConstraint bc;
    BindingConstraint other;
    BOOST_CHECK_EQUAL(bc.contains(&bc), true);
    BOOST_CHECK_EQUAL(bc.contains(&other), false);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_contains_area)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");
    auto link = AreaAddLinkBetweenAreas(area1, area2);
    auto cluster = std::make_shared<ThermalCluster>(area3);
    area3->thermal.list.addToCompleteList(cluster);
    area3->thermal.list.buildIndexes();

    BindingConstraint bc;
    bc.weight(link, 1.0);
    bc.weight(cluster.get(), 1.0);

    BOOST_CHECK_EQUAL(bc.contains(area1), true);
    BOOST_CHECK_EQUAL(bc.contains(area2), true);
    BOOST_CHECK_EQUAL(bc.contains(area3), true);

    auto area4 = addAreaToListOfAreas(study->areas, "area4");
    BOOST_CHECK_EQUAL(bc.contains(area4), false);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_contains_link_and_cluster)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");
    auto link1 = AreaAddLinkBetweenAreas(area1, area2);
    auto link2 = AreaAddLinkBetweenAreas(area1, area3);
    auto cluster1 = std::make_shared<ThermalCluster>(area1);
    cluster1->setName("cluster1");
    area1->thermal.list.addToCompleteList(cluster1);
    auto cluster2 = std::make_shared<ThermalCluster>(area1);
    cluster2->setName("cluster2");
    area1->thermal.list.addToCompleteList(cluster2);
    area1->thermal.list.buildIndexes();

    BindingConstraint bc;
    bc.weight(link1, 1.0);
    bc.weight(cluster1.get(), 1.0);

    BOOST_CHECK_EQUAL(bc.contains(link1), true);
    BOOST_CHECK_EQUAL(bc.contains(link2), false);
    BOOST_CHECK_EQUAL(bc.contains(cluster1.get()), true);
    BOOST_CHECK_EQUAL(bc.contains(cluster2.get()), false);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_buildFormula_linksWithOffsets)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");
    auto linkPos = AreaAddLinkBetweenAreas(area1, area2);
    auto linkNeg = AreaAddLinkBetweenAreas(area1, area3);

    BindingConstraint bc;
    bc.weight(linkPos, 2.0);
    bc.offset(linkPos, 3);
    bc.weight(linkNeg, 4.0);
    bc.offset(linkNeg, -5);

    std::string formula;
    bc.buildFormula(formula);

    BOOST_CHECK(formula.find(linkPos->getName() + " x (t + 3)") != std::string::npos);
    BOOST_CHECK(formula.find(linkNeg->getName() + " x (t - 5)") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_buildFormula_clusterActiveInactive)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto activeCluster = std::make_shared<ThermalCluster>(area1);
    activeCluster->setName("active_cluster");
    activeCluster->enabled = true;
    activeCluster->mustrun = false;
    area1->thermal.list.addToCompleteList(activeCluster);

    auto inactiveCluster = std::make_shared<ThermalCluster>(area1);
    inactiveCluster->setName("inactive_cluster");
    inactiveCluster->enabled = true;
    inactiveCluster->mustrun = true;
    area1->thermal.list.addToCompleteList(inactiveCluster);
    area1->thermal.list.buildIndexes();

    BindingConstraint bc;
    bc.weight(activeCluster.get(), 1.0);
    bc.weight(inactiveCluster.get(), 2.0);

    std::string formula;
    bc.buildFormula(formula);

    const auto activePos = formula.find(activeCluster->getFullName());
    const auto inactivePos = formula.find(inactiveCluster->getFullName());
    BOOST_REQUIRE(activePos != std::string::npos);
    BOOST_REQUIRE(inactivePos != std::string::npos);

    const auto activeTerm = formula.substr(activePos, formula.find(')', activePos) - activePos);
    const auto inactiveTerm = formula.substr(inactivePos,
                                             formula.find(')', inactivePos) - inactivePos);

    BOOST_CHECK(activeTerm.find("N/A") == std::string::npos);
    BOOST_CHECK(inactiveTerm.find("N/A") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_initLinkArrays)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    auto area3 = addAreaToListOfAreas(study->areas, "area3");
    auto link1 = AreaAddLinkBetweenAreas(area1, area2);
    auto link2 = AreaAddLinkBetweenAreas(area1, area3);

    auto activeCluster = std::make_shared<ThermalCluster>(area1);
    activeCluster->setName("active_cluster");
    activeCluster->enabled = true;
    activeCluster->mustrun = false;
    area1->thermal.list.addToCompleteList(activeCluster);

    auto inactiveCluster = std::make_shared<ThermalCluster>(area1);
    inactiveCluster->setName("inactive_cluster");
    inactiveCluster->enabled = false;
    area1->thermal.list.addToCompleteList(inactiveCluster);
    area1->thermal.list.buildIndexes();

    BindingConstraint bc;
    bc.weight(link1, 1.0);
    bc.offset(link1, 2);
    bc.weight(link2, 3.0);
    bc.weight(activeCluster.get(), 4.0);
    bc.offset(activeCluster.get(), 5);
    bc.weight(inactiveCluster.get(), 6.0);

    const auto structures = bc.initLinkArrays();

    BOOST_REQUIRE_EQUAL(structures.linkWeight.size(), 2u);
    BOOST_REQUIRE_EQUAL(structures.linkOffset.size(), 2u);
    BOOST_REQUIRE_EQUAL(structures.linkIndex.size(), 2u);
    BOOST_CHECK(std::find(structures.linkWeight.begin(), structures.linkWeight.end(), 1.0)
                != structures.linkWeight.end());
    BOOST_CHECK(std::find(structures.linkWeight.begin(), structures.linkWeight.end(), 3.0)
                != structures.linkWeight.end());
    BOOST_CHECK(std::find(structures.linkOffset.begin(), structures.linkOffset.end(), 2)
                != structures.linkOffset.end());

    // clusterCount() only counts the active cluster: vectors are sized and filled accordingly
    BOOST_REQUIRE_EQUAL(structures.clusterWeight.size(), 1u);
    BOOST_REQUIRE_EQUAL(structures.clusterOffset.size(), 1u);
    BOOST_REQUIRE_EQUAL(structures.clusterIndex.size(), 1u);
    BOOST_REQUIRE_EQUAL(structures.clustersAreaIndex.size(), 1u);
    BOOST_CHECK_EQUAL(structures.clusterWeight[0], 4.0);
    BOOST_CHECK_EQUAL(structures.clusterOffset[0], 5);
    BOOST_CHECK_EQUAL(structures.clusterIndex[0], (long)activeCluster->index);
    BOOST_CHECK_EQUAL(structures.clustersAreaIndex[0], (long)area1->index);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_timeSeriesFileName)
{
    auto study = std::make_unique<Study>();
    EnvForLoading env(study->areas, StudyVersion(8, 7));
    env.folder = "some/folder";

    BindingConstraint bc;
    bc.pId("dummy_id");

    bc.operatorType(BindingConstraint::opLess);
    BOOST_CHECK_EQUAL(bc.timeSeriesFileName(env),
                      (std::filesystem::path("some/folder") / "dummy_id_lt.txt").string());

    bc.operatorType(BindingConstraint::opGreater);
    BOOST_CHECK_EQUAL(bc.timeSeriesFileName(env),
                      (std::filesystem::path("some/folder") / "dummy_id_gt.txt").string());

    bc.operatorType(BindingConstraint::opEquality);
    BOOST_CHECK_EQUAL(bc.timeSeriesFileName(env),
                      (std::filesystem::path("some/folder") / "dummy_id_eq.txt").string());

    bc.operatorType(BindingConstraint::opBoth);
    BOOST_CHECK_EQUAL(bc.timeSeriesFileName(env), "");

    bc.operatorType(BindingConstraint::opUnknown);
    BOOST_CHECK_EQUAL(bc.timeSeriesFileName(env), "");
}

BOOST_AUTO_TEST_CASE(BindingConstraint_clustersAndWeights)
{
    auto study = std::make_unique<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    auto cluster1 = std::make_shared<ThermalCluster>(area1);
    cluster1->setName("cluster1");
    area1->thermal.list.addToCompleteList(cluster1);
    auto cluster2 = std::make_shared<ThermalCluster>(area1);
    cluster2->setName("cluster2");
    area1->thermal.list.addToCompleteList(cluster2);
    area1->thermal.list.buildIndexes();

    BindingConstraint bc;
    bc.weight(cluster1.get(), 1.5);
    bc.weight(cluster2.get(), 2.5);

    const auto& weights = bc.clustersAndWeights();
    BOOST_REQUIRE_EQUAL(weights.size(), 2u);
    BOOST_CHECK_CLOSE(weights.at(cluster1.get()), 1.5, 0.0001);
    BOOST_CHECK_CLOSE(weights.at(cluster2.get()), 2.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_copyWeights_and_copyOffsets_preserveUntouched)
{
    // Destination study: what we copy into
    auto destStudy = std::make_unique<Study>();
    auto destArea1 = addAreaToListOfAreas(destStudy->areas, "area1");
    auto destArea2 = addAreaToListOfAreas(destStudy->areas, "area2");
    auto destArea3 = addAreaToListOfAreas(destStudy->areas, "area3");
    auto destLink = AreaAddLinkBetweenAreas(destArea1, destArea2);
    auto untouchedLink = AreaAddLinkBetweenAreas(destArea1, destArea3);
    auto destCluster = std::make_shared<ThermalCluster>(destArea1);
    destCluster->setName("cluster1");
    destArea1->thermal.list.addToCompleteList(destCluster);
    destArea1->thermal.list.buildIndexes();

    // Source study: describes the constraint being copied from
    auto srcStudy = std::make_unique<Study>();
    auto srcArea1 = addAreaToListOfAreas(srcStudy->areas, "area1");
    auto srcArea2 = addAreaToListOfAreas(srcStudy->areas, "area2");
    auto srcLink = AreaAddLinkBetweenAreas(srcArea1, srcArea2);
    auto srcCluster = std::make_shared<ThermalCluster>(srcArea1);
    srcCluster->setName("cluster1");
    srcArea1->thermal.list.addToCompleteList(srcCluster);
    // No counterpart in destStudy: exercises the "unresolved entries are silently skipped" path
    auto srcOrphanArea = addAreaToListOfAreas(srcStudy->areas, "orphan_area");
    auto srcOrphanLink = AreaAddLinkBetweenAreas(srcArea1, srcOrphanArea);
    srcArea1->thermal.list.buildIndexes();

    BindingConstraint rhs;
    rhs.weight(srcLink, 1.5);
    rhs.offset(srcLink, 2);
    rhs.weight(srcCluster.get(), 3.5);
    rhs.offset(srcCluster.get(), 4);
    rhs.weight(srcOrphanLink, 9.0);

    const auto identityTranslate = [](AreaName& out, const AreaName& in) { out = in; };

    BindingConstraint bc;
    bc.weight(destLink, 42.0);      // will be overwritten by the copy
    bc.weight(untouchedLink, 99.0); // not referenced by rhs: must survive emptyBefore=false

    bc.copyWeights(*destStudy, rhs, /*emptyBefore*/ false, identityTranslate);
    bc.copyOffsets(*destStudy, rhs, /*emptyBefore*/ false, identityTranslate);

    BOOST_CHECK_CLOSE(bc.weight(destLink), 1.5, 0.0001);
    BOOST_CHECK_EQUAL(bc.offset(destLink), 2);
    BOOST_CHECK_CLOSE(bc.weight(destCluster.get()), 3.5, 0.0001);
    BOOST_CHECK_EQUAL(bc.offset(destCluster.get()), 4);
    BOOST_CHECK_CLOSE(bc.weight(untouchedLink), 99.0, 0.0001);
    // the unresolved orphan link must not have created any spurious entry
    BOOST_CHECK_EQUAL(bc.linkCount(), 2u);
}

BOOST_AUTO_TEST_CASE(BindingConstraint_copyWeights_emptyBeforeClearsExisting)
{
    auto destStudy = std::make_unique<Study>();
    auto destArea1 = addAreaToListOfAreas(destStudy->areas, "area1");
    auto destArea2 = addAreaToListOfAreas(destStudy->areas, "area2");
    auto destArea3 = addAreaToListOfAreas(destStudy->areas, "area3");
    auto destLink = AreaAddLinkBetweenAreas(destArea1, destArea2);
    auto staleLink = AreaAddLinkBetweenAreas(destArea1, destArea3);

    auto srcStudy = std::make_unique<Study>();
    auto srcArea1 = addAreaToListOfAreas(srcStudy->areas, "area1");
    auto srcArea2 = addAreaToListOfAreas(srcStudy->areas, "area2");
    auto srcLink = AreaAddLinkBetweenAreas(srcArea1, srcArea2);

    BindingConstraint rhs;
    rhs.weight(srcLink, 7.0);

    const auto identityTranslate = [](AreaName& out, const AreaName& in) { out = in; };

    BindingConstraint bc;
    bc.weight(staleLink, 123.0);

    bc.copyWeights(*destStudy, rhs, /*emptyBefore*/ true, identityTranslate);

    BOOST_CHECK_EQUAL(bc.weight(staleLink), 0.); // cleared by emptyBefore
    BOOST_CHECK_CLOSE(bc.weight(destLink), 7.0, 0.0001);
    BOOST_CHECK_EQUAL(bc.linkCount(), 1u);
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
