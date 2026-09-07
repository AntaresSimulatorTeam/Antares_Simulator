// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/sets.h"

#include <string>
#include <yaml-cpp/yaml.h>

#include <antares/utils/utils.h>

namespace Antares::Data
{
namespace
{
std::string trim(const std::string& s)
{
    const auto begin = s.find_first_not_of(" \t");
    if (begin == std::string::npos)
    {
        return std::string();
    }
    const auto end = s.find_last_not_of(" \t");
    return s.substr(begin, end - begin + 1);
}

} // namespace

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

Sets::SetAreasType& Sets::operator[](unsigned int i)
{
    assert(i < pMap.size() && "Sets: operator[] index out of bounds");
    return *(pByIndex[i]);
}

const Sets::SetAreasType& Sets::operator[](unsigned int i) const
{
    assert(i < pMap.size() && "Sets: operator[] index out of bounds");
    return *(pByIndex[i]);
}

void Sets::dumpToLogs() const
{
    for (const auto& [setId, set]: pMap)
    {
        logs.info() << "   found `" << setId << "` (" << set->size() << ' '
                    << (set->size() < 2 ? "item" : "items")
                    << ((!hasOutput(setId)) ? ", no output" : "") << ')';
    }
}

void Sets::defaultForAreas()
{
    clear();
    Options opts;
    opts.caption = "All areas";
    opts.comments = "Spatial aggregates on all areas";
    opts.output = false;
    opts.rules.emplace_back(ruleFilter, "add-all");
    auto district = std::make_shared<SetAreasType>();
    add("all areas", district, opts);
}

std::string Sets::toString()
{
    static const char* cmds[ruleMax] = {"none", "+", "-", "apply-filter"};
    std::string ret;
    for (const auto& [setId, options]: pOptions)
    {
        const Options& opts = options;
        ret += '[' + setId + "]\n";
        ret += "caption = " + opts.caption + '\n';
        if (not opts.comments.empty())
        {
            ret += "comments = " + opts.comments + '\n';
        }
        if (!opts.output)
        {
            ret += "output = false\n";
        }

        for (unsigned int r = 0; r != opts.rules.size(); ++r)
        {
            const Rule& rule = opts.rules[r];
            ret += std::string(cmds[rule.first]) + " = " + rule.second + '\n';
        }
        ret += '\n';
    }
    return ret;
}

bool Sets::loadFromFile(const std::filesystem::path& filename)
{
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
        std::string value;

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

                value = std::string(p->value);
                value = stringToLower(value);

                if (p->key == "+")
                {
                    opts.rules.emplace_back(ruleAdd, value);
                    continue;
                }
                if (p->key == "-")
                {
                    opts.rules.emplace_back(ruleRemove, value);
                    continue;
                }
                if (p->key == "apply-filter")
                {
                    opts.rules.emplace_back(ruleFilter, value);
                    continue;
                }
                if (p->key == "output")
                {
                    opts.output = stringToBool(value);
                    continue;
                }
                if (p->key == "comments")
                {
                    opts.comments = std::string(p->value);
                    opts.comments = trim(opts.comments);
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
            IDType newid = std::string(section->name);
            newid = stringToLower(newid);
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

void Sets::rebuildAllFromRules(SetHandlerAreas& handler)
{
    for (const auto& setId: pNameByIndex)
    {
        rebuildFromRules(setId, handler);
    }
}

bool Sets::loadOutputPrecisionsFromFile(const std::filesystem::path& filename)
{
    if (!std::filesystem::exists(filename))
    {
        // Optional file: default behavior is kept (all precisions exported)
        return true;
    }

    logs.info() << "sets: loading the district output granularities from `" << filename << "`";

    YAML::Node root;
    try
    {
        root = YAML::LoadFile(filename.string());
    }
    catch (const YAML::Exception& e)
    {
        logs.error() << "sets: impossible to parse `" << filename << "`: " << e.what();
        return false;
    }

    if (!root || !root.IsMap())
    {
        logs.warning() << "sets: `" << filename
                       << "`: the root node must be a mapping of "
                          "districts";
        return true;
    }

    for (const auto& entry: root)
    {
        if (!entry.first.IsScalar())
        {
            logs.warning() << "sets: `" << filename << "`: invalid district key, ignored";
            continue;
        }
        const std::string districtKey = entry.first.as<std::string>();
        const IDType districtId = stringToLower(districtKey);
        const auto pair = pOptions.find(districtId);
        if (pair == pOptions.end())
        {
            logs.warning() << "sets: `" << filename << "`: unknown district `" << districtKey
                           << "`, ignored";
            continue;
        }
        if (!entry.second.IsMap())
        {
            logs.warning() << "sets: `" << filename << "`: district `" << districtKey
                           << "` must be a mapping with `mc-all` and/or `mc-ind` keys, ignored";
            continue;
        }

        const YAML::Node& options = entry.second;
        for (const char* reportType: {"mc-all", "mc-ind"})
        {
            const YAML::Node node = options[reportType];
            if (!node.IsDefined())
            {
                continue;
            }
            if (!node.IsSequence())
            {
                logs.warning() << "sets: `" << filename << "`: district `" << districtKey << "`, `"
                               << reportType
                               << "` must be a sequence of "
                                  "granularities, ignored";
                continue;
            }
            std::string value;
            for (const auto& item: node)
            {
                if (!item.IsScalar())
                {
                    continue;
                }
                if (!value.empty())
                {
                    value += ", ";
                }
                value += item.as<std::string>();
            }
            const unsigned int mask = stringIntoDatePrecision(value);
            if (reportType == std::string("mc-all"))
            {
                pair->second.filterSynthesis = mask;
            }
            else
            {
                pair->second.filterYearByYear = mask;
            }
        }
    }

    // Summary of the applied filters
    for (const auto& pair: pOptions)
    {
        if (pair.second.filterSynthesis != filterAll)
        {
            logs.info() << "sets: district `" << pair.first << "`: mc-all granularities = "
                        << (pair.second.filterSynthesis == filterNone
                              ? std::string("none")
                              : datePrecisionIntoString(pair.second.filterSynthesis));
        }
        if (pair.second.filterYearByYear != filterAll)
        {
            logs.info() << "sets: district `" << pair.first << "`: mc-ind granularities = "
                        << (pair.second.filterYearByYear == filterNone
                              ? std::string("none")
                              : datePrecisionIntoString(pair.second.filterYearByYear));
        }
    }
    return true;
}

unsigned int Sets::outputFilter(const IDType& id, bool synthesis) const
{
    const auto pair = pOptions.find(id);
    if (pair == pOptions.end())
    {
        return filterAll;
    }
    return synthesis ? pair->second.filterSynthesis : pair->second.filterYearByYear;
}

void Sets::rebuildFromRules(const IDType& id, SetHandlerAreas& handler)
{
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
    for (unsigned int i = 0; i != opts.rules.size(); ++i)
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

void Sets::rebuildIndexes()
{
    pNameByIndex.clear();
    pNameByIndex.resize(pMap.size());

    pByIndex.clear();
    pByIndex.resize(pMap.size());

    unsigned int index = 0;
    for (const auto& [setId, set]: pMap)
    {
        pByIndex[index] = set;
        pNameByIndex[index] = setId;
        ++index;
    }
}

bool Sets::hasOutput(const std::string& s) const
{
    const auto pair = pOptions.find(s);
    return (pair != pOptions.end()) ? pair->second.output : false;
}

bool Sets::hasOutput(const unsigned int index) const
{
    return hasOutput(IDType(pNameByIndex[index]));
}

unsigned int Sets::resultSize(const std::string& s) const
{
    const auto pair = pOptions.find(s);
    return (pair != pOptions.end()) ? pair->second.resultSize : 0;
}

Sets::IDType Sets::caption(const std::string& s) const
{
    const auto pair = pOptions.find(s);
    return (pair != pOptions.end()) ? pair->second.caption : IDType();
}

Sets::IDType Sets::caption(const unsigned int i) const
{
    return caption(IDType(pNameByIndex[i]));
}

unsigned int Sets::resultSize(const unsigned int index) const
{
    return resultSize(IDType(pNameByIndex[index]));
}

unsigned int Sets::size() const
{
    return (unsigned int)pMap.size();
}

SetHandlerAreas::SetHandlerAreas(AreaList& areas):
    areas_(areas)
{
}

void SetHandlerAreas::clear(Sets::SetAreasType& set)
{
    set.clear();
}

unsigned int SetHandlerAreas::size(Sets::SetAreasType& set)
{
    return (unsigned int)set.size();
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
        for (const auto& [areaName, area]: areas_)
        {
            set.insert(area.get());
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
