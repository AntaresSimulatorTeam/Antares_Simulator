/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_TOOLBOX_INPUT_H__
#define __ANTARES_TOOLBOX_INPUT_H__

#include <ui/common/component/panel.h>

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
class AInput : public Antares::Component::Panel
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    ** \param parent the parent window
    */
    AInput(wxWindow* parent);
    //! Destructor
    virtual ~AInput();
    //@}

    /*!
    ** \brief Clear then Update the input
    */
    virtual void update() = 0;

    /*!
    ** \brief Only update the content and not the list itself
    */
    virtual void updateRowContent();

    /*!
    ** \brief Update each item available in the input
    **
    ** \param searchString The string to highlight
    */
    virtual void updateInnerData(const wxString& searchString);

    /*!
    ** \brief Get the recommended size for the control
    */
    virtual wxPoint recommendedSize() const
    {
        return wxPoint(50, 50);
    }

protected:
    virtual void internalBuildSubControls() = 0;

}; // class IInput

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_INPUT_H__
