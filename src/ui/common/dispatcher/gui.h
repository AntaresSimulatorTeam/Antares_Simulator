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
#ifndef __ANTARES_TOOLBOX_ACTION_GUI_H__
#define __ANTARES_TOOLBOX_ACTION_GUI_H__

#include "action.h"
#include "../wx-wrapper.h"

// forward declaration
class wxDialog;

namespace Antares
{
// Forward declaration
namespace Window
{
class Message;
}

namespace Dispatcher
{
namespace GUI
{
/*!
** \brief Rebuild the layout of a wx component
*/
void Layout(wxSizer* sizer);

/*!
** \brief Refresh a wx component
*/
void Refresh(wxWindow* window);

/*!
** \brief Refresh a wx component after a delay
*/
void Refresh(wxWindow* window, uint delay);

/*!
** \brief Create a new instance of the window and show it
*/
template<class FormT, class ParentT>
void CreateAndShowModal(ParentT* parent = NULL);

/*!
** \brief Close the window
*/
void Close(wxWindow* window);

/*!
** \brief Close the window
*/
void Close(wxWindow* window, uint delay);

/*!
** \brief Show modal the window and then destroy it
*/
void ShowModal(wxDialog* window, bool destroy = true);

/*!
** \brief Show the window
**
** \param window The component to show
** \param focus True to give the focus to the window
** \param makeModal True to make the window modal
*/
void Show(wxWindow* window, bool focus = true, bool makeModal = false);

/*!
** \brief Create a new instance of the window and show it
*/
// void ShowModal(Window::Message* message);

/*!
** \brief Close the window
*/
void Destroy(wxWindow* window);

/*!
** \brief Close the window
*/
void Destroy(wxTimer* window);

} // namespace GUI
} // namespace Dispatcher
} // namespace Antares

#include "gui.hxx"

#endif // __ANTARES_TOOLBOX_ACTION_GUI_H__
