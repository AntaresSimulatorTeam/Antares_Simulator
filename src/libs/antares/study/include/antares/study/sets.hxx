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
#ifndef __ANTARES_LIBS_STUDY_SETS_HXX__
#define __ANTARES_LIBS_STUDY_SETS_HXX__

namespace Antares::Data
{

template<class L>
void Sets::dumpToLogs(L& log) const
{
    using namespace Yuni;
    const typename MapType::const_iterator end = pMap.end();
    for (typename MapType::const_iterator i = pMap.begin(); i != end; ++i)
    {
        log.info() << "   found `" << i->first << "` (" << (uint)i->second->size() << ' '
                   << (i->second->size() < 2 ? "item" : "items")
                   << ((!hasOutput(i->first)) ? ", no output" : "") << ')';
    }
}

template<class StringT>
bool Sets::saveToFile(const StringT& filename) const
{
    using namespace Yuni;
    using namespace Antares;

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

template<class HandlerT>
inline void Sets::rebuildAllFromRules(HandlerT& handler)
{
    for (uint i = 0; i != pMap.size(); ++i)
    {
        rebuildFromRules(pNameByIndex[i], handler);
    }
}

template<class HandlerT>
void Sets::rebuildFromRules(const IDType& id, HandlerT& handler)
{
    using namespace Yuni;
    using namespace Antares;

    typename MapOptions::iterator i = pOptions.find(id);
    if (i == pOptions.end())
    {
        return;
    }
    // Options
    Options& opts = i->second;
    auto& set = *(pMap[id]);

    // Clear the result first
    handler.clear(set);
    // Apply all rules
    for (uint i = 0; i != opts.rules.size(); ++i)
    {
        const Rule& rule = opts.rules[i];
        const Yuni::String& arg = *(rule.second);
        switch (rule.first) // type
        {
        case ruleAdd:
        {
            // Trying to add a single item
            if (!handler.add(set, arg))
            {
                // Failed. Maybe the argument references another group
                const IDType other = arg;
                typename MapType::iterator i = pMap.find(other);
                if (i != pMap.end())
                {
                    if (handler.add(set, *(i->second)))
                    {
                        break;
                    }
                }
            }
            break;
        }
        case ruleRemove:
        {
            // Trying to remove a single item
            if (!handler.remove(set, arg))
            {
                // Failed. Maybe the argument references another group
                const IDType other = arg;
                typename MapType::iterator i = pMap.find(other);
                if (i != pMap.end())
                {
                    if (handler.remove(set, *(i->second)))
                    {
                        break;
                    }
                }
            }
            break;
        }
        case ruleFilter:
        {
            handler.applyFilter(set, arg);
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

template<class StringT>
inline bool Sets::hasOutput(const StringT& s) const
{
    // Assert, if a C* container can not be found at compile time
    static_assert(Yuni::Traits::CString<StringT>::valid);

    typename MapOptions::const_iterator i = pOptions.find(s);
    return (i != pOptions.end()) ? i->second.output : false;
}

template<class StringT>
inline uint Sets::resultSize(const StringT& s) const
{
    // Assert, if a C* container can not be found at compile time
    static_assert(Yuni::Traits::CString<StringT>::valid);

    typename MapOptions::const_iterator i = pOptions.find(s);
    return (i != pOptions.end()) ? i->second.resultSize : 0;
}

template<class StringT>
inline typename Sets::IDType Sets::caption(const StringT& s) const
{
    // Assert, if a C* container can not be found at compile time
    static_assert(Yuni::Traits::CString<StringT>::valid);

    typename MapOptions::const_iterator i = pOptions.find(s);
    return (i != pOptions.end()) ? i->second.caption : IDType();
}
} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_SETS_HXX__
