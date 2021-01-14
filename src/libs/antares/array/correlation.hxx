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
#ifndef __ANTARES_LIBS_ARRAY_CORRELATION_HXX__
#define __ANTARES_LIBS_ARRAY_CORRELATION_HXX__

namespace Antares
{
namespace Data
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

inline bool Correlation::loadFromINI(Study& study, const IniFile& ini, bool warnings, int version)
{
    return internalLoadFromINI(study, ini, warnings, version);
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

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_ARRAY_CORRELATION_HXX__
