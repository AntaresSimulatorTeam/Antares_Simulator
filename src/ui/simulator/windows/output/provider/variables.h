// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_H__
#define __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_H__

#include <ui/common/component/spotlight.h>
#include "../fwd.h"
#include <wx/bitmap.h>

namespace Antares::Window::OutputViewer::Provider
{
class Variables: public Antares::Component::Spotlight::IProvider
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
    Variables(Component* com);
    //! Destructor
    virtual ~Variables();
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
    Component* pComponent;

    //! Bitmap re-attach
    wxBitmap* pBmpVariable;
    wxBitmap* pBmpThermal;

}; // class Layer

} // namespace Antares::Window::OutputViewer::Provider

#endif // __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_H__
