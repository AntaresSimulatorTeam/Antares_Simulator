/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_FWD_H__
#define __ANTARES_FWD_H__

// wxWigets
class wxGrid;
class wxWindow;
class wxTopLevelWindow;

// Antares study library
#include <antares/study/fwd.h>

namespace Antares
{
namespace Map
{
class Component;

} // namespace Map

namespace Component
{
class Notebook;

} // namespace Component

namespace Window
{
class StudyLogs;
class BindingConstraint;
class Notes;
class Sets;

} // namespace Window

namespace Forms
{
class ApplWnd;
template<class WindowT>
class Disabler;

} // namespace Forms

} // namespace Antares

#endif // __ANTARES_FWD_H__
