// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_CORRELATION_HXX__
#define __ANTARES_LIBS_ARRAY_CORRELATION_HXX__
#include "antares/study/area/area.h"

namespace Antares::Data
{
inline void InterAreaCorrelationResetMatrix(Matrix<>* m, AreaList* l)
{
    assert(NULL != m);
    assert(NULL != l);
    m->resize(l->size(), l->size());
    m->fillUnit();
}

inline void Correlation::mode(Correlation::Mode mode)
{
    pMode = mode;
}

inline Correlation::Mode Correlation::mode() const
{
    return pMode;
}

template<class StringT>
inline void Correlation::set(Matrix<>& m, const Area& from, const Area& to, const StringT& value)
{
    double d;
    if (!value.template to<double>(d))
    {
        Antares::logs.error() << correlationName << ": Invalid decimal value for '" << from.name
                              << "' / '" << to.name << "'  (got '" << value << "')";
        return;
    }

    set(m, from, to, d);
}

} // namespace Antares::Data

#endif // __ANTARES_LIBS_ARRAY_CORRELATION_HXX__
