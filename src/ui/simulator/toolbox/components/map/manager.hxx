// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_MAP_CONTAINER_HXX__
#define __ANTARES_TOOLBOX_MAP_CONTAINER_HXX__

namespace Antares::Map
{
inline void Manager::deallocationStackAdd(Item* it)
{
    pStackDeallocation.insert(it);
}

inline void Manager::deallocationStackRemove(Item* it)
{
    pStackDeallocation.erase(it);
}

inline bool Manager::deallocationStackExists(Item* it)
{
    return pStackDeallocation.find(it) != pStackDeallocation.end();
}

inline bool Manager::loadFromAttachedStudy()
{
    return (pStudy) ? loadFromStudy(*pStudy) : false;
}

template<class T>
inline T* Manager::add()
{
    return new T(*this);
}

} // namespace Antares::Map

#endif // __ANTARES_TOOLBOX_MAP_CONTAINER_HXX__
