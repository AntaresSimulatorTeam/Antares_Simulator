/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_PROGRESSBAR_H__
#define __ANTARES_TOOLBOX_COMPONENTS_PROGRESSBAR_H__

#include <ui/common/component/panel.h>

namespace Antares
{
namespace Component
{
/*!
** \brief A simple panel with a caption
*/
class ProgressBar : public Panel
{
public:
    //! \name Constructors & Destructor
    //@{
    /*!
    ** \brief Default contructor
    ** \param parent The parent control
    */
    ProgressBar(wxWindow* parent);

    //! Destructor
    ~ProgressBar();
    //@}

    //! \name Value
    //@{
    //! Get the progress value
    double value() const
    {
        return pValue;
    }
    //! Set the progress value
    void value(double v)
    {
        pValue = v;
        Refresh();
    }
    //@}

protected:
    /*!
    ** \brief Event: The panel has to paint itself
    */
    void onDraw(wxPaintEvent& evt);
    /*!
    ** \brief Event: The panel is been resizing
    */
    void onResize(wxSizeEvent& evt);

private:
    //! Progression
    double pValue;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class ProgressBar

} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_PROGRESSBAR_H__
