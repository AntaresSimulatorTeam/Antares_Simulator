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

#include <algorithm>
#include "rules.h"
#include "../study.h"
#include "../../logs.h"
#include "scBuilderUtils.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
Rules::Rules() : load(), solar(), hydro(), wind(), thermal(), hydroLevels(), pAreaCount(0)
{
}

Rules::~Rules()
{
    delete[] thermal;
}

void Rules::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    file << "[" << pName << "]\n";
    if (pAreaCount)
    {
        // load
        load.saveToINIFile(study, file);
        // solar
        solar.saveToINIFile(study, file);
        // hydro
        hydro.saveToINIFile(study, file);
        // wind
        wind.saveToINIFile(study, file);
        // Thermal, each area
        for (uint i = 0; i != pAreaCount; ++i)
            thermal[i].saveToINIFile(study, file);
        // hydro levels
        hydroLevels.saveToINIFile(study, file);
    }
    file << '\n';
}

bool Rules::reset(const Study& study)
{
    // Alias to the current study
    assert(&study != nullptr);

    // The new area count
    pAreaCount = study.areas.size();

    load.reset(study);
    solar.reset(study);
    hydro.reset(study);
    wind.reset(study);

    delete[] thermal;
    thermal = new thermalTSNumberData[pAreaCount];

    for (uint i = 0; i != pAreaCount; ++i)
    {
        thermal[i].attachArea(study.areas.byIndex[i]);
        thermal[i].reset(study);
    }

    hydroLevels.reset(study);
    return true;
}

void Rules::loadFromInstrs(Study& study,
                           const AreaName::Vector& instrs,
                           String value,
                           bool updaterMode = false)
{
    assert(instrs.size() > 2);

    const AreaName& kind_of_scenario = instrs[0]; // load, thermal, hydro, ..., hydro levels, ... ?
    const uint year = instrs[2].to<uint>();
    const AreaName& areaname = instrs[1];
    const ClusterName& clustername = (instrs.size() == 4) ? instrs[3] : "";

    if (kind_of_scenario.size() > 2)
        return;

    Data::Area* area = study.areas.find(areaname);
    if (!area)
    {
        // silently ignore the error
        if (not updaterMode)
            logs.warning() << "[scenario-builder] The area '" << areaname << "' has not been found";

        return;
    }

    if (kind_of_scenario == "t")
    {
        if (clustername.empty())
            return;

        const ThermalCluster* cluster = area->thermal.list.find(clustername);
        if (not cluster)
            cluster = area->thermal.mustrunList.find(clustername);

        if (cluster)
        {
            uint val = fromStringToTSnumber(value);
            thermal[area->index].set(cluster, year, val);
        }
        else
        {
            bool isTheActiveRule
              = (pName.toLower() == study.parameters.activeRulesScenario.toLower());
            if (not updaterMode and isTheActiveRule)
            {
                string clusterId = (area->id).to<string>() + "." + clustername.to<string>();
                disabledClustersOnRuleActive[clusterId].push_back(year);
            }
        }
    }

    if (kind_of_scenario == "l")
    {
        uint val = fromStringToTSnumber(value);
        load.set(area->index, year, val);
    }

    if (kind_of_scenario == "w")
    {
        uint val = fromStringToTSnumber(value);
        wind.set(area->index, year, val);
    }

    if (kind_of_scenario == "h")
    {
        uint val = fromStringToTSnumber(value);
        hydro.set(area->index, year, val);
    }

    if (kind_of_scenario == "s")
    {
        uint val = fromStringToTSnumber(value);
        solar.set(area->index, year, val);
    }

    if (kind_of_scenario == "hl")
    {
        double val = fromStringToHydroLevel(value, 1.);
        hydroLevels.set(area->index, year, val);
    }
}

void Rules::apply(Study& study)
{
    if (pAreaCount)
    {
        load.apply(study);
        solar.apply(study);
        hydro.apply(study);
        wind.apply(study);
        for (uint i = 0; i != pAreaCount; ++i)
            thermal[i].apply(study);
        hydroLevels.apply(study);
    }
}

void Rules::sendWarningsForDisabledClusters()
{
    for (map<string, vector<uint>>::iterator it = disabledClustersOnRuleActive.begin();
         it != disabledClustersOnRuleActive.end();
         it++)
    {
        vector<uint>& scenariiForCurrentCluster = it->second;
        int nbScenariiForCluster = (int)scenariiForCurrentCluster.size();
        vector<uint>::iterator itv = scenariiForCurrentCluster.begin();

        // Listing the 10 first scenarii numbers (years numbers) where current cluster is refered
        // to. Notice that these scenarii could be less then 10, but are at least 1.
        string listScenarii = to_string(*itv + 1);
        itv++;
        for (; itv != scenariiForCurrentCluster.end(); itv++)
            listScenarii += ", " + to_string(*itv + 1);

        // Adding last scenario to the list
        if (nbScenariiForCluster > 10)
            listScenarii += "..." + to_string(scenariiForCurrentCluster.back());

        logs.warning()
          << "Cluster " << it->first
          << " not found: it may be disabled, though given TS numbers in sc builder for year(s) :";
        logs.warning() << listScenarii;
    }
}

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares
