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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
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
#include "antares/study/binding_constraint/BindingConstraintSaver.h"
#include "antares/utils/utils.h"

void Data::BindingConstraintsRepository::clear()
{
    constraints_.clear();
    activeConstraints_.clear();
}

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

void BindingConstraintsRepository::removeConstraintsWhoseNameConstains(const AnyString& filter)
{
    WhoseNameContains pred(filter);
    constraints_.erase(std::remove_if(constraints_.begin(), constraints_.end(), pred),
                       constraints_.end());
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

bool BindingConstraintsRepository::saveToFolder(const AnyString& folder) const
{
    BindingConstraintSaver::EnvForSaving env;
    env.folder = folder;
    return internalSaveToFolder(env);
}

bool BindingConstraintsRepository::rename(BindingConstraint* bc, const AnyString& newname)
{
    // Copy of the name
    ConstraintName name;
    name = newname;
    if (name == bc->name())
    {
        return true;
    }
    ConstraintName id = Antares::transformNameIntoID(name);
    if (std::any_of(constraints_.begin(),
                    constraints_.end(),
                    [&id](auto constraint) { return constraint->id() == id; }))
    {
        return false;
    }
    bc->name(name);
    bc->pId(name);
    JIT::Invalidate(bc->RHSTimeSeries().jit);
    return true;
}

bool BindingConstraintsRepository::loadFromFolder(Study& study,
                                                  const StudyLoadOptions& options,
                                                  const AnyString& folder)
{
    // Log entries
    logs.info(); // space for beauty
    logs.info() << "Loading constraints...";

    // Cleaning
    clear();

    if (study.usedByTheSolver)
    {
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
    }

    EnvForLoading env(study.areas, study.header.version);
    env.folder = folder;

    env.iniFilename << env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
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

    // When ran from the solver and if the simplex is in `weekly` mode,
    // all weekly constraints will become daily ones.
    if (study.usedByTheSolver && sorDay == study.parameters.simplexOptimizationRange)
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

bool BindingConstraintsRepository::internalSaveToFolder(
  BindingConstraintSaver::EnvForSaving& env) const
{
    if (constraints_.empty())
    {
        logs.info() << "No binding constraint to export.";
        if (!Yuni::IO::Directory::Create(env.folder))
        {
            return false;
        }
        // stripping the file
        env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
        return Yuni::IO::File::CreateEmptyFile(env.folder);
    }

    if (constraints_.size() == 1)
    {
        logs.info() << "Exporting 1 binding constraint...";
    }
    else
    {
        logs.info() << "Exporting " << constraints_.size() << " binding constraints...";
    }

    if (!Yuni::IO::Directory::Create(env.folder))
    {
        return false;
    }

    IniFile ini;
    bool ret = true;
    uint index = 0;
    auto end = constraints_.end();

    for (auto i = constraints_.begin(); i != end; ++i, ++index)
    {
        env.section = ini.addSection(std::to_string(index));
        ret = Antares::Data::BindingConstraintSaver::saveToEnv(env, i->get()) && ret;
    }

    env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
    return ini.save(env.folder) && ret;
}

void BindingConstraintsRepository::reverseWeightSign(const AreaLink* lnk)
{
    each([&lnk](BindingConstraint& constraint) { constraint.reverseWeightSign(lnk); });
}

uint64_t BindingConstraintsRepository::memoryUsage() const
{
    uint64_t m = sizeof(BindingConstraintsRepository);
    for (const auto& i: constraints_)
    {
        m += i->memoryUsage();
    }
    return m;
}

namespace // anonymous
{
template<class T>
class RemovePredicate final
{
public:
    explicit RemovePredicate(const T* u):
        pItem(u)
    {
    }

    bool operator()(const std::shared_ptr<BindingConstraint>& bc) const
    {
        assert(bc);
        if (bc->contains(pItem))
        {
            logs.info() << "destroying the binding constraint " << bc->name();
            return true;
        }
        return false;
    }

private:
    const T* pItem;
};

} // anonymous namespace

void BindingConstraintsRepository::remove(const Area* area)
{
    RemovePredicate<Area> predicate(area);
    auto e = std::remove_if(constraints_.begin(), constraints_.end(), predicate);
    constraints_.erase(e, constraints_.end());
    activeConstraints_.clear();
}

void BindingConstraintsRepository::remove(const AreaLink* lnk)
{
    RemovePredicate<AreaLink> predicate(lnk);
    auto e = std::remove_if(constraints_.begin(), constraints_.end(), predicate);
    constraints_.erase(e, constraints_.end());
    activeConstraints_.clear();
}

void BindingConstraintsRepository::remove(const BindingConstraint* bc)
{
    RemovePredicate<BindingConstraint> predicate(bc);
    auto e = std::remove_if(constraints_.begin(), constraints_.end(), predicate);
    constraints_.erase(e, constraints_.end());
    activeConstraints_.clear();
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

void BindingConstraintsRepository::markAsModified() const
{
    for (const auto& i: constraints_)
    {
        i->markAsModified();
    }
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

void BindingConstraintsRepository::forceReload(bool reload) const
{
    if (!constraints_.empty())
    {
        for (const auto& i: constraints_)
        {
            i->forceReload(reload);
        }
    }
}

} // namespace Antares::Data
