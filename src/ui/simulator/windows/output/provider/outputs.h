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
#ifndef __ANTARES_WINDOWS_OUTPUT_PROVIDER_OUTPUTS_H__
#define __ANTARES_WINDOWS_OUTPUT_PROVIDER_OUTPUTS_H__

#include <ui/common/component/spotlight.h>
#include "../fwd.h"
#include <wx/bitmap.h>

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
class Outputs : public Antares::Component::Spotlight::IProvider
{
public:
    //! The spotlight component (alias)
    using Spotlight = Antares::Component::Spotlight;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Outputs(Component& com, Layer* layer);
    //! Destructor
    virtual ~Outputs();
    //@}

    /*!
    ** \brief Perform a new search
    */
    virtual void search(Spotlight::IItem::Vector& out,
                        const Spotlight::SearchToken::Vector& tokens,
                        const Yuni::String& text = "");

    /*!
    ** \brief An item has been selected
    */
    virtual bool onSelect(Spotlight::IItem::Ptr& item);

private:
    //! The parent component
    Component& pComponent;
    //! The current layer, if any
    Layer* pLayer;

    wxBitmap* pBmpClose;
    wxBitmap* pBmpNumbers[10];
    wxBitmap* pBmpMultiple;
    wxBitmap* pBmpEmpty;

}; // class Layer

} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_PROVIDER_OUTPUTS_H__
