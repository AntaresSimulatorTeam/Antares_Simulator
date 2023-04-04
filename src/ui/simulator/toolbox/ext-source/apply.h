/*
** Copyright 2007-2023 RTE
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
#ifndef __ANTARES_TOOLBOX_EXT_SOURCE_APPLY_H__
#define __ANTARES_TOOLBOX_EXT_SOURCE_APPLY_H__

#include <yuni/yuni.h>
#include <antares/study.h>
#include <antares/study/action/action.h>
#include <antares/study/action/context.h>

namespace Antares
{
namespace ExtSource
{
namespace Handler
{
/*!
** \brief Apply all actions
*/
void Apply(Antares::Action::Context::Ptr context,
           Antares::Action::IAction::Ptr actions,
           bool windowRequired = false);

} // namespace Handler
} // namespace ExtSource
} // namespace Antares

#endif // __ANTARES_TOOLBOX_EXT_SOURCE_APPLY_H__
