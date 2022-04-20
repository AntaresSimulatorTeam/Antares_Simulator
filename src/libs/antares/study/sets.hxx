/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_SETS_HXX__
#define __ANTARES_LIBS_STUDY_SETS_HXX__

namespace Antares
{
namespace Data
{
template<class T>
inline Sets<T>::Sets() : pByIndex(NULL), pNameByIndex(NULL), pModified(false)
{
}

template<class T>
inline Sets<T>::Sets(const Sets& rhs) :
 pMap(rhs.pMap), pOptions(rhs.pOptions), pByIndex(NULL), pNameByIndex(NULL), pModified(false)
{
    if (rhs.pByIndex)
        rebuildIndexes();
}

template<class T>
inline Sets<T>::~Sets()
{
    delete[] pByIndex;
}

template<class T>
typename Sets<T>::iterator Sets<T>::begin()
{
    return pMap.begin();
}

template<class T>
typename Sets<T>::const_iterator Sets<T>::begin() const
{
    return pMap.begin();
}

template<class T>
typename Sets<T>::iterator Sets<T>::end()
{
    return pMap.end();
}

template<class T>
typename Sets<T>::const_iterator Sets<T>::end() const
{
    return pMap.end();
}

template<class T>
void Sets<T>::clear()
{
    if (pByIndex)
    {
        delete[] pByIndex;
        pByIndex = NULL;
    }
    if (pNameByIndex)
    {
        delete[] pNameByIndex;
        pNameByIndex = NULL;
    }

    pMap.clear();
    pOptions.clear();
}

template<class T>
inline T& Sets<T>::operator[](uint i)
{
    assert(i < pMap.size() && "Sets: operator[] index out of bounds");
    return *(pByIndex[i]);
}

template<class T>
inline const T& Sets<T>::operator[](uint i) const
{
    assert(i < pMap.size() && "Sets: operator[] index out of bounds");
    return *(pByIndex[i]);
}

template<class T>
template<class L>
void Sets<T>::dumpToLogs(L& log) const
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

template<class T>
void Sets<T>::defaultForAreas()
{
    using namespace Yuni;
    clear();
    Options opts;
    opts.caption = "All areas";
    opts.comments = "Spatial aggregates on all areas";
    opts.output = false;
    opts.rules.push_back(Rule(ruleFilter, new String("add-all")));
    auto item = std::make_shared<T>();
    add("all areas", item, opts);
}

template<class T>
YString Sets<T>::toString()
{
    using namespace Yuni;
    using namespace Antares;
    static const char* cmds[ruleMax] = {"none", "+", "-", "apply-filter"};
    const auto end = pOptions.cend();
    YString ret = "";
    for (auto i = pOptions.cbegin(); i != end; ++i)
    {
        const Options& opts = i->second;
        ret << '[' << i->first << "]\n";
        ret << "caption = " << opts.caption << '\n';
        if (not opts.comments.empty())
            ret << "comments = " << opts.comments << '\n';
        if (!opts.output)
            ret << "output = false\n";

        for (uint r = 0; r != opts.rules.size(); ++r)
        {
            const Rule& rule = opts.rules[r];
            ret << cmds[rule.first] << " = " << rule.second << '\n';
        }
        ret << '\n';
    }
    return ret;
}

template<class T>
template<class StringT>
bool Sets<T>::saveToFile(const StringT& filename) const
{
    using namespace Yuni;
    using namespace Antares;

    IO::File::Stream file;
    if (!file.open(filename, IO::OpenMode::write | IO::OpenMode::truncate))
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
            file << "comments = " << opts.comments << '\n';
        if (!opts.output)
            file << "output = false\n";

        for (uint r = 0; r != opts.rules.size(); ++r)
        {
            const Rule& rule = opts.rules[r];
            file << cmds[rule.first] << " = " << rule.second << '\n';
        }
        file << '\n';
    }
    return true;
}

template<class T>
template<class StringT>
bool Sets<T>::loadFromFile(const StringT& filename)
{
    using namespace Yuni;
    using namespace Antares;

    // Empty the container first
    clear();

    // Loading the INI file
    if (!IO::File::Exists(filename))
        // Error silently ignored
        return true;

    IniFile ini;
    if (ini.open(filename))
    {
        Yuni::String value;

        // each section...
        for (auto* section = ini.firstSection; section != nullptr; section = section->next)
        {
            // Clearing the name.
            if (!section->name)
                continue;

            // Creating a new section
            auto item = std::make_shared<T>();
            Options opts;
            opts.caption = section->name;

            // each property...
            const IniFile::Property* p;
            for (p = section->firstProperty; p != nullptr; p = p->next)
            {
                if (p->key.empty())
                    continue;

                value = p->value;
                value.toLower();

                if (p->key == "+")
                {
                    opts.rules.push_back(Rule(ruleAdd, new String(value)));
                    continue;
                }
                if (p->key == "-")
                {
                    opts.rules.push_back(Rule(ruleRemove, new String(value)));
                    continue;
                }
                if (p->key == "apply-filter")
                {
                    opts.rules.push_back(Rule(ruleFilter, new String(value)));
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
            add(newid, item, opts);
        }

        // Not modified anymore
        pModified = false;
        // All indexes must be rebuilt
        rebuildIndexes();
        return true;
    }
    return false;
}

template<class T>
template<class HandlerT>
inline void Sets<T>::rebuildAllFromRules(HandlerT& handler)
{
    for (uint i = 0; i != pMap.size(); ++i)
        rebuildFromRules(pNameByIndex[i], handler);
}

template<class T>
template<class HandlerT>
void Sets<T>::rebuildFromRules(const IDType& id, HandlerT& handler)
{
    using namespace Yuni;
    using namespace Antares;

    typename MapOptions::iterator i = pOptions.find(id);
    if (i == pOptions.end())
        return;
    // Options
    Options& opts = i->second;
    Type& set = *(pMap[id]);
    assert(&set != NULL);

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
                        break;
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
                        break;
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

template<class T>
void Sets<T>::rebuildIndexes()
{
    delete[] pByIndex;
    delete[] pNameByIndex;

    if (!pMap.empty())
    {
        pByIndex = new TypePtr[pMap.size()];
        pNameByIndex = new IDType[pMap.size()];
        const typename MapType::iterator end = pMap.end();
        uint index = 0;
        for (typename MapType::iterator i = pMap.begin(); i != end; ++i)
        {
            pByIndex[index] = i->second;
            pNameByIndex[index] = i->first;
            ++index;
        }
    }
    else
    {
        pByIndex = NULL;
        pNameByIndex = NULL;
    }
}

template<class T>
template<class StringT>
inline bool Sets<T>::hasOutput(const StringT& s) const
{
    using namespace Yuni;
    // Assert, if a C* container can not be found at compile time
    YUNI_STATIC_ASSERT(Traits::CString<StringT>::valid, HasOutput_InvalidTypeForBuffer);

    typename MapOptions::const_iterator i = pOptions.find(s);
    return (i != pOptions.end()) ? i->second.output : false;
}

template<class T>
inline bool Sets<T>::hasOutput(const uint index) const
{
    return hasOutput(IDType(pNameByIndex[index]));
}

template<class T>
template<class StringT>
inline uint Sets<T>::resultSize(const StringT& s) const
{
    using namespace Yuni;
    // Assert, if a C* container can not be found at compile time
    YUNI_STATIC_ASSERT(Traits::CString<StringT>::valid, HasOutput_InvalidTypeForBuffer);

    typename MapOptions::const_iterator i = pOptions.find(s);
    return (i != pOptions.end()) ? i->second.resultSize : 0;
}

template<class T>
template<class StringT>
inline typename Sets<T>::IDType Sets<T>::caption(const StringT& s) const
{
    using namespace Yuni;
    // Assert, if a C* container can not be found at compile time
    YUNI_STATIC_ASSERT(Traits::CString<StringT>::valid, HasOutput_InvalidTypeForBuffer);

    typename MapOptions::const_iterator i = pOptions.find(s);
    return (i != pOptions.end()) ? i->second.caption : IDType();
}

template<class T>
inline typename Sets<T>::IDType Sets<T>::caption(const uint i) const
{
    return caption(IDType(pNameByIndex[i]));
}

template<class T>
inline uint Sets<T>::resultSize(const uint index) const
{
    return resultSize(IDType(pNameByIndex[index]));
}

template<class T>
inline uint Sets<T>::size() const
{
    return (uint)pMap.size();
}

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_SETS_HXX__
