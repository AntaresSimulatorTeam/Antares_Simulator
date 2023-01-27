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
#ifndef __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXAREA_NAME_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXAREA_NAME_HXX__

namespace Antares
{
namespace Action
{
namespace Settings
{
template<class StringT>
inline SuffixAreaName::SuffixAreaName(const StringT& value, bool enabled) : pValue(value)
{
    pInfos.caption = "Suffix for area names";
    if (!enabled)
        pInfos.behavior = bhSkip;
}

inline SuffixAreaName::~SuffixAreaName()
{
}

inline bool SuffixAreaName::performWL(Context&)
{
    return true;
}

inline bool SuffixAreaName::autoExpand() const
{
    return false;
}

inline bool SuffixAreaName::shouldPrepareRootNode() const
{
    return true;
}

} // namespace Settings
} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXAREA_NAME_HXX__
