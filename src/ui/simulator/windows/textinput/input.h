// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_TEXTINPUT_INPUT_H__
#define __ANTARES_WINDOWS_TEXTINPUT_INPUT_H__


namespace Antares::Window
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

} // namespace Antares::Window

#endif // __ANTARES_WINDOWS_TEXTINPUT_INPUT_H__
