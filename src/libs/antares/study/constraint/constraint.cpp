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
#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include "constraint.h"
#include "../study.h"
#include "../../logs.h"
#include "../../utils.h"
#include "../runtime.h"
#include "../memory-usage.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

namespace Antares
{
namespace Data
{
bool compareConstraints(const BindingConstraint* s1, const BindingConstraint* s2)
{
    return ((s1->name()) < (s2->name()));
}

BindingConstraint::Operator BindingConstraint::StringToOperator(const AnyString& text)
{
    ShortString16 l(text);
    l.toLower();

    if (l == "both" or l == "<>" or l == "><" or l == "< and >")
        return opBoth;
    if (l == "less" or l == "<" or l == "<=")
        return opLess;
    if (l == "greater" or l == ">" or l == ">=")
        return opGreater;
    if (l == "equal" or l == "=" or l == "==")
        return opEquality;
    return opUnknown;
}

BindingConstraint::Type BindingConstraint::StringToType(const AnyString& text)
{
    if (not text.empty())
    {
        ShortString16 l(text);
        l.toLower();
        switch (l.first())
        {
        case 'h':
        {
            if (l == "hourly" or l == "hour" or l == "h")
                return typeHourly;
            break;
        }
        case 'd':
        {
            if (l == "daily" or l == "day" or l == "d")
                return typeDaily;
            break;
        }
        case 'w':
        {
            if (l == "weekly" or l == "week" or l == "w")
                return typeWeekly;
            break;
        }
        }
    }
    return typeUnknown;
}

const char* BindingConstraint::TypeToCString(const BindingConstraint::Type type)
{
    static const char* const names[typeMax + 1] = {"", "hourly", "daily", "weekly", ""};
    assert((uint)type < (uint)(typeMax + 1));
    return names[type];
}

const char* BindingConstraint::OperatorToCString(BindingConstraint::Operator o)
{
    static const char* const names[opMax + 1] = {"", "equal", "less", "greater", "both", ""};
    assert((uint)o < (uint)(opMax + 1));
    return names[o];
}

const char* BindingConstraint::OperatorToShortCString(BindingConstraint::Operator o)
{
    static const char* const names[opMax + 1]
      = {"", "equality", "bounded above", "bounded below", "bounded on both sides", ""};
    assert((uint)o < (uint)(opMax + 1));
    return names[o];
}

const char* BindingConstraint::MathOperatorToCString(BindingConstraint::Operator o)
{
    static const char* const names[opMax + 1] = {"", "=", "<", ">", "< and >", ""};
    assert((uint)o < (uint)(opMax + 1));
    return names[o];
}

BindingConstraint::BindingConstraint() : pEnabled(false)
{
}

BindingConstraint::~BindingConstraint()
{
#ifndef NDEBUG
    pName = "<INVALID>";
    pID = "<INVALID>";
#endif
}

void BindingConstraint::name(const AnyString& newname)
{
    pName = newname;
    pID.clear();
    Antares::TransformNameIntoID(pName, pID);
}

void BindingConstraint::weight(const AreaLink* lnk, double w)
{
    if (lnk)
    {
        if (Math::Zero(w))
        {
            auto i = pLinkWeights.find(lnk);
            if (i != pLinkWeights.end())
                pLinkWeights.erase(i);
        }
        else
            pLinkWeights[lnk] = w;
    }
}

void BindingConstraint::weight(const ThermalCluster* clstr, double w)
{
    if (clstr)
    {
        if (Math::Zero(w))
        {
            auto i = pClusterWeights.find(clstr);
            if (i != pClusterWeights.end())
                pClusterWeights.erase(i);
        }
        else
            pClusterWeights[clstr] = w;
    }
}

void BindingConstraint::removeAllWeights()
{
    pLinkWeights.clear();
    pClusterWeights.clear();
}

void BindingConstraint::offset(const AreaLink* lnk, int o)
{
    if (lnk)
    {
        if (Math::Zero(o))
        {
            auto i = pLinkOffsets.find(lnk);
            if (i != pLinkOffsets.end())
                pLinkOffsets.erase(i);
        }
        else
            pLinkOffsets[lnk] = o;
    }
}

void BindingConstraint::offset(const ThermalCluster* clstr, int o)
{
    if (clstr)
    {
        if (Math::Zero(o))
        {
            auto i = pClusterOffsets.find(clstr);
            if (i != pClusterOffsets.end())
                pClusterOffsets.erase(i);
        }
        else
            pClusterOffsets[clstr] = o;
    }
}

void BindingConstraint::removeAllOffsets()
{
    pLinkOffsets.clear();
    pClusterOffsets.clear();
}

uint Antares::Data::BindingConstraint::enabledClusterCount() const
{
    return std::count_if(
      pClusterWeights.begin(), pClusterWeights.end(), [](const clusterWeightMap::value_type& i) {
          return i.first->enabled && !i.first->mustrun;
      });
}

bool BindingConstraint::removeLink(const AreaLink* lnk)
{
    auto iw = pLinkWeights.find(lnk);
    if (iw != pLinkWeights.end())
    {
        pLinkWeights.erase(iw);
        return true;
    }

    auto io = pLinkOffsets.find(lnk);
    if (io != pLinkOffsets.end())
    {
        pLinkOffsets.erase(io);
        return true;
    }
    return false;
}

bool BindingConstraint::removeCluster(const ThermalCluster* clstr)
{
    auto iw = pClusterWeights.find(clstr);
    if (iw != pClusterWeights.end())
    {
        pClusterWeights.erase(iw);
        return true;
    }

    auto io = pClusterOffsets.find(clstr);
    if (io != pClusterOffsets.end())
    {
        pClusterOffsets.erase(io);
        return true;
    }
    return false;
}

void BindingConstraint::resetToDefaultValues()
{
    pEnabled = true;
    pComments.clear();
    pValues.zero();
    pValues.markAsModified();
    pValues.flush();
}

void BindingConstraint::copyWeights(const Study& study,
                                    const BindingConstraint& rhs,
                                    bool emptyBefore)
{
    if (emptyBefore)
    {
        pLinkWeights.clear();
        pClusterWeights.clear();
    }

    if (not rhs.pLinkWeights.empty())
    {
        auto end = rhs.pLinkWeights.end();
        for (auto i = rhs.pLinkWeights.begin(); i != end; ++i)
        {
            // Alias to the current link
            const AreaLink* sourceLink = i->first;
            // weight
            const double weight = i->second;

            assert(sourceLink and "Invalid link in binding constraint");
            assert(sourceLink->from and "Invalid area pointer 'from' within link");
            assert(sourceLink->with and "Invalid area pointer 'with' within link");
            const AreaLink* localLink
              = study.areas.findLink(sourceLink->from->id, sourceLink->with->id);
            if (localLink)
                pLinkWeights[localLink] = weight;
        }
    }

    if (not rhs.pClusterWeights.empty())
    {
        auto end = rhs.pClusterWeights.end();
        for (auto i = rhs.pClusterWeights.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster* thermalCluster = i->first;
            // weight
            const double weight = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            const Area* localParent = study.areas.findFromName(thermalCluster->parentArea->name);
            if (localParent)
            {
                const ThermalCluster* localTC
                  = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterWeights[localTC] = weight;
            }
        }
    }
}

void BindingConstraint::copyWeights(const Study& study,
                                    const BindingConstraint& rhs,
                                    bool emptyBefore,
                                    Yuni::Bind<void(AreaName&, const AreaName&)>& translate)
{
    if (emptyBefore)
    {
        pLinkWeights.clear();
        pClusterWeights.clear();
    }

    if (rhs.pLinkWeights.empty())
        return;

    AreaName fromID;
    AreaName withID;

    auto end = rhs.pLinkWeights.end();
    for (auto i = rhs.pLinkWeights.begin(); i != end; ++i)
    {
        // Alias to the current link
        const AreaLink* sourceLink = i->first;
        // weight
        const double weight = i->second;

        assert(sourceLink and "Invalid link in binding constraint");
        assert(sourceLink->from and "Invalid area pointer 'from' within link");
        assert(sourceLink->with and "Invalid area pointer 'with' within link");

        // Translatiob
        translate(fromID, sourceLink->from->id);
        translate(withID, sourceLink->with->id);

        const AreaLink* localLink = study.areas.findLink(fromID, withID);
        if (localLink)
            pLinkWeights[localLink] = weight;
    }

    if (not rhs.pClusterWeights.empty())
    {
        auto end = rhs.pClusterWeights.end();
        for (auto i = rhs.pClusterWeights.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster* thermalCluster = i->first;
            // weight
            const double weight = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            AreaName parentID;
            translate(parentID, thermalCluster->parentArea->id);

            const Area* localParent = study.areas.find(parentID);
            if (localParent)
            {
                const ThermalCluster* localTC
                  = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterWeights[localTC] = weight;
            }
        }
    }
}

void BindingConstraint::copyOffsets(const Study& study,
                                    const BindingConstraint& rhs,
                                    bool emptyBefore)
{
    if (emptyBefore)
    {
        pLinkOffsets.clear();
        pClusterOffsets.clear();
    }

    if (not rhs.pLinkOffsets.empty())
    {
        auto end = rhs.pLinkOffsets.end();
        for (auto i = rhs.pLinkOffsets.begin(); i != end; ++i)
        {
            // Alias to the current link
            const AreaLink* sourceLink = i->first;
            // offset
            const int offset = i->second;

            assert(sourceLink and "Invalid link in binding constraint");
            assert(sourceLink->from and "Invalid area pointer 'from' within link");
            assert(sourceLink->with and "Invalid area pointer 'with' within link");
            const AreaLink* localLink
              = study.areas.findLink(sourceLink->from->id, sourceLink->with->id);
            if (localLink)
                pLinkOffsets[localLink] = offset;
        }
    }

    if (not rhs.pClusterOffsets.empty())
    {
        auto end = rhs.pClusterOffsets.end();
        for (auto i = rhs.pClusterOffsets.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster* thermalCluster = i->first;
            // weight
            const int offset = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            const Area* localParent = study.areas.findFromName(thermalCluster->parentArea->name);
            if (localParent)
            {
                const ThermalCluster* localTC
                  = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterOffsets[localTC] = offset;
            }
        }
    }
}

void BindingConstraint::copyOffsets(const Study& study,
                                    const BindingConstraint& rhs,
                                    bool emptyBefore,
                                    Yuni::Bind<void(AreaName&, const AreaName&)>& translate)
{
    if (emptyBefore)
        pLinkOffsets.clear();
    if (rhs.pLinkOffsets.empty())
        return;

    AreaName fromID;
    AreaName withID;

    auto end = rhs.pLinkOffsets.end();
    for (auto i = rhs.pLinkOffsets.begin(); i != end; ++i)
    {
        // Alias to the current link
        const AreaLink* sourceLink = i->first;
        // offset
        const int offset = i->second;

        assert(sourceLink and "Invalid link in binding constraint");
        assert(sourceLink->from and "Invalid area pointer 'from' within link");
        assert(sourceLink->with and "Invalid area pointer 'with' within link");

        // Translatiob
        translate(fromID, sourceLink->from->id);
        translate(withID, sourceLink->with->id);

        const AreaLink* localLink = study.areas.findLink(fromID, withID);
        if (localLink)
            pLinkOffsets[localLink] = offset;
    }

    if (not rhs.pClusterOffsets.empty())
    {
        auto end = rhs.pClusterOffsets.end();
        for (auto i = rhs.pClusterOffsets.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster* thermalCluster = i->first;
            // weight
            const int offset = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            AreaName parentID;
            translate(parentID, thermalCluster->parentArea->id);

            const Area* localParent = study.areas.find(parentID);
            if (localParent)
            {
                const ThermalCluster* localTC
                  = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterOffsets[localTC] = offset;
            }
        }
    }
}

bool BindingConstraint::loadFromEnv(BindingConstraint::EnvForLoading& env)
{
    // Name / ID
    pName.clear();
    pID.clear();
    // No comments
    pComments.clear();
    // The type must be `hourly` by default for studies <=3.1, which was the only
    // type of binding constraints supported.
    pType = typeUnknown;
    // The operator is `<` by default, which was the only option for studies <= 3.1
    pOperator = opUnknown;
    // Enabled: True by default to automatically allow the use of bindingconstraint
    // from old studies (<= 3.1)
    pEnabled = true;

    // Reset
    bool ret = true;

    // Foreach property in the section...
    for (const IniFile::Property* p = env.section->firstProperty; p; p = p->next)
    {
        if (p->key.empty())
            continue;

        if (env.version > 310)
        {
            if (p->key == "name")
            {
                pName = p->value;
                continue;
            }
            if (p->key == "id")
            {
                pID = p->value;
                pID.toLower(); // force the lowercase
                continue;
            }
            if (p->key == "enabled")
            {
                pEnabled = p->value.to<bool>();
                continue;
            }
            if (p->key == "type")
            {
                pType = BindingConstraint::StringToType(p->value);
                continue;
            }
            if (p->key == "operator")
            {
                pOperator = BindingConstraint::StringToOperator(p->value);
                continue;
            }
            if (p->key == "comments")
            {
                pComments = p->value;
                continue;
            }
        }
        else
        {
            // All versions <= 3.1
            if (p->key == "name")
            {
                // For studies <=3.1, the ID is the name of the binding constraint but
                // in lowercase
                pName = p->value;
                pID = pName;
                pID.toLower();
                continue;
            }
            if (p->key == "type")
            {
                // For all studies <3.2, the field `type` is invalid, but it must be present
                // For compatibility, we have to force its value to `typeHourly`, since it
                // was the only type of binding constraint available for those studies.
                pType = typeHourly;
                // Only one operator was available for those studies. Since the field `type`
                // must be present, we can assign the operator here.
                pOperator = opLess;
                continue;
            }
        }

        // It may be a link
        // Separate the key
        String::Size setKey = p->key.find('%');

        // initialize the values
        double w = .0;
        int o = 0;

        // Separate the value
        if (setKey != 0 && setKey != String::npos) // It is a link
        {
            CString<64> stringWO = p->value;
            String::Size setVal = p->value.find('%');
            uint occurence = 0;
            bool ret = true;
            stringWO.words("%", [&](const CString<64>& part) -> bool {
                if (occurence == 0)
                {
                    if (setVal == 0) // weight is null
                    {
                        if (not part.to<int>(o))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid offset";
                            ret = false;
                        }
                    }
                    else // weight is not null
                    {
                        if (not part.to<double>(w))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid weight";
                            ret = false;
                        }
                    }
                }

                if (occurence == 1 && setVal != 0)
                {
                    if (not part.to<int>(o))
                    {
                        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `"
                                     << p->key << "`: invalid offset";
                        ret = false;
                    }
                }

                ++occurence;
                return ret; // continue to iterate
            });

            if (not ret)
                continue;

            const AreaLink* lnk = env.areaList.findLinkFromINIKey(p->key);
            if (!lnk)
            {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: link not found";
                continue;
            }
            if (not Math::Zero(w))
                this->weight(lnk, w);

            if (not Math::Zero(o))
                this->offset(lnk, o);

            continue;
        }
        else // It must be a cluster
        {
            // Separate the key
            String::Size setKey = p->key.find('.');
            if (0 == setKey or setKey == String::npos)
            {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: invalid key";
                continue;
            }

            CString<64> stringWO = p->value;
            String::Size setVal = p->value.find('%');
            uint occurence = 0;
            bool ret = true;
            stringWO.words("%", [&](const CString<64>& part) -> bool {
                if (occurence == 0)
                {
                    if (setVal == 0) // weight is null
                    {
                        if (not part.to<int>(o))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid offset";
                            ret = false;
                        }
                    }
                    else // weight is not null
                    {
                        if (not part.to<double>(w))
                        {
                            logs.error() << env.iniFilename << ": in [" << env.section->name
                                         << "]: `" << p->key << "`: invalid weight";
                            ret = false;
                        }
                    }
                }

                if (occurence == 1 && setVal != 0)
                {
                    if (not part.to<int>(o))
                    {
                        logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `"
                                     << p->key << "`: invalid offset";
                        ret = false;
                    }
                }

                ++occurence;
                return ret; // continue to iterate
            });

            if (not ret)
                continue;

            const ThermalCluster* clstr = env.areaList.findClusterFromINIKey(p->key);
            if (!clstr)
            {
                logs.error() << env.iniFilename << ": in [" << env.section->name << "]: `" << p->key
                             << "`: cluster not found";
                continue;
            }
            if (not Math::Zero(w))
                this->weight(clstr, w);

            if (not Math::Zero(o))
                this->offset(clstr, o);

            continue;
        }
    }

    // Checking for validity
    if (!ret or !pName or !pID or pOperator == opUnknown or pType == typeUnknown)
    {
        // Reporting the error into the logs
        if (!pName)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid binding constraint name";
        if (!pID)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid binding constraint id";
        if (pType == typeUnknown)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid type [hourly,daily,weekly]";
        if (pOperator == opUnknown)
            logs.error() << env.iniFilename << ": in [" << env.section->name
                         << "]: Invalid operator [less,greater,equal,both]";

        // Invalid binding constraint
        return false;
    }

    // The binding constraint can not be enabled if there is no weight in the table
    if (pLinkWeights.empty() && pClusterWeights.empty())
        pEnabled = false;

    if (env.version > 310)
    {
        // Values
        env.buffer.clear() << env.folder << SEP << pID << ".txt";
        if (pValues.loadFromCSVFile(env.buffer,
                                    columnMax,
                                    (pType == typeHourly) ? 8784 : 366,
                                    Matrix<>::optImmediate | Matrix<>::optFixedSize,
                                    &env.matrixBuffer))
        {
            if (pComments.empty())
            {
                logs.info() << " added `" << pName << "` (" << TypeToCString(pType) << ", "
                            << OperatorToShortCString(pOperator) << ')';
            }
            else
            {
                logs.info() << " added `" << pName << "` (" << TypeToCString(pType) << ", "
                            << OperatorToShortCString(pOperator) << ") " << pComments;
            }
            return true;
        }
    }
    else // studies <= 3.1
    {
        // For studies <=3.1, only a mere vector was available for the second member
        // Resetting the matrix
        pValues.reset(columnMax, 8784, true);
        env.buffer.clear() << env.folder << SEP << "boundvalues_" << pID
                           << (env.version < 310 ? ".csv" : ".txt");
        ::Matrix<> m;
        if (m.loadFromCSVFile(env.buffer,
                              1,
                              8760,
                              Matrix<>::optImmediate | Matrix<>::optFixedSize,
                              &env.matrixBuffer))
        {
            // Copying values
            const uint h = (m.height <= 8760) ? m.height : 8760;
            (void)memcpy(pValues.entry[columnInferior], m.entry[0], sizeof(double) * h);
            logs.info() << " added `" << pName << "` (" << TypeToCString(pType) << ", "
                        << OperatorToShortCString(pOperator) << ')';
            pValues.flush();
            return true;
        }
    }
    return false;
}

bool BindingConstraint::saveToEnv(BindingConstraint::EnvForSaving& env)
{
    env.section->add("name", pName);
    env.section->add("id", pID);
    env.section->add("enabled", pEnabled);
    env.section->add("type", TypeToCString(pType));
    env.section->add("operator", OperatorToCString(pOperator));

    if (not pComments.empty())
        env.section->add("comments", pComments);

    if (not pLinkWeights.empty())
    {
        auto end = pLinkWeights.end();
        for (auto i = pLinkWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid link");
            assert(i->first->from and "Invalid area name");
            assert(i->first->with and "Invalid area name");

            const AreaLink& lnk = *(i->first);
            env.key.clear() << lnk.from->id << '%' << lnk.with->id;
            String value;
            value << i->second;
            if (pLinkOffsets.find(i->first) != pLinkOffsets.end())
                value << '%' << pLinkOffsets[i->first];
            // env.section->add(env.key, i->second);
            env.section->add(env.key, value);
        }
    }

    if (not pClusterWeights.empty())
    {
        auto end = pClusterWeights.end();
        for (auto i = pClusterWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid thermal cluster");

            const ThermalCluster& clstr = *(i->first);
            env.key.clear() << clstr.getFullName();
            String value;
            value << i->second;
            if (pClusterOffsets.find(i->first) != pClusterOffsets.end())
                value << '%' << pClusterOffsets[i->first];
            // env.section->add(env.key, i->second);
            env.section->add(env.key, value);
        }
    }

    // Exporting the matrix
    env.matrixFilename.clear() << env.folder << SEP << pID << ".txt";
    return pValues.saveToCSVFile(env.matrixFilename);
}

BindConstList::BindConstList()
{
}

void BindConstList::clear()
{
    if (not pList.empty())
    {
        for (uint i = 0; i != pList.size(); ++i)
            delete pList[i];
        pList.clear();
    }
}

BindConstList::~BindConstList()
{
    // see clear()
    for (uint i = 0; i != pList.size(); ++i)
        delete pList[i];
}

bool BindConstList::loadFromFolder(Study& study,
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
        if (study.parameters.mode == stdmAdequacyDraft)
        {
            logs.info() << "  The constraints shall be ignored in adequacy-draft";
            return true;
        }
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

    auto* e = new BindingConstraint::EnvForLoading(study.areas, study.header.version);
    auto& env = *e;
    env.folder = folder;

    env.iniFilename << env.folder << SEP << "bindingconstraints.ini";
    IniFile ini;
    if (not ini.open(env.iniFilename))
    {
        delete e;
        return false;
    }

    // For each section
    if (ini.firstSection)
    {
        for (env.section = ini.firstSection; env.section; env.section = env.section->next)
        {
            if (env.section->firstProperty)
            {
                BindingConstraint* bc = new BindingConstraint();
                if (bc->loadFromEnv(env))
                    pList.push_back(bc);
                else
                    delete bc;
            }
        }
    }

    // Logs
    if (pList.empty())
        logs.info() << "No binding constraint found";
    else
    {
        std::sort(pList.begin(), pList.end(), compareConstraints);

        if (pList.size() == 1)
            logs.info() << "1 binding constraint found";
        else
            logs.info() << pList.size() << " binding constraints found";
    }

    delete e;

    // When ran from the solver and if the simplex is in `weekly` mode,
    // all weekly constraints will become daily ones.
    if (study.usedByTheSolver)
    {
        if (sorDay == study.parameters.simplexOptimizationRange)
            mutateWeeklyConstraintsIntoDailyOnes();
    }

    return true;
}

void BindConstList::mutateWeeklyConstraintsIntoDailyOnes()
{
    each([&](BindingConstraint& constraint) {
        if (constraint.type() == BindingConstraint::typeWeekly)
        {
            logs.info() << "  The type of the constraint '" << constraint.name()
                        << "' is now 'daily'";
            constraint.mutateTypeWithoutCheck(BindingConstraint::typeDaily);
        }
    });
}

bool BindConstList::internalSaveToFolder(BindingConstraint::EnvForSaving& env) const
{
    if (pList.empty())
    {
        logs.info() << "No binding constraint to export.";
        if (not IO::Directory::Create(env.folder))
            return false;
        // stripping the file
        env.folder << SEP << "bindingconstraints.ini";
        return IO::File::CreateEmptyFile(env.folder);
    }

    if (pList.size() == 1)
        logs.info() << "Exporting 1 binding constraint...";
    else
        logs.info() << "Exporting " << pList.size() << " binding constraints...";

    if (not IO::Directory::Create(env.folder))
        return false;

    IniFile ini;
    bool ret = true;
    uint index = 0;
    auto end = pList.end();
    ShortString64 text;

    for (auto i = pList.begin(); i != end; ++i, ++index)
    {
        text = index;
        env.section = ini.addSection(text);
        ret = (*i)->saveToEnv(env) and ret;
    }

    env.folder << SEP << "bindingconstraints.ini";
    return ini.save(env.folder) and ret;
}

void BindConstList::reverseWeightSign(const AreaLink* lnk)
{
    each([&](BindingConstraint& constraint) { constraint.reverseWeightSign(lnk); });
}

void BindConstList::reverseWeightSign(const ThermalCluster* clstr)
{
    each([&](BindingConstraint& constraint) { constraint.reverseWeightSign(clstr); });
}

void BindingConstraint::reverseWeightSign(const AreaLink* lnk)
{
    auto i = pLinkWeights.find(lnk);
    if (i != pLinkWeights.end())
    {
        i->second *= -1.;
        logs.info() << "Updated the binding constraint `" << pName << '`';
    }
}

void BindingConstraint::reverseWeightSign(const ThermalCluster* clstr)
{
    auto i = pClusterWeights.find(clstr);
    if (i != pClusterWeights.end())
    {
        i->second *= -1.;
        logs.info() << "Updated the binding constraint `" << pName << '`';
    }
}

uint64 BindConstList::memoryUsage() const
{
    uint64 m = sizeof(BindConstList);
    for (uint i = 0; i != pList.size(); ++i)
        m += pList[i]->memoryUsage();
    return m;
}

bool BindingConstraint::contains(const Area* area) const
{
    const linkWeightMap::const_iterator end = pLinkWeights.end();
    for (linkWeightMap::const_iterator i = pLinkWeights.begin(); i != end; ++i)
    {
        if ((i->first)->from == area or (i->first)->with == area)
            return true;
    }

    const clusterWeightMap::const_iterator tEnd = pClusterWeights.end();
    for (clusterWeightMap::const_iterator i = pClusterWeights.begin(); i != tEnd; ++i)
    {
        if ((i->first)->parentArea == area)
            return true;
    }

    return false;
}

namespace // anonymous
{
template<class T>
class RemovePredicate final
{
public:
    RemovePredicate(const T* u) : pItem(u)
    {
    }

    bool operator()(const BindingConstraint* bc) const
    {
        assert(bc);
        if (bc->contains(pItem))
        {
            logs.info() << "destroying the binding constraint " << bc->name();
            delete bc;
            return true;
        }
        return false;
    }

private:
    const T* pItem;
};

} // anonymous namespace

void BindConstList::remove(const Area* area)
{
    RemovePredicate<Area> predicate(area);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
}

void BindConstList::remove(const AreaLink* lnk)
{
    RemovePredicate<AreaLink> predicate(lnk);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
}

void BindConstList::remove(const BindingConstraint* bc)
{
    RemovePredicate<BindingConstraint> predicate(bc);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
}

void BindingConstraint::buildFormula(String& s) const
{
    char tmp[42];
    bool first = true;
    auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i)
    {
        if (!first)
            s << " + ";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);

        s << '(' << (const char*)tmp << " x " << (i->first)->getName();

        auto at = pLinkOffsets.find(i->first);
        if (at != pLinkOffsets.end())
        {
            int o = at->second;
            if (o > 0)
                s << " x (t + " << pLinkOffsets.find(i->first)->second << ')';
            if (o < 0)
                s << " x (t - " << Math::Abs(pLinkOffsets.find(i->first)->second) << ')';
        }

        s << ')';
        first = false;
    }

    auto tEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != tEnd; ++i)
    {
        if (!first)
            s << " + ";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);

        s << '(' << (const char*)tmp << " x " << (i->first)->getFullName();

        auto at = pClusterOffsets.find(i->first);
        if (at != pClusterOffsets.end())
        {
            int o = at->second;
            if (o > 0)
                s << " x (t + " << pClusterOffsets.find(i->first)->second << ')';
            if (o < 0)
                s << " x (t - " << Math::Abs(pClusterOffsets.find(i->first)->second) << ')';
        }

        if (not(i->first)->enabled || (i->first)->mustrun)
            s << " x N/A";

        s << ')';
        first = false;
    }
}

void BindingConstraint::buildHTMLFormula(String& s) const
{
    char tmp[42];
    s.clear();
    bool first = true;
    auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i)
    {
        if (!first)
            s << " <font color=\"black\">+</font> ";
        s << "<font color=\"#AAAAAA\">(</font><font color=\"#FF781E\">";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);
        s << (const char*)tmp;
        s << "</font><font color=\"#FF2222\">x</font> <font color=\"#4F5B81\">"
          << (i->first)->from->name << '.' << (i->first)->with->name
          << "</font><font color=\"#AAAAAA\">)</font>";
        first = false;
    }

    auto tEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != tEnd; ++i)
    {
        if (!first)
            s << " <font color=\"black\">+</font> ";
        s << "<font color=\"#AAAAAA\">(</font><font color=\"#FF781E\">";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);
        s << (const char*)tmp;
        s << "</font><font color=\"#FF2222\">x</font> <font color=\"#4F5B81\">"
          << (i->first)->name() << "</font><font color=\"#AAAAAA\">)</font>";
        first = false;
    }
}

BindConstList::iterator BindConstList::begin()
{
    return pList.begin();
}

BindConstList::const_iterator BindConstList::begin() const
{
    return pList.begin();
}

BindConstList::iterator BindConstList::end()
{
    return pList.end();
}

BindConstList::const_iterator BindConstList::end() const
{
    return pList.end();
}

Yuni::uint64 BindingConstraint::memoryUsage() const
{
    return sizeof(BindingConstraint)
           // comments
           + pComments.capacity()
           // Values
           + pValues.memoryUsage()
           // Estimation
           + pLinkWeights.size() * (sizeof(double) + 3 * sizeof(void*))
           // Estimation
           + pLinkOffsets.size() * (sizeof(int) + 3 * sizeof(void*))
           // Estimation
           + pClusterWeights.size() * (sizeof(double) + 3 * sizeof(void*))
           // Estimation
           + pClusterOffsets.size() * (sizeof(int) + 3 * sizeof(void*));
}

void BindConstList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    if (u.mode == stdmAdequacyDraft)
        return;
    // Disabled by the optimization preferences
    if (!u.study.parameters.include.constraints)
        return;

    // each constraint...
    for (uint i = 0; i != pList.size(); ++i)
    {
        auto& bc = *(pList[i]);
        u.requiredMemoryForInput += sizeof(void*) * 2;
        uint count = (bc.operatorType() == BindingConstraint::opBoth) ? 2 : 1;
        for (uint i = 0; i != count; ++i)
        {
            u.requiredMemoryForInput += sizeof(BindingConstraintRTI);
            u.requiredMemoryForInput += (sizeof(long) + sizeof(double)) * bc.linkCount();
            u.requiredMemoryForInput += (sizeof(long) + sizeof(double)) * bc.clusterCount();
            Matrix<>::EstimateMemoryUsage(u, 1, HOURS_PER_YEAR);
        }
    }
}

bool BindingConstraint::contains(const BindingConstraint* bc) const
{
    return (this == bc);
}

bool BindingConstraint::contains(const AreaLink* lnk) const
{
    const linkWeightMap::const_iterator i = pLinkWeights.find(lnk);
    return (i != pLinkWeights.end());
}

bool BindingConstraint::contains(const ThermalCluster* clstr) const
{
    const clusterWeightMap::const_iterator i = pClusterWeights.find(clstr);
    return (i != pClusterWeights.end());
}

void BindingConstraint::enabled(bool v)
{
    pEnabled = v;
}

void BindingConstraint::operatorType(BindingConstraint::Operator o)
{
    pOperator = o;
}

bool BindingConstraint::hasAllWeightedLinksOnLayer(size_t layerID)
{
    if (layerID == 0 || (linkCount() == 0 && clusterCount() == 0))
        return true;

    BindingConstraint::iterator endWeights = this->end();

    for (Data::BindingConstraint::iterator j = this->begin(); j != endWeights; ++j)
    {
        auto* areaLink = j->first;
        if (!areaLink)
            continue;

        if (!areaLink->isVisibleOnLayer(layerID) || j->second == 0)
        {
            return false;
        }
    }
    return true;
}

bool BindingConstraint::hasAllWeightedClustersOnLayer(size_t layerID)
{
    if (layerID == 0 || (linkCount() == 0 && clusterCount() == 0))
        return true;

    auto endWeights = pClusterWeights.end();

    for (auto j = pClusterWeights.begin(); j != endWeights; ++j)
    {
        auto* clstr = j->first;
        if (!clstr)
            continue;

        if (!clstr->isVisibleOnLayer(layerID) || j->second == 0)
        {
            return false;
        }
    }
    return true;
}

double BindingConstraint::weight(const AreaLink* lnk) const
{
    linkWeightMap::const_iterator i = pLinkWeights.find(lnk);
    return (i != pLinkWeights.end()) ? i->second : 0.;
}

double BindingConstraint::weight(const ThermalCluster* clstr) const
{
    clusterWeightMap::const_iterator i = pClusterWeights.find(clstr);
    return (i != pClusterWeights.end()) ? i->second : 0.;
}

int BindingConstraint::offset(const AreaLink* lnk) const
{
    linkOffsetMap::const_iterator i = pLinkOffsets.find(lnk);
    return (i != pLinkOffsets.end()) ? i->second : 0;
}

int BindingConstraint::offset(const ThermalCluster* lnk) const
{
    clusterOffsetMap::const_iterator i = pClusterOffsets.find(lnk);
    return (i != pClusterOffsets.end()) ? i->second : 0;
}

void BindingConstraint::initLinkArrays(double* w,
                                       double* cW,
                                       int* o,
                                       int* cO,
                                       long* linkIndex,
                                       long* clusterIndex,
                                       long* clustersAreaIndex) const
{
    assert(w and "Invalid weight pointer");

    uint off = 0;
    auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i, ++off)
    {
        linkIndex[off] = (i->first)->index;
        w[off] = i->second;

        auto offsetIt = pLinkOffsets.find(i->first);
        if (offsetIt != pLinkOffsets.end())
            o[off] = offsetIt->second;
        else
            o[off] = 0;
    }

    off = 0;
    auto cEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != cEnd; ++i)
    {
        if (i->first->enabled && !i->first->mustrun)
        {
            clusterIndex[off] = (i->first)->index;
            clustersAreaIndex[off] = (i->first)->parentArea->index;
            cW[off] = i->second;

            auto offsetIt = pClusterOffsets.find(i->first);
            if (offsetIt != pClusterOffsets.end())
                cO[off] = offsetIt->second;
            else
                cO[off] = 0;

            ++off;
        }
    }
}

bool BindingConstraint::invalidate(bool reload) const
{
    return pValues.invalidate(reload);
}

bool BindConstList::invalidate(bool reload) const
{
    if (not pList.empty())
    {
        bool ret = true;
        for (uint i = 0; i != pList.size(); ++i)
            ret = pList[i]->invalidate(reload) and ret;
        return ret;
    }
    return true;
}

void BindingConstraint::markAsModified() const
{
    pValues.markAsModified();
}

void BindConstList::markAsModified() const
{
    if (not pList.empty())
    {
        for (uint i = 0; i != pList.size(); ++i)
            pList[i]->markAsModified();
    }
}

void BindingConstraint::clearAndReset(const AnyString& name,
                                      BindingConstraint::Type newType,
                                      BindingConstraint::Operator op)
{
    // Name / ID
    pName = name;
    pID.clear();
    TransformNameIntoID(name, pID);
    // New type
    pType = newType;
    // Operator
    pOperator = op;
    // Resetting the weights
    pLinkWeights.clear();
    // Resetting the offsets
    pLinkOffsets.clear();
    // Resetting the weights
    pClusterWeights.clear();
    // Resetting the offsets
    pClusterOffsets.clear();

    switch (pType)
    {
    case typeUnknown:
    {
        pValues.reset(0, 0);
        logs.error() << "invalid type for " << name << " (got 'unknown')";
        assert(false);
        break;
    }
    case typeHourly:
    {
        pValues.reset(columnMax, 8784, true);
        break;
    }
    case typeDaily:
    {
        pValues.reset(columnMax, 366, true);
        break;
    }
    case typeWeekly:
    {
        pValues.reset(columnMax, 366);
        break;
    }
    case typeMax:
    {
        pValues.reset(0, 0);
        logs.error() << "invalid type for " << name;
        break;
    }
    }
    pValues.markAsModified();
    pValues.flush();
}

bool BindConstList::saveToFolder(const AnyString& folder) const
{
    auto* env = new BindingConstraint::EnvForSaving();
    env->folder = folder;
    bool r = internalSaveToFolder(*env);
    delete env;
    return r;
}

bool BindConstList::rename(BindingConstraint* bc, const AnyString& newname)
{
    // Copy of the name
    ConstraintName name;
    name = newname;
    if (name == bc->name())
        return true;
    ConstraintName id;
    Antares::TransformNameIntoID(name, id);
    if (NULL != find(id))
        return false;
    bc->name(name);
    JIT::Invalidate(bc->matrix().jit);
    return true;
}

BindingConstraint* BindConstList::find(const AnyString& id)
{
    for (uint i = 0; i != (uint)pList.size(); ++i)
    {
        if (pList[i]->id() == id)
            return pList[i];
    }
    return NULL;
}

const BindingConstraint* BindConstList::find(const AnyString& id) const
{
    for (uint i = 0; i != (uint)pList.size(); ++i)
    {
        if (pList[i]->id() == id)
            return pList[i];
    }
    return NULL;
}

BindingConstraint* BindConstList::findByName(const AnyString& name)
{
    for (uint i = 0; i != (uint)pList.size(); ++i)
    {
        if (pList[i]->name() == name)
            return pList[i];
    }
    return NULL;
}

const BindingConstraint* BindConstList::findByName(const AnyString& name) const
{
    for (uint i = 0; i != (uint)pList.size(); ++i)
    {
        if (pList[i]->name() == name)
            return pList[i];
    }
    return NULL;
}

void BindConstList::removeConstraintsWhoseNameConstains(const AnyString& filter)
{
    WhoseNameContains pred(filter);
    pList.erase(std::remove_if(pList.begin(), pList.end(), pred), pList.end());
}

BindingConstraint* BindConstList::add(const AnyString& name)
{
    auto* bc = new BindingConstraint();
    bc->name(name);
    pList.push_back(bc);
    std::sort(pList.begin(), pList.end(), compareConstraints);
    return bc;
}

void BindingConstraint::matrix(const double onevalue)
{
    pValues.fill(onevalue);
    pValues.markAsModified();
}

} // namespace Data
} // namespace Antares
