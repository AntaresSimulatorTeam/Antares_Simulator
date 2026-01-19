// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_PROVIDER_COMPARISON_H__
#define __ANTARES_WINDOWS_OUTPUT_PROVIDER_COMPARISON_H__

#include <ui/common/component/spotlight.h>
#include "../fwd.h"
#include <wx/bitmap.h>

namespace Antares::Window::OutputViewer::Provider
{
class Comparison final: public Antares::Component::Spotlight::IProvider
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
    Comparison(Component& com);
    //! Destructor
    virtual ~Comparison();
    //@}

    /*!
    ** \brief Perform a new search
    */
    virtual void search(Spotlight::IItem::Vector& out,
                        const Spotlight::SearchToken::Vector& tokens,
                        const Yuni::String& text = "") override;

    /*!
    ** \brief An item has been selected
    */
    virtual bool onSelect(Spotlight::IItem::Ptr& item) override;

private:
    //! The parent component
    Component& pComponent;

    wxBitmap* pBmpSum;
    wxBitmap* pBmpAverage;
    wxBitmap* pBmpMin;
    wxBitmap* pBmpMax;
    wxBitmap* pBmpDiff;

}; // class Layer

} // namespace Antares::Window::OutputViewer::Provider

#endif // __ANTARES_WINDOWS_OUTPUT_PROVIDER_COMPARISON_H__
