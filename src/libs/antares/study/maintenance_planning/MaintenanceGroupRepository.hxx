//
// Created by milos on 1/11/23.
//

#pragma once

#include <memory>
#include <antares/utils/utils.h>

namespace Antares::Data
{

inline uint MaintenanceGroupRepository::size() const
{
    return (uint)maintenanceGroups_.size();
}

inline bool MaintenanceGroupRepository::empty() const
{
    return maintenanceGroups_.empty();
}

inline uint MaintenanceGroupRepository::scenariosNumber() const
{
    return scenariosNumber_;
}

inline void MaintenanceGroupRepository::scenariosNumber(uint num)
{
    scenariosNumber_ = num;
}

inline uint MaintenanceGroupRepository::scenariosLength() const
{
    return scenariosLength_;
}

inline void MaintenanceGroupRepository::scenariosLength(uint l)
{
    scenariosLength_ = l;
}

template<class PredicateT>
inline void MaintenanceGroupRepository::each(const PredicateT& predicate)
{
    uint count = (uint)maintenanceGroups_.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(maintenanceGroups_[i]));
}

template<class PredicateT>
inline void MaintenanceGroupRepository::each(const PredicateT& predicate) const
{
    uint count = (uint)maintenanceGroups_.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(maintenanceGroups_[i].get()));
}
} // namespace Antares::Data
