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
#ifndef __ANTARES_COMMON_COMPONENTS_FRAME_REGISTRY_H__
#define __ANTARES_COMMON_COMPONENTS_FRAME_REGISTRY_H__

#include "frame.h"

namespace Antares
{
namespace Component
{
namespace Frame
{
namespace Registry
{
/*!
** \brief Register a new frame
*/
void RegisterFrame(IFrame* frame);

/*!
** \brief Unregister a frame
*/
void UnregisterFrame(IFrame* frame);

/*!
** \brief Update all registered frames
*/
void DispatchUpdate();

/*!
** \brief Update all registered frames
**
** param exclude Any frame which should be excluded from the update
*/
void DispatchUpdate(IFrame* exclude);

/*!
** \brief Close all local registered frames
*/
void CloseAllLocal();

/*!
** \brief Get the list of all registered frames
*/
const IFrame::Vector& List();

} // namespace Registry
} // namespace Frame
} // namespace Component
} // namespace Antares

#endif // __ANTARES_COMMON_COMPONENTS_FRAME_REGISTRY_H__
