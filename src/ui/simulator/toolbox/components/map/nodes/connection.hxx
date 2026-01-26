// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_MAP_CONNECTION_HXX__
#define __ANTARES_TOOLBOX_MAP_CONNECTION_HXX__

namespace Antares::Map
{
inline Item::Type Connection::type() const
{
    return tyConnection;
}

inline bool Connection::selected() const
{
    return pSelected;
}

inline Antares::Data::AreaLink* Connection::attachedAreaLink() const
{
    return pAttachedAreaLink;
}

} // namespace Antares::Map

#endif // __ANTARES_TOOLBOX_MAP_CONNECTION_HXX__
