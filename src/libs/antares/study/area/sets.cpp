
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

typename Sets::iterator Sets::begin()
{
    return pMap.begin();
}

typename Sets::const_iterator Sets::begin() const
{
    return pMap.begin();
}

typename Sets::iterator Sets::end()
{
    return pMap.end();
}

typename Sets::const_iterator Sets::end() const
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

void Sets::defaultForAreas()
{
    using namespace Yuni;
    clear();
    Options opts;
    opts.caption = "All areas";
    opts.comments = "Spatial aggregates on all areas";
    opts.output = false;
    opts.rules.push_back(Rule(ruleFilter, new String("add-all")));
    auto item = std::make_shared<SetAreasType>();
    add("all areas", item, opts);
}

YString Sets::toString()
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
            auto item = std::make_shared<SetAreasType>();
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

void Sets::rebuildIndexes()
{
    pNameByIndex.clear();
    pByIndex.clear();

    if (!pMap.empty())
    {
        pByIndex.resize(pMap.size());
        pNameByIndex.resize(pMap.size());
        const typename MapType::iterator end = pMap.end();
        uint index = 0;
        for (typename MapType::iterator i = pMap.begin(); i != end; ++i)
        {
            pByIndex[index] = i->second;
            pNameByIndex[index] = i->first;
            ++index;
        }
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

uint Sets::size() const
{
    return (uint)pMap.size();
}
} // namespace Antares::Data