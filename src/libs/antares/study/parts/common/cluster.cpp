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
#include "antares/study/parts/common/cluster.h"

#include <yuni/yuni.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>

#include <antares/utils/utils.h>
#include "antares/study/study.h"

namespace Antares::Data
{
Cluster::Cluster(Area* parent):
    parentArea(parent),
    series(tsNumbers)
{
}

const ClusterName& Cluster::group() const
{
    return pGroup;
}

const ClusterName& Cluster::name() const
{
    return pName;
}

const ClusterName& Cluster::id() const
{
    return pID;
}

Yuni::String Cluster::getFullName() const
{
    Yuni::String s;
    s << parentArea->name << "." << pID;
    return s;
}

void Cluster::setName(const AnyString& newname)
{
    pName = newname;
    pID.clear();
    pID = transformNameIntoID(pName);
}

#define SEP Yuni::IO::Separator

bool Cluster::saveDataSeriesToFolder(const AnyString& folder) const
{
    if (folder.empty())
    {
        return true;
    }

    Yuni::Clob buffer;
    buffer.clear() << folder << SEP << parentArea->id << SEP << id();
    if (!Yuni::IO::Directory::Create(buffer))
    {
        return true;
    }

    buffer.clear() << folder << SEP << parentArea->id << SEP << id() << SEP << "series.txt";
    return series.timeSeries.saveToCSVFile(buffer, precision());
}

bool Cluster::loadDataSeriesFromFolder(Study& s, const AnyString& folder)
{
    if (folder.empty())
    {
        return true;
    }

    auto& buffer = s.bufferLoadingTS;

    bool ret = true;
    buffer.clear() << folder << SEP << parentArea->id << SEP << id() << SEP << "series."
                   << s.inputExtension;
    ret = series.timeSeries.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &s.dataBuffer) && ret;

    if (s.usedByTheSolver && s.parameters.derated)
    {
        series.timeSeries.averageTimeseries();
    }

    series.timeseriesNumbers.clear();

    return ret;
}

#undef SEP


void Cluster::addReserveParticipation(
  Data::ReserveName name,
  ThermalClusterReserveParticipation reserveParticipation)
{
    clusterReservesParticipations.emplace(name, reserveParticipation);
}

bool Cluster::isParticipatingInReserve(Data::ReserveName name)
{
    if (clusterReservesParticipations.contains(name))
        return true;
    else
        return false;
}

float Cluster::reserveMaxPower(Data::ReserveName name)
{
    if (clusterReservesParticipations.contains(name))
        return clusterReservesParticipations.at(name).maxPower;
    else
        return -1;
}

float Cluster::reserveCost(Data::ReserveName name)
{
    if (clusterReservesParticipations.contains(name))
        return clusterReservesParticipations.at(name).participationCost;
    else
        return -1;
}

float Cluster::reserveCostOff(Data::ReserveName name)
{
    if (clusterReservesParticipations.contains(name))
        return clusterReservesParticipations.at(name).participationCostOff;
    else
        return -1;
}

float Cluster::reserveMaxPowerOff(Data::ReserveName name)
{
    if (clusterReservesParticipations.contains(name))
        return clusterReservesParticipations.at(name).maxPowerOff;
    else
        return -1;
}

unsigned int Cluster::reserveParticipationsCount(){
    return clusterReservesParticipations.size();
}

void Cluster::invalidateArea()
{
    if (parentArea)
    {
        parentArea->forceReload();
    }
}

bool Cluster::isVisibleOnLayer(const size_t& layerID) const
{
    return parentArea ? parentArea->isVisibleOnLayer(layerID) : false;
}

void Cluster::reset()
{
    unitCount = 0;
    enabled = true;
    nominalCapacity = 0.;

    series.timeSeries.reset(1, HOURS_PER_YEAR);
}

bool CompareClusterName::operator()(const Cluster* s1, const Cluster* s2) const
{
    return (s1->getFullName() < s2->getFullName());
}

std::vector<Data::ReserveName> Cluster::listOfParticipatingReserves()
{
    std::vector<Data::ReserveName> reserves;
    for (auto reserve: clusterReservesParticipations)
    {
        reserves.push_back(reserve.first);
    }
    return reserves;
}

} // namespace Antares::Data
