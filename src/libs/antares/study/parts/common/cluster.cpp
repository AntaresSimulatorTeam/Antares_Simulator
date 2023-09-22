#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <yuni/io/directory.h>

#include "cluster.h"
#include "../../study.h"
#include <antares/utils/utils.h>

namespace Antares::Data
{
Cluster::Cluster(Area* parent) :
 unitCount(0),
 enabled(true),
 parentArea(parent),
 index(0),
 nominalCapacity(0.),
 areaWideIndex((uint)-1),
 series(nullptr)

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
    TransformNameIntoID(pName, pID);
}

#define SEP Yuni::IO::Separator
int Cluster::saveDataSeriesToFolder(const AnyString& folder) const
{
    if (not folder.empty())
    {
        Yuni::Clob buffer;

        buffer.clear() << folder << SEP << parentArea->id << SEP << id();
        if (Yuni::IO::Directory::Create(buffer))
        {
            int ret = 1;
            buffer.clear() << folder << SEP << parentArea->id << SEP << id() << SEP << "series.txt";
            ret = series->timeSeries.saveToCSVFile(buffer, precision()) && ret;

            return ret;
        }
        return 0;
    }
    return 1;
}

int Cluster::loadDataSeriesFromFolder(Study& s, const AnyString& folder)
{
    if (not folder.empty())
    {
        auto& buffer = s.bufferLoadingTS;

        int ret = 1;
        buffer.clear() << folder << SEP << parentArea->id << SEP << id() << SEP << "series."
                       << s.inputExtension;
        ret = series->timeSeries.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &s.dataBuffer) && ret;

        if (s.usedByTheSolver && s.parameters.derated)
            series->timeSeries.averageTimeseries();

        series->timeseriesNumbers.clear();

        return ret;
    }
    return 1;
}
#undef SEP

void Cluster::invalidateArea()
{
    if (parentArea)
        parentArea->forceReload();
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

    if (not series)
        series = new DataSeriesCommon();

    series->timeSeries.reset(1, HOURS_PER_YEAR);
}

bool CompareClusterName::operator()(const Cluster* s1, const Cluster* s2) const
{
    return (s1->getFullName() < s2->getFullName());
}

} // namespace Antares::Data
