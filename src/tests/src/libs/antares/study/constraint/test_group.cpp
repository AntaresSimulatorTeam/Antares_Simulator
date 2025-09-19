/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
//
// Created by marechaljas on 28/06/23.
//
#define BOOST_TEST_MODULE binding_constraints_groups
#define WIN32_LEAN_AND_MEAN
#include <files-system.h>
#include <filesystem>
#include <fstream>
#include <memory>

#include <boost/test/unit_test.hpp>

#include "antares/study/study.h"

using namespace Antares::Data;
namespace fs = std::filesystem;

class PublicStudy final: public Study
{
public:
    bool internalLoadBindingConstraints(const StudyLoadOptions& options) override
    {
        return Study::internalLoadBindingConstraints(options);
    }
};

struct Fixture
{
    Fixture()
    {
        fs::create_directories(working_tmp_dir / "bindingconstraints");
        study->header.version = StudyVersion(8, 7);
        study->folderInput = working_tmp_dir.string();
    }

    void addConstraint(const std::string& name, const std::string& group) const
    {
        std::ofstream constraints(working_tmp_dir / "bindingconstraints" / "bindingconstraints.ini",
                                  std::ios_base::app);
        static unsigned constraintNumber = 1;
        constraints << "[" << constraintNumber++ << "]\n"
                    << "name = " << name << "\n"
                    << "id = " << name << "\n"
                    << "enabled = false\n"
                    << "type = hourly\n"
                    << "operator = equal\n"
                    << "filter-year-by-year = annual\n"
                    << "filter-synthesis = hourly\n"
                    << "comments = dummy_comment\n"
                    << "group = " << group << "\n";
        constraints.close();
        std::ofstream rhs(working_tmp_dir / "bindingconstraints" / (name + "_eq.txt"));
        rhs.close();
    }

    std::shared_ptr<PublicStudy> study = std::make_shared<PublicStudy>();
    StudyLoadOptions options;
    std::filesystem::path working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
};

BOOST_AUTO_TEST_SUITE(BindingConstraintTests_Groups)

BOOST_FIXTURE_TEST_CASE(WhenLoadingAConstraint_AGroupExists, Fixture)
{
    addConstraint("dummy_name", "dummy_group");
    const bool loading_ok = study->internalLoadBindingConstraints(options);

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(study->bindingConstraints.size(), 1);
    BOOST_CHECK_EQUAL(study->bindingConstraintsGroups.size(), 1);

    auto constraint = *study->bindingConstraints.begin();

    BOOST_CHECK_EQUAL(constraint->name(), "dummy_name");
    BOOST_CHECK_EQUAL(constraint->id(), "dummy_name");
    BOOST_CHECK_EQUAL(constraint->enabled(), false);
    BOOST_CHECK_EQUAL(constraint->type(), BindingConstraint::Type::typeHourly);
    BOOST_CHECK_EQUAL(constraint->operatorType(), BindingConstraint::Operator::opEquality);
    BOOST_CHECK_EQUAL(constraint->yearByYearFilter(), FilterFlag::filterAnnual);
    BOOST_CHECK_EQUAL(constraint->synthesisFilter(), FilterFlag::filterHourly);
    BOOST_CHECK_EQUAL(constraint->comments(), "dummy_comment");
    BOOST_CHECK_EQUAL(constraint->group(), "dummy_group");
}

BOOST_FIXTURE_TEST_CASE(WhenLoadingsConstraints_AllGroupExists, Fixture)
{
    addConstraint("dummy_name_1", "dummy_group_uno");
    addConstraint("dummy_name_2", "dummy_group_uno");
    addConstraint("dummy_name_3", "dummy_group_other");

    study->header.version = StudyVersion(8, 7);

    study->folderInput = working_tmp_dir.string();
    const bool loading_ok = study->internalLoadBindingConstraints(options);

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(study->bindingConstraints.size(), 3);
    BOOST_CHECK_EQUAL(study->bindingConstraintsGroups.size(), 2);
}

BOOST_FIXTURE_TEST_CASE(WhenLoadingsConstraints_AllGroupsNonEmpty, Fixture)
{
    addConstraint("dummy_name_1", "dummy_group_uno");
    addConstraint("dummy_name_2", "dummy_group_uno");
    addConstraint("dummy_name_3", "dummy_group_other");

    study->header.version = StudyVersion(8, 7);

    study->folderInput = working_tmp_dir.string();
    const bool loading_ok = study->internalLoadBindingConstraints(options);

    BOOST_CHECK_EQUAL(loading_ok, true);
    BOOST_CHECK_EQUAL(study->bindingConstraints.size(), 3);
    BOOST_CHECK_EQUAL(study->bindingConstraintsGroups.size(), 2);
    BOOST_CHECK(std::all_of(study->bindingConstraintsGroups.begin(),
                            study->bindingConstraintsGroups.end(),
                            [](const auto& group) { return !group->constraints().empty(); }));
}

BOOST_AUTO_TEST_CASE(group_numberOfTimeseries_empty)
{
    BindingConstraintGroup group("group");
    BOOST_CHECK_EQUAL(group.numberOfTimeseries(), 0);
}

void addToGroup(BindingConstraintGroup& group, const std::string& name, unsigned int width)
{
    auto c = std::make_shared<BindingConstraint>();
    c->RHSTimeSeries().reset(width, /*height = */ 1);
    c->name(name);
    group.add(c);
}

BOOST_AUTO_TEST_CASE(group_numberOfTimeseries_all_equal)
{
    BindingConstraintGroup group("group");
    addToGroup(group, "bc-1", 5);
    addToGroup(group, "bc-2", 5);
    BOOST_CHECK_EQUAL(group.numberOfTimeseries(), 5);
}

BOOST_AUTO_TEST_CASE(group_numberOfTimeseries_some_one)
{
    BindingConstraintGroup group("group");
    addToGroup(group, "bc-1", 1);
    addToGroup(group, "bc-2", 5);
    BOOST_CHECK_EQUAL(group.numberOfTimeseries(), 5);
}

BOOST_AUTO_TEST_CASE(group_numberOfTimeseries_some_one_mixed)
{
    BindingConstraintGroup group("group");
    addToGroup(group, "bc-1", 1);
    addToGroup(group, "bc-2", 5);
    addToGroup(group, "bc-3", 1);
    BOOST_CHECK_EQUAL(group.numberOfTimeseries(), 5);
}

BOOST_AUTO_TEST_CASE(group_numberOfTimeseries_same_name)
{
    BindingConstraintGroup group("group");
    addToGroup(group, "bc-1", 1);
    addToGroup(group, "bc-1", 5); // Name collision, not added to group
    addToGroup(group, "bc-3", 1);
    BOOST_CHECK_EQUAL(group.numberOfTimeseries(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
