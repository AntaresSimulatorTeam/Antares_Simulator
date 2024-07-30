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
#pragma once

#include <cassert>
#include <map>
#include <memory>
#include <set>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include "antares/study/area/area.h"

namespace Antares::Data
{
class SetHandlerAreas;

class Sets final
{
public:
    //
    using IDType = Yuni::ShortString128;

    //! A single set of areas
    // CompareAreaName : to control the order of areas in a set of areas. This order can have an
    // effect, even if tiny, on the results of aggregations.
    using SetAreasType = std::set<Area*, CompareAreaName>;

    //! Value
    using TypePtr = std::shared_ptr<SetAreasType>;

    //! Map of Item
    using MapType = std::map<IDType, TypePtr>;
    //! Standard iterators from the STL
    using iterator = MapType::iterator;
    //! Standard iterators from the STL (const)
    using const_iterator = MapType::const_iterator;

    enum RuleType
    {
        ruleNone = 0,
        ruleAdd,
        ruleRemove,
        ruleFilter,
        ruleMax,
    };

    //! Definition of a single rule
    using Rule = std::pair<RuleType, std::string>;
    //! Rule Set
    using RuleSet = std::vector<Rule>;

    class Options final
    {
    public:
        Options():
            output(true),
            resultSize(0)
        {
        }

        Options(const Options& rhs):
            caption(rhs.caption),
            comments(rhs.comments),
            rules(rhs.rules),
            output(rhs.output),
            resultSize(rhs.resultSize)
        {
        }

        void reset(const IDType& id)
        {
            caption = id;
            comments.clear();
            rules.clear();
            output = false;
            resultSize = 0;
        }

        Options& operator=(const Options& rhs)
        {
            caption = rhs.caption;
            comments = rhs.comments;
            rules = rhs.rules;
            output = rhs.output;
            resultSize = rhs.resultSize;
            return *this;
        }

    public:
        //! Caption
        IDType caption;
        //! Comments
        Yuni::String comments;
        //! Rules to build the group
        RuleSet rules;
        //! Enable/Disable the results in the output
        bool output;
        //! The number of items in the result set
        uint resultSize;

    }; // class Options

    using MapOptions = std::map<IDType, Options>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Sets() = default;
    /*!
    ** \brief Copy constructor
    */
    Sets(const Sets& rhs);
    //! Destructor
    ~Sets() = default;
    //@}

    //! \name Iterators
    //@{
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    //@}

    /*!
    ** \brief Clear all groups
    */
    void clear();

    TypePtr add(const IDType& name)
    {
        TypePtr p = std::make_shared<SetAreasType>();
        pMap[name] = p;
        pOptions[name].reset(name);
        return p;
    }

    TypePtr add(const IDType& name, const TypePtr& data)
    {
        pMap[name] = data;
        pOptions[name].reset(name);
        return data;
    }

    TypePtr add(const IDType& name, const TypePtr& data, Options& opts)
    {
        pMap[name] = data;
        pOptions[name] = opts;
        return data;
    }

    bool forceReload(bool /*reload*/) const
    {
        pModified = true;
        return true;
    }

    void markAsModified() const
    {
        pModified = true;
    }

    uint size() const;

    /*!
    ** \brief Get if the results for a given group should be written to the output
    */
    bool hasOutput(const Yuni::ShortString128& s) const;

    /*!
    ** \brief Get if the results for a given group should be written to the output
    */
    bool hasOutput(const uint index) const;

    /*!
    ** \brief Get the size of a result set
    */
    uint resultSize(const Yuni::ShortString128& s) const;

    /*!
    ** \brief Get the size of a result set
    */
    uint resultSize(const uint index) const;

    void dumpToLogs() const;

    /*!
    ** \brief Load a rule set from an INI File
    */
    bool loadFromFile(const std::filesystem::path& filename);

    bool saveToFile(const Yuni::String& filename) const;
    /*!
    ** \brief format the string to match the options
    */
    YString toString();

    /*!
    ** \brief Create default groups for set of areas
    */
    void defaultForAreas();

    /*!
    ** \brief Rebuild the lists of all group from the rules
    */
    void rebuildAllFromRules(SetHandlerAreas& handler);

    const IDType& nameByIndex(const uint i) const
    {
        assert(i < pMap.size() && "Sets: operator[] index out of bounds");
        return pNameByIndex[i];
    }

    IDType caption(const Yuni::ShortString128& s) const;
    IDType caption(const uint i) const;

    SetAreasType& operator[](uint i);
    const SetAreasType& operator[](uint i) const;

private:
    /*!
    ** \brief Rebuild the lists of a group from the rules
    */
    void rebuildFromRules(const IDType& id, SetHandlerAreas& handler);
    void rebuildIndexes();

    //! All groups
    MapType pMap;
    MapOptions pOptions;
    //!
    std::vector<TypePtr> pByIndex;
    std::vector<IDType> pNameByIndex;
    mutable bool pModified = false;
}; // class Sets

class SetHandlerAreas
{
public:
    explicit SetHandlerAreas(AreaList& areas);
    void clear(Sets::SetAreasType& set);
    uint size(Sets::SetAreasType& set);

    bool add(Sets::SetAreasType& set, const std::string& value);
    void add(Sets::SetAreasType& set, const Sets::SetAreasType& otherSet);
    bool remove(Sets::SetAreasType& set, const std::string& value);
    void remove(Sets::SetAreasType& set, const Sets::SetAreasType& otherSet);
    bool applyFilter(Sets::SetAreasType& set, const std::string& value);

private:
    AreaList& areas_;
}; // class SetHandlerAreas

} // namespace Antares::Data
