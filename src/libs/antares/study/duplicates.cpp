/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <set>
#include <string>

#include <antares/logs/logs.h>
#include <antares/study/study.h>

namespace
{
template<class T>
std::string getName(const T& t)
{
    return t.properties.name;
}

template<class T>
std::string getName(const std::shared_ptr<T>& t)
{
    return t->name();
}

template<class Container>
bool check(const Container& c, const std::string& objectType, const std::string& context)
{
    std::set<std::string> names;
    bool ret = true;
    for (const auto& it: c)
    {
        if (auto [name, inserted] = names.insert(getName(it)); !inserted)
        {
            logs.error() << "Duplicate " << objectType << " `" << *name << "` found in " << context;
            ret = false;
        }
    }
    return ret;
}

std::string quoteArea(const std::string& name)
{
    return "area `" + name + "`";
}

} // namespace

namespace Antares::Check
{
bool checkForDuplicates(const Antares::Data::Study& study)
{
    bool ret = true;
    ret = check(study.bindingConstraints, "binding constraint", "study") && ret;

    for (const auto& [areaName, area]: study.areas)
    {
        ret = check(area->thermal.list.all(), "thermal cluster", quoteArea(areaName)) && ret;
    }

    for (const auto& [areaName, area]: study.areas)
    {
        ret = check(area->renewable.list.all(), "renewable cluster", quoteArea(areaName)) && ret;
    }

    for (const auto& [areaName, area]: study.areas)
    {
        ret = check(area->shortTermStorage.storagesByIndex,
                    "short term storage",
                    quoteArea(areaName))
              && ret;
    }
    return ret;
}
} // namespace Antares::Check
