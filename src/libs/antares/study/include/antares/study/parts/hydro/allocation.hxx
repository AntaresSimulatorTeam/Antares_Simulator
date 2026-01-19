// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_HXX__
#define __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_HXX__

namespace Antares::Data
{
template<class CallbackT>
inline void HydroAllocation::eachNonNull(const CallbackT& callback) const
{
    assert(pMustUseValuesFromAreaID);
    auto end = pValuesFromAreaID.end();
    for (auto i = pValuesFromAreaID.begin(); i != end; ++i)
    {
        callback(i->first, i->second);
    }
}

} // namespace Antares::Data

#endif // __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_HXX__
