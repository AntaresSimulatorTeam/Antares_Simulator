#include "MaintenanceGroup.h"

namespace Antares::Data
{

bool MaintenanceGroup::saveAreaListToFile(const std::string& filename) const
{
    if (!filename)
        return false;

    std::string data;
    {
        // Preparing a new list of areas, sorted by their name
        using List = std::list<std::string>;
        List list;
        {
            auto end = associatedAreas.end();
            for (auto i = associatedAreas.begin(); i != end; ++i)
                list.push_back((i->second)->name.c_str());
            list.sort();
        }
        {
            auto end = list.end();
            for (auto i = list.begin(); i != end; ++i)
                data << *i << '\n';
        }
    }

    // Writing data into the appropriate file
    std::ofstream file(filename);
    if (!file.is_open())
    {
        file << data;
        return true;
    }
    logs.error() << "impossible to write " << filename;
    return false;
}

void MaintenanceGroup::name(const std::string& newname)
{
    name_ = newname;
    ID_.clear();
    Antares::TransformNameIntoID(name_, ID_);
}

uint MaintenanceGroup::getNbOfAssociatedAreas() const
{
    return associatedAreas.size();
}

} // namespace Antares::Data