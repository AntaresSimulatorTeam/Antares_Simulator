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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/study/sets.h"

namespace Antares::Data
{
Sets::Sets(const Sets& rhs):
    pMap(rhs.pMap),
    pOptions(rhs.pOptions)
{
    if (rhs.pByIndex.size())
    {
        rebuildIndexes();
    }
}

Sets::iterator Sets::begin()
{
    return pMap.begin();
}

Sets::const_iterator Sets::begin() const
{
    return pMap.begin();
}

Sets::iterator Sets::end()
{
    return pMap.end();
}

Sets::const_iterator Sets::end() const
{
    return pMap.end();
}

void Sets::clear()
{
    pByIndex.clear();
    pNameByIndex.clear();
    pMap.clear();
    pOptions.clear();
}

Sets::SetAreasType& Sets::operator[](uint i)
{
    assert(i < pMap.size() && "Sets: operator[] index out of bounds");
    return *(pByIndex[i]);
}

const Sets::SetAreasType& Sets::operator[](uint i) const
{
    assert(i < pMap.size() && "Sets: operator[] index out of bounds");
    return *(pByIndex[i]);
}

bool Sets::hasOutput(const Yuni::ShortString128& s) const
{
    const auto pair = pOptions.find(s);
    return (pair != pOptions.end()) ? pair->second.output : false;
}

uint Sets::resultSize(const Yuni::ShortString128& s) const
{
    const auto pair = pOptions.find(s);
    return (pair != pOptions.end()) ? pair->second.resultSize : 0;
}

Sets::IDType Sets::caption(const Yuni::ShortString128& s) const
{
    const auto pair = pOptions.find(s);
    return (pair != pOptions.end()) ? pair->second.caption : IDType();
}

void Sets::defaultForAreas()
{
    using namespace Yuni;
    clear();
    Options opts;
    opts.caption = "All areas";
    opts.comments = "Spatial aggregates on all areas";
    opts.output = false;
    opts.rules.push_back(Rule(ruleFilter, "add-all"));
    auto district = std::make_shared<SetAreasType>();
    add("all areas", district, opts);
}

void Sets::rebuildAllFromRules(SetHandlerAreas& handler)
{
    for (uint i = 0; i != pMap.size(); ++i)
    {
        rebuildFromRules(pNameByIndex[i], handler);
    }
}

void Sets::rebuildFromRules(const IDType& id, SetHandlerAreas& handler)
{
    using namespace Yuni;
    using namespace Antares;

    const auto pair = pOptions.find(id);
    if (pair == pOptions.end())
    {
        return;
    }

    // Options
    Options& opts = pair->second;
    auto& set = *(pMap[id]);

    // Clear the result first
    handler.clear(set);
    // Apply all rules
    for (uint i = 0; i != opts.rules.size(); ++i)
    {
        const Rule& rule = opts.rules[i];
        const std::string name = rule.second;
        switch (rule.first) // type
        {
        case ruleAdd:
        {
            // Trying to add a single item
            if (!handler.add(set, name))
            {
                // Failed. Maybe the argument references another group
                const IDType other = name;
                MapType::iterator i = pMap.find(other);
                if (i != pMap.end())
                {
                    handler.add(set, *(i->second));
                }
            }
            break;
        }
        case ruleRemove:
        {
            // Trying to remove a single item
            if (!handler.remove(set, name))
            {
                // Failed. Maybe the argument references another group
                const IDType other = name;
                MapType::iterator i = pMap.find(other);
                if (i != pMap.end())
                {
                    handler.remove(set, *(i->second));
                }
            }
            break;
        }
        case ruleFilter:
        {
            handler.applyFilter(set, name);
            break;
        }
        case ruleNone:
        case ruleMax:
        {
            // Huh ??
            assert(false && "Should not be here !");
            break;
        }
        }
    }
    // Retrieving the size of the result set
    opts.resultSize = handler.size(set);
    logs.debug() << "  > set :: " << opts.caption << ": applying " << opts.rules.size()
                 << " rules, got " << opts.resultSize << " items";
}

bool Sets::saveToFile(const Yuni::String& filename) const
{
    Yuni::IO::File::Stream file;
    if (!file.open(filename, Yuni::IO::OpenMode::write | Yuni::IO::OpenMode::truncate))
    {
        logs.error() << "I/O Error: " << filename << ": impossible to write the file";
        return false;
    }

    static const char* cmds[ruleMax] = {"none", "+", "-", "apply-filter"};
    const auto end = pOptions.cend();
    for (auto i = pOptions.cbegin(); i != end; ++i)
    {
        const Options& opts = i->second;
        file << '[' << i->first << "]\n";
        file << "caption = " << opts.caption << '\n';
        if (not opts.comments.empty())
        {
            file << "comments = " << opts.comments << '\n';
        }
        if (!opts.output)
        {
            file << "output = false\n";
        }

        for (uint r = 0; r != opts.rules.size(); ++r)
        {
            const Rule& rule = opts.rules[r];
            file << cmds[rule.first] << " = " << rule.second << '\n';
        }
        file << '\n';
    }
    return true;
}

YString Sets::toString()
{
    using namespace Yuni;
    using namespace Antares;
    static const char* cmds[ruleMax] = {"none", "+", "-", "apply-filter"};
    YString ret = "";
    for (const auto& [setId, options] : pOptions)
    {
        const Options& opts = options;
        ret << '[' << setId << "]\n";
        ret << "caption = " << opts.caption << '\n';
        if (not opts.comments.empty())
        {
            ret << "comments = " << opts.comments << '\n';
        }
        if (!opts.output)
        {
            ret << "output = false\n";
        }

        for (uint r = 0; r != opts.rules.size(); ++r)
        {
            const Rule& rule = opts.rules[r];
            ret << cmds[rule.first] << " = " << rule.second << '\n';
        }
        ret << '\n';
    }
    return ret;
}

bool Sets::loadFromFile(const std::filesystem::path& filename)
{
    using namespace Yuni;
    using namespace Antares;

    // Empty the container first
    clear();

    // Loading the INI file
    if (!std::filesystem::exists(filename))
    {
        // Error silently ignored
        return true;
    }

    IniFile ini;
    if (ini.open(filename))
    {
        Yuni::String value;

        // each section...
        for (auto* section = ini.firstSection; section != nullptr; section = section->next)
        {
            // Clearing the name.
            if (!section->name)
            {
                continue;
            }

            // Creating a new section
            auto district = std::make_shared<SetAreasType>();
            Options opts;
            opts.caption = section->name;

            // each property...
            const IniFile::Property* p;
            for (p = section->firstProperty; p != nullptr; p = p->next)
            {
                if (p->key.empty())
                {
                    continue;
                }

                value = p->value;
                value.toLower();

                if (p->key == "+")
                {
                    opts.rules.push_back(Rule(ruleAdd, value.to<std::string>()));
                    continue;
                }
                if (p->key == "-")
                {
                    opts.rules.push_back(Rule(ruleRemove, value.to<std::string>()));
                    continue;
                }
                if (p->key == "apply-filter")
                {
                    opts.rules.push_back(Rule(ruleFilter, value.to<std::string>()));
                    continue;
                }
                if (p->key == "output")
                {
                    opts.output = value.to<bool>();
                    continue;
                }
                if (p->key == "comments")
                {
                    opts.comments = p->value;
                    opts.comments.trim(" \t");
                    continue;
                }
                if (p->key == "caption")
                {
                    opts.caption = p->value;
                    continue;
                }

                logs.warning() << "sets: `" << filename << "`: Invalid property `" << p->key
                               << '\'';
            }

            // Add the new group
            IDType newid = section->name;
            newid.toLower();
            add(newid, district, opts);
        }

        // Not modified anymore
        pModified = false;
        // All indexes must be rebuilt
        rebuildIndexes();
        return true;
    }
    return false;
}

void Sets::rebuildIndexes()
{
    pNameByIndex.clear();
    pNameByIndex.resize(pMap.size());

    pByIndex.clear();
    pByIndex.resize(pMap.size());

    uint index = 0;
    for (auto& [setId, set] : pMap)
    {
        pByIndex[index] = set;
        pNameByIndex[index] = setId;
        ++index;
    }
}

bool Sets::hasOutput(const uint index) const
{
    return hasOutput(IDType(pNameByIndex[index]));
}

Sets::IDType Sets::caption(const uint i) const
{
    return caption(IDType(pNameByIndex[i]));
}

uint Sets::resultSize(const uint index) const
{
    return resultSize(IDType(pNameByIndex[index]));
}

void Sets::dumpToLogs() const
{
    using namespace Yuni;
    for (auto& [setId, set] : pMap)
    {
        logs.info() << "   found `" << setId << "` (" << set->size() << ' '
                    << (set->size() < 2 ? "item" : "items")
                    << ((!hasOutput(setId)) ? ", no output" : "") << ')';
    }
}

uint Sets::size() const
{
    return (uint)pMap.size();
}

SetHandlerAreas::SetHandlerAreas(AreaList& areas):
    areas_(areas)
{
}

void SetHandlerAreas::clear(Sets::SetAreasType& set)
{
    set.clear();
}

uint SetHandlerAreas::size(Sets::SetAreasType& set)
{
    return (uint)set.size();
}

bool SetHandlerAreas::add(Sets::SetAreasType& set, const std::string& value)
{
    Area* area = AreaListLFind(&areas_, value.c_str());
    if (area)
    {
        set.insert(area);
        return true;
    }
    return false;
}

void SetHandlerAreas::add(Sets::SetAreasType& set, const Sets::SetAreasType& otherSet)
{
    set.insert(otherSet.begin(), otherSet.end());
}

bool SetHandlerAreas::remove(Sets::SetAreasType& set, const std::string& value)
{
    Area* area = AreaListLFind(&areas_, value.c_str());
    if (area)
    {
        set.erase(area);
        return true;
    }
    return false;
}

void SetHandlerAreas::remove(Sets::SetAreasType& set, const Sets::SetAreasType& otherSet)
{
    std::ranges::for_each(otherSet, [&set](auto* area) { set.erase(area); });
}

bool SetHandlerAreas::applyFilter(Sets::SetAreasType& set, const std::string& value)
{
    if (value == "add-all")
    {
        for (const auto& [areaName, area] : areas_)
        {
            set.insert(area);
        }
        return true;
    }

    if (value == "remove-all")
    {
        set.clear();
        return true;
    }
    return false;
}

} // namespace Antares::Data
