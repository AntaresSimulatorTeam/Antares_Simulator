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
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__

namespace Antares
{
namespace Data
{
#ifndef ANTARES_SWAP_SUPPORT
inline void ThermalCluster::flush()
{
    // do nothing
}
#endif

#ifndef ANTARES_SWAP_SUPPORT
inline void ThermalClusterList::flush()
{
    // do nothing
}
#endif

inline uint ThermalClusterList::size() const
{
    return (uint)cluster.size();
}

inline bool ThermalClusterList::empty() const
{
    return cluster.empty();
}

inline ThermalClusterList::iterator ThermalClusterList::begin()
{
    return cluster.begin();
}

inline ThermalClusterList::const_iterator ThermalClusterList::begin() const
{
    return cluster.begin();
}

inline ThermalClusterList::iterator ThermalClusterList::end()
{
    return cluster.end();
}

inline ThermalClusterList::const_iterator ThermalClusterList::end() const
{
    return cluster.end();
}


inline const ThermalCluster* ThermalClusterList::find(const Data::ClusterName& id) const
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

inline ThermalCluster* ThermalClusterList::find(const Data::ClusterName& id)
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

template<class PredicateT>
void ThermalClusterList::each(const PredicateT& predicate) const
{
    auto end = cluster.cend();
    for (auto i = cluster.cbegin(); i != end; ++i)
    {
        const ThermalCluster& it = *(i->second);
        predicate(it);
    }
}

template<class PredicateT>
void ThermalClusterList::each(const PredicateT& predicate)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        ThermalCluster& it = *(i->second);
        predicate(it);
    }
}

} // namespace Data
} // namespace Antares

namespace Yuni
{
namespace Extension
{
namespace CString
{
template<class CStringT>
class Append<CStringT, Antares::Data::ThermalLaw>
{
public:
    static void Perform(CStringT& string, Antares::Data::ThermalLaw law)
    {
        switch (law)
        {
        case Antares::Data::thermalLawUniform:
            string += "uniform";
            break;
        case Antares::Data::thermalLawGeometric:
            string += "geometric";
            break;
        }
    }
};

template<>
class Into<Antares::Data::ThermalLaw>
{
public:
    typedef Antares::Data::ThermalLaw TargetType;
    enum
    {
        valid = 1
    };

    static bool Perform(AnyString string, TargetType& out);

    template<class StringT>
    static TargetType Perform(const StringT& s)
    {
        TargetType law = Antares::Data::thermalLawUniform;
        Perform(s, law);
        return law;
    }
};

} // namespace CString
} // namespace Extension
} // namespace Yuni

#endif // __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__
