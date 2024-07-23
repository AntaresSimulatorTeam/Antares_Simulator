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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_WINDOWS_TEXTINPUT_INPUT_H__
#define __ANTARES_WINDOWS_TEXTINPUT_INPUT_H__


namespace Antares
{
namespace Window
{
/*!
** \brief Display a dialog box for resizing matrices
**
** \param[in,out] User's input text
** \param title Title of the dialog box
** \param caption Caption
** \return True if the operation should continue, false if aborted
*/
bool AskForInput(wxString& out, const wxString& title, const wxString& caption);

} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_TEXTINPUT_INPUT_H__
