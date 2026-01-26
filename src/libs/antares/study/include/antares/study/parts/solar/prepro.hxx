// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_SOLAR_PREPRO_HXX__
#define __ANTARES_LIBS_STUDY_PARTS_SOLAR_PREPRO_HXX__

namespace Antares::Data::Solar
{
inline bool Prepro::forceReload(bool reload) const
{
    return xcast.forceReload(reload);
}

inline void Prepro::markAsModified() const
{
    xcast.markAsModified();
}

inline void Prepro::resetToDefault()
{
    xcast.resetToDefaultValues();
}

} // namespace Antares::Data::Solar

#endif // __ANTARES_LIBS_STUDY_PARTS_SOLAR_PREPRO_H__
