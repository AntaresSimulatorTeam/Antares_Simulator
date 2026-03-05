// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 11/05/23.
//

#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <antares/study/study.h>
#include "antares/study/binding_constraint/BindingConstraint.h"
#include "antares/study/binding_constraint/BindingConstraintLoader.h"

namespace Antares::Data
{
std::shared_ptr<Data::BindingConstraint> BindingConstraintsRepository::find(const AnyString& id)
{
    for (const auto& i: constraints_)
    {
        if (i->id() == id)
        {
            return i;
        }
    }
    return nullptr;
}

std::shared_ptr<const Data::BindingConstraint> BindingConstraintsRepository::find(
  const AnyString& id) const
{
    for (const auto& i: constraints_)
    {
        if (i->id() == id)
        {
            return i;
        }
    }
    return nullptr;
}

BindingConstraint* BindingConstraintsRepository::findByName(const AnyString& name)
{
    for (const auto& i: constraints_)
    {
        if (i->name() == name)
        {
            return i.get();
        }
    }
    return nullptr;
}

const BindingConstraint* BindingConstraintsRepository::findByName(const AnyString& name) const
{
    for (const auto& i: constraints_)
    {
        if (i->name() == name)
        {
            return i.get();
        }
    }
    return nullptr;
}

static int valueForSort(BindingConstraint::Operator op)
{
    switch (op)
    {
    case BindingConstraint::opLess:
        return 0;
    case BindingConstraint::opGreater:
        return 1;
    case BindingConstraint::opEquality:
        return 2;
    case BindingConstraint::opBoth:
        return 3;
    default:
        return -1;
    }
}

bool compareConstraints(const std::shared_ptr<BindingConstraint>& s1,
                        const std::shared_ptr<BindingConstraint>& s2)
{
    if (s1->name() != s2->name())
    {
        return s1->name() < s2->name();
    }
    else
    {
        return valueForSort(s1->operatorType()) < valueForSort(s2->operatorType());
    }
}

std::shared_ptr<BindingConstraint> BindingConstraintsRepository::add(const AnyString& name)
{
    auto bc = std::make_shared<BindingConstraint>();
    bc->name(name);
    bc->pId(name);
    constraints_.push_back(bc);
    std::sort(constraints_.begin(), constraints_.end(), compareConstraints);
    return bc;
}

std::vector<std::shared_ptr<BindingConstraint>> BindingConstraintsRepository::LoadBindingConstraint(
  EnvForLoading env)
{
    BindingConstraintLoader loader;
    return loader.load(std::move(env));
}

bool BindingConstraintsRepository::loadFromFolder(Study& study,
                                                  const StudyLoadOptions& options,
                                                  const std::filesystem::path& folder)
{
    // Log entries
    logs.info(); // space for beauty
    logs.info() << "Loading constraints...";

    if (options.ignoreConstraints)
    {
        logs.info() << "  The constraints have been disabled by the user";
        return true;
    }
    if (!study.parameters.include.constraints)
    {
        logs.info() << "  The constraints shall be ignored due to the optimization preferences";
        return true;
    }

    EnvForLoading env(study.areas, study.header.version);
    env.folder = folder;

    env.iniFilename = folder / "bindingconstraints.ini";
    IniFile ini;
    if (!ini.open(env.iniFilename))
    {
        return false;
    }

    // For each section
    if (ini.firstSection)
    {
        for (env.section = ini.firstSection; env.section; env.section = env.section->next)
        {
            if (env.section->firstProperty)
            {
                auto new_bc = LoadBindingConstraint(env);
                std::copy(new_bc.begin(), new_bc.end(), std::back_inserter(constraints_));
            }
        }
    }

    // Logs
    if (constraints_.empty())
    {
        logs.info() << "No binding constraint found";
    }
    else
    {
        std::sort(constraints_.begin(), constraints_.end(), compareConstraints);

        if (constraints_.size() == 1)
        {
            logs.info() << "1 binding constraint found";
        }
        else
        {
            logs.info() << constraints_.size() << " binding constraints found";
        }
    }

    // When the simplex is in `weekly` mode,
    // all weekly constraints will become daily ones.
    if (sorDay == study.parameters.simplexOptimizationRange)
    {
        changeConstraintsWeeklyToDaily();
    }

    return true;
}

void BindingConstraintsRepository::changeConstraintsWeeklyToDaily()
{
    each(
      [](BindingConstraint& constraint)
      {
          if (constraint.type() == BindingConstraint::typeWeekly)
          {
              logs.info() << "  The type of the constraint '" << constraint.name()
                          << "' is now 'daily'";
              constraint.setTimeGranularity(BindingConstraint::typeDaily);
          }
      });
}

BindingConstraintsRepository::iterator BindingConstraintsRepository::begin()
{
    return constraints_.begin();
}

BindingConstraintsRepository::const_iterator BindingConstraintsRepository::begin() const
{
    return constraints_.begin();
}

BindingConstraintsRepository::iterator BindingConstraintsRepository::end()
{
    return constraints_.end();
}

BindingConstraintsRepository::const_iterator BindingConstraintsRepository::end() const
{
    return constraints_.end();
}

std::vector<std::shared_ptr<BindingConstraint>> BindingConstraintsRepository::activeConstraints()
  const
{
    if (!activeConstraints_.empty())
    {
        return activeConstraints_;
    }

    for (auto& bc: constraints_)
    {
        if (bc->isActive())
        {
            activeConstraints_.push_back(bc);
        }
    }

    return activeConstraints_;
}

static bool isBindingConstraintTypeInequality(const Data::BindingConstraint& bc)
{
    return bc.operatorType() == BindingConstraint::opLess
           || bc.operatorType() == BindingConstraint::opGreater;
}

BindingConstraintsRepository::Vector
BindingConstraintsRepository::getPtrForInequalityBindingConstraints() const
{
    auto activeBC = activeConstraints();
    Vector ptr;

    for (auto& bc: activeBC)
    {
        if (isBindingConstraintTypeInequality(*bc))
        {
            ptr.push_back(bc);
        }
    }

    return ptr;
}

} // namespace Antares::Data
