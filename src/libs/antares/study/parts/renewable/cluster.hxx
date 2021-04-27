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
#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_HXX__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_HXX__

namespace Antares
{
namespace Data
{
#ifndef ANTARES_SWAP_SUPPORT
inline void RenewableCluster::flush()
{
    // do nothing
}
#endif

#ifndef ANTARES_SWAP_SUPPORT
inline void RenewableClusterList::flush()
{
    // do nothing
}
#endif

inline uint RenewableClusterList::size() const
{
    return (uint)cluster.size();
}

inline bool RenewableClusterList::empty() const
{
    return cluster.empty();
}

inline RenewableClusterList::iterator RenewableClusterList::begin()
{
    return cluster.begin();
}

inline RenewableClusterList::const_iterator RenewableClusterList::begin() const
{
    return cluster.begin();
}

inline RenewableClusterList::iterator RenewableClusterList::end()
{
    return cluster.end();
}

inline RenewableClusterList::const_iterator RenewableClusterList::end() const
{
    return cluster.end();
}

inline const Data::RenewableClusterName& RenewableCluster::group() const
{
    return pGroup;
}

inline const Data::RenewableClusterName& RenewableCluster::id() const
{
    return pID;
}

inline const Data::RenewableClusterName& RenewableCluster::name() const
{
    return pName;
}

inline const RenewableCluster* RenewableClusterList::find(
  const Data::RenewableClusterName& id) const
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

inline RenewableCluster* RenewableClusterList::find(const Data::RenewableClusterName& id)
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

template<class PredicateT>
void RenewableClusterList::each(const PredicateT& predicate) const
{
    auto end = cluster.cend();
    for (auto i = cluster.cbegin(); i != end; ++i)
    {
        const RenewableCluster& it = *(i->second);
        predicate(it);
    }
}

template<class PredicateT>
void RenewableClusterList::each(const PredicateT& predicate)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        RenewableCluster& it = *(i->second);
        predicate(it);
    }
}

} // namespace Data
} // namespace Antares

#endif //__ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_HXX__
