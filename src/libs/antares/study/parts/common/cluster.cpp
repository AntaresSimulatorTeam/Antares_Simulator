// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/common/cluster.h"

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/study/study.h>
#include <antares/utils/utils.h>

namespace fs = std::filesystem;

namespace Antares::Data
{
Cluster::Cluster(Area* parent):
    parentArea(parent),
    series(tsNumbers)
{
}

const std::string& Cluster::name() const
{
    return pName;
}

const std::string& Cluster::id() const
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

void Cluster::setGroup(const std::string& group)
{
    group_ = group;
    boost::to_upper(group_);
}

std::string Cluster::getGroup() const
{
    return group_;
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

bool Cluster::loadDataSeriesFromFolder(Study& s, const fs::path& folder)
{
    if (folder.empty())
    {
        return true;
    }

    auto& buffer = s.bufferLoadingTS;

    bool ret = true;
    fs::path seriesPath = folder / parentArea->id / id() / "series.txt";

    ret = series.timeSeries.loadFromCSVFile(seriesPath.string(), 1, HOURS_PER_YEAR, &s.dataBuffer)
          && ret;

    if (s.parameters.derated)
    {
        series.timeSeries.averageTimeseries();
    }

    series.timeseriesNumbers.clear();

    return ret;
}

#undef SEP

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

} // namespace Antares::Data
