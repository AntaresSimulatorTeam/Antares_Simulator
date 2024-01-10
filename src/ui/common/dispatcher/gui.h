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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
