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
#include <cmath>
#include "cbuilder.h"
#include "grid.h"

using namespace Yuni;

//#define SEP Yuni::IO::Separator

namespace Antares
{
double linkInfo::getWeightWithImpedance() const
{
    return (1 + ((hasPShiftsEqual) ? 0 : 10) + 100 * nImpedanceChanges);
}

Yuni::String linkInfo::getName() const
{
    /*String s;
    s << ptr->from->name << "/" << ptr->with->name;
    return s;*/

    return ptr->getName();
}

template class Graph::Grid<areaInfo>;

CBuilder::CBuilder(Antares::Data::Study::Ptr study) :
 pPrefix(CB_PREFIX), pPrefixDelete(CB_PREFIX), pDelete(false), pStudy(study)
{
}

CBuilder::~CBuilder()
{
    // delete all the elements of pLink
    for (auto i = pLink.begin(); i != pLink.end(); i++)
    {
        delete *i;
    }
}

bool Antares::CBuilder::isCycleDriver(linkInfo* lnkI)
{
    std::string s1(lnkI->ptr->from->name.to<std::string>());
    std::string s2(lnkI->ptr->with->name.to<std::string>());
    return _grid.findDrivingEdgeFromNodeNames(s1, s2) == nullptr;
}

uint Antares::CBuilder::cycleCount(linkInfo* lnkI)
{
    uint n = 0;
    for (auto it = pMesh.begin(); it != pMesh.end(); it++)
        n += (uint)(std::count(it->begin(), it->end(), lnkI));

    return n;
}

bool CBuilder::update(bool applyCheckBox)
{
    buildAreaToLinkInfosMap();
    // Keep only enabled AC lines, remove disabled or DC lines
    Vector enabledACLines;
    pMesh.clear();

    // Update impedances from study file and compute impedance changes

    for (auto linkInfoIt = pLink.begin(); linkInfoIt != pLink.end(); linkInfoIt++)
    {
        auto linkInfo = *linkInfoIt;
        Data::AreaLink* link = linkInfo->ptr;

       // Try to open link data files (GUI only)
        if (link->parameters.jit)
        {
            YString dataFilename = link->parameters.jit->sourceFilename;
            if (!link->loadTimeSeries(*pStudy, dataFilename))
                return false;
        }
    }

    for (auto linkInfoIt = pLink.begin(); linkInfoIt != pLink.end(); linkInfoIt++)
    {
        auto linkInfo = *linkInfoIt;
        Data::AreaLink* link = linkInfo->ptr;

        if (applyCheckBox)
        {
            link->useLoopFlow = includeLoopFlow;

            link->usePST = includePhaseShift;
        }
        // set used to count the number of different impedances
        std::set<double> impedances;

        uint columnImpedance = (uint)Data::fhlImpedances;

        link->forceReload(true);
        // load the impedance
        // Can probably be improved (below) !!!
        linkInfo->nImpedanceChanges = 0;
        linkInfo->avgImpedance = link->parameters[columnImpedance][0];
        uint hour;
        for (uint x = 1; x < HOURS_PER_YEAR; x++)
        {
            hour = x - 1;
            if (link->parameters[columnImpedance][x] != link->parameters[columnImpedance][hour])
            {
                impedances.insert(link->parameters[columnImpedance][x]);
            }

            if (includeLoopFlow) // check validity of loopflow against NTC
            {
                for (uint tsIndex = 0; tsIndex < link->indirectCapacities.width; ++tsIndex)
                {
                    if ((-1.0 * link->indirectCapacities[tsIndex][hour]
                         > link->parameters[Data::fhlLoopFlow][hour])
                        || (link->directCapacities[tsIndex][hour]
                            < link->parameters[Data::fhlLoopFlow][hour]))
                    {
                        logs.error() << "Error on loop flow to NTC comparison validity at hour "
                                     << x << " for line " << linkInfo->getName();
                        return false;
                    }
                }
                if (checkNodalLoopFlow) // check validity of loop flow values (sum = 0 at node)
                {
                    double sum = 0.0;
                    for (auto lnk : areaToLinks[link->from])
                    {
                        sum += link->from == lnk->ptr->from
                                 ? -1 * lnk->ptr->parameters[Data::fhlLoopFlow][hour]
                                 : lnk->ptr->parameters[Data::fhlLoopFlow][hour];
                    }

                    if (sum != 0.0)
                    {
                        logs.error() << "Error on loop flow sum validity (!= 0) at hour " << x
                                     << " on node " << link->from->id;
                        return false;
                    }

                    sum = 0.0;
                    for (auto lnk : areaToLinks[link->with])
                    {
                        sum += link->with == lnk->ptr->from
                                 ? -1 * lnk->ptr->parameters[Data::fhlLoopFlow][hour]
                                 : lnk->ptr->parameters[Data::fhlLoopFlow][hour];
                    }

                    if (sum != 0.0)
                    {
                        logs.error() << "Error on loop flow sum validity (!= 0) at hour " << x
                                     << " on node " << link->with->id;
                        return false;
                    }
                }
            }

            if (includePhaseShift) // check validity of phase-shift
            {
                if (link->parameters[Data::fhlPShiftMinus][hour]
                    != link->parameters[Data::fhlPShiftPlus][hour])
                {
                    linkInfo->hasPShiftsEqual = false;
                }

                if (link->parameters[Data::fhlPShiftMinus][hour]
                    > link->parameters[Data::fhlPShiftPlus][hour])
                {
                    logs.error() << "Error on phase shift calendar validity at hour " << x
                                 << " for line " << linkInfo->getName();
                    return false;
                }
            }
        }

        linkInfo->nImpedanceChanges = (uint)impedances.size();

        if (linkInfo->nImpedanceChanges > 0)
        {
            for (uint x = 1; x < HOURS_PER_YEAR; x++)
            {
                linkInfo->avgImpedance += link->parameters[columnImpedance][x];
            }
            linkInfo->avgImpedance /= HOURS_PER_YEAR;
        }
        // To improve (above) !!
        linkInfo->avgImpedance = 1;

        if (linkInfo->nImpedanceChanges == 0 && linkInfo->avgImpedance == 0)
        {
            linkInfo->enabled = false;
        }

        linkInfo->weight = linkInfo->getWeightWithImpedance();
    }

    for (auto linkInfoIt = pLink.begin(); linkInfoIt != pLink.end(); linkInfoIt++)
    {
        if ((*linkInfoIt)->enabled
            && ((*linkInfoIt)->type
                == Antares::Data::atAC /*|| (*linkInfoIt)->type == linkInfo::tyACPST*/))
            enabledACLines.push_back(*linkInfoIt);
    }

    if (enabledACLines.empty())
    {
        return true;
    }

    logs.info() << "Search basis ";

    _grid.clear();

    // create the graph
    for (auto l = enabledACLines.begin(); l != enabledACLines.end(); l++)
    {
        std::string s1((*l)->ptr->from->name.to<std::string>());
        std::string s2((*l)->ptr->with->name.to<std::string>());
        auto n1 = _grid.addNode(*((*l)->ptr->from), s1);
        auto n2 = _grid.addNode(*((*l)->ptr->with), s2);
        _grid.addEdge(n1, n2, (long)(*l)->getWeightWithImpedance());
    }

    // build the set of loops which span the grid
    if (!_grid.buildMesh())
        return false;

    // create the constraints
    logs.info() << "Compute Mesh ";
    const std::vector<std::vector<int>>& meshIndexMatrix = _grid.getMeshIndexMatrix();

    for (uint i = 0; i < meshIndexMatrix.size(); i++)
    {
        std::vector<linkInfo*> Ci;

        for (uint j = 0; j < meshIndexMatrix[i].size(); j++)
        {
            Ci.push_back(enabledACLines[meshIndexMatrix[i][j]]);
        }

        pMesh.push_back(Ci);
    }

    isUpToDate = true;

    return true;
}

bool CBuilder::runConstraintsBuilder(bool standalone)
{
    // build the set of loops which span the grid
    if (!update(true))
        return false;

    // create the constraints
    logs.info() << "Write constraints ";

    const bool result = createConstraints(pMesh);

    if (standalone)
    {
        auto& study = *Data::Study::Current::Get();
        study.saveToFolder(study.folder);
    }
    // return result;
    return result;
}

bool CBuilder::deletePreviousConstraints()
{
    if (pPrefixDelete.empty())
        return true;

    logs.info() << "Deleting previously built network constraints (with prefix  " << pPrefixDelete
                << ")";

    // Data::BindConstList::iterator it = pStudy->bindingConstraints.begin();
    pStudy->bindingConstraints.removeConstraintsWhoseNameConstains(pPrefixDelete);

    for (auto linkInfoIt = pLink.begin(); linkInfoIt != pLink.end(); linkInfoIt++)
    {
        auto linkInfo = *linkInfoIt;
        Data::AreaLink* link = linkInfo->ptr;

        if (link->parameters.jit)
        {
            YString dataFilename = link->parameters.jit->sourceFilename;
            if (!link->loadTimeSeries(*pStudy, dataFilename))
                return false;
        }

        link->useLoopFlow = false;
        link->usePST = false;
    }

    return true;
}

bool CBuilder::saveCBuilderToFile(const String& filename) const
{
    if (!Data::Study::Current::Valid())
        return false;
    String tmp;
    auto& study = *Data::Study::Current::Get();

    IniFile ini;
    auto* mainSection = ini.addSection(".general");

    // Study
    mainSection->add("study", study.folder);

    // Tmp
    /*wxStringToString(pPathTemp->GetValue(), tmp);
    mainSection->add("temporary", tmp);*/

    // Prefix
    mainSection->add("prefix", CB_PREFIX);        // for now, predifined prefix
    mainSection->add("prefix_delete", CB_PREFIX); // for now, predifined prefex
    mainSection->add("include_loopflow", includeLoopFlow);
    mainSection->add("include_phase_shift", includePhaseShift);
    mainSection->add("calendar_start", calendarStart);
    mainSection->add("calendar_end", calendarEnd);
    mainSection->add("infinite_value", infiniteSecondMember);
    mainSection->add("nodal_loopflow_check", checkNodalLoopFlow);
    mainSection->add("delete", pDelete);
    // aliases

    if (filename == "")
    {
        YString buffer;

        buffer.clear() << study.folder << Yuni::IO::Separator << "settings" << Yuni::IO::Separator
                       << "constraintbuilder.ini";
        return ini.save(buffer);
    }

    return ini.save(filename);
}

bool CBuilder::completeCBuilderFromFile(const String& filename)
{
    YString buffer;
    if (filename == "")
    {
        auto& study = *Data::Study::Current::Get();

        buffer.clear() << study.folder << Yuni::IO::Separator << "settings" << Yuni::IO::Separator
                       << "constraintbuilder.ini";
        if (!IO::File::Exists(buffer))
        {
            return false;
        }
    }
    else
    {
        buffer = filename;
    }

    logs.info() << "Read data";
    IniFile ini;
    if (ini.open(buffer))
    {
        // logs.info() << "Reading " << filename;
        logs.info() << "Read data (INI file)";
        IniFile::Section* section;
        CString<50, false> key;
        CString<50, false> value;

        for (section = ini.firstSection; section != NULL; section = section->next)
        {
            if (section->name == ".general")
            {
                IniFile::Property* p = section->firstProperty;
                for (; p != NULL; p = p->next)
                {
                    key = p->key;
                    key.toLower();

                    if (key == "study")
                    {
                        pStudyFolder = p->value;
                        continue;
                    }
                    if (key == "prefix")
                    {
                        pPrefix = p->value;
                        continue;
                    }
                    if (key == "prefix_delete")
                    {
                        pPrefixDelete = p->value;
                        continue;
                    }
                    if (key == "include_loopflow")
                    {
                        includeLoopFlow = p->value.to<bool>();
                        continue;
                    }
                    if (key == "include_phase_shift")
                    {
                        includePhaseShift = p->value.to<bool>();
                        continue;
                    }
                    if (key == "calendar_start")
                    {
                        calendarStart = p->value.to<uint>();
                        continue;
                    }
                    if (key == "calendar_end")
                    {
                        calendarEnd = p->value.to<uint>();
                        continue;
                    }
                    if (key == "infinite_value")
                    {
                        infiniteSecondMember = p->value.to<double>();
                        continue;
                    }
                    if (key == "nodal_loopflow_check")
                    {
                        checkNodalLoopFlow = p->value.to<bool>();
                        continue;
                    }
                    if (key == "delete")
                    {
                        pDelete = p->value.to<bool>();
                        continue;
                    }
                }
            }
        }

        if (pLink.empty())
        {
            logs.error() << "no link found.";
            return false;
        }
    }
    return true;
}

int CBuilder::alreadyExistingNetworkConstraints(const Yuni::String& prefix) const
{
    auto& study = *Data::Study::Current::Get();
    int nSubCount = 0;
    for (auto j = study.bindingConstraints.begin(); j != study.bindingConstraints.end(); j++)
    {
        std::string name = (*j)->name().c_str();
        if (name.find(prefix.to<std::string>()) == 0) // name starts with the prefix
        {
            auto dot = name.find(".");
            auto maxNumber = name.substr(prefix.size(), dot);
            try
            {
                nSubCount = std::max(nSubCount, std::stoi(maxNumber));
            }
            catch (std::exception& e)
            {
                logs.error() << e.what() << '\n';
            }
        }
    }
    return nSubCount;
}

} // namespace Antares
