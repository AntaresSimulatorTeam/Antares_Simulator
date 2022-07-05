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
#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_H__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_H__

#include <yuni/yuni.h>
#include "../panel.h"
#include <vector>
#include <yuni/core/event.h>
#include <wx/bitmap.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <yuni/core/color/rgb.h>

#include <memory>

namespace Antares
{
namespace Component
{
class Spotlight final : public Antares::Component::Panel, public Yuni::IEventObserver<Spotlight>
{
public:
    enum Options
    {
        optNone = 0,
        optResultsInAPopup = 1,
        //! Display groups as well
        optGroups = 2,
        //! Try to automatically resize the parent window when according to the list
        optAutoResizeParent = 4,
        //! The search input will not be shown
        optNoSearchInput = 8,
        //! Use a white background instead of the default one
        optBkgWhite = 16,
    };
    enum
    {
        //! Default flags used by the constructor
        defaultFlags = optResultsInAPopup | optGroups,
    };

// class SearchToken
#include "searchtoken.h"
// class IItem / Separator
#include "iitem.h"
#include "separator.h"
#include "text.h"
// class IProvider
#include "provider.h"
// class NullProvider
#include "null-provider.h"

public:
    /*!
    ** \brief Display the spotlight in a frame
    */
    static void FrameShow(wxWindow* parent,
                          IProvider::Ptr provider,
                          uint flags = optGroups,
                          uint width = 340);

    /*!
    ** \brief Close any opened window
    */
    static void FrameClose();

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    ** \param flags  Flags
    */
    Spotlight(wxWindow* parent, uint flags = defaultFlags);
    //! Destructor
    virtual ~Spotlight();
    //@}

    //! \name Search
    //@{
    /*!
    ** \brief Update the component with a given search string
    */
    void search(const Yuni::String& text);

    /*!
    ** \brief Update the component with a given search string
    ** \see search(const Yuni::String&)
    */
    void search(const wxString& text);

    /*!
    ** \brief Redo the last search
    */
    void redoResearch();

    /*!
    ** \brief Reset the search input
    **
    ** The component will be updated with an empty search input.
    */
    void resetSearchInput();
    //@}

    /*!
    ** \brief set the value of the search input
    **
    ** The component will be updated with the value of the text.
    */
    void onMapLayerChanged(const wxString* text);
    //@}

    void onMapLayerAdded(const wxString* text);

    void onMapLayerRemoved(const wxString* text);

    void onMapLayerRenamed(const wxString* text);

    //! \name Data provider
    //@{
    //! Get the provider
    IProvider::Ptr provider() const;
    //! Set the provider
    void provider(IProvider::Ptr provider);
    //@}

    //! \name Visual
    //@{
    //! Get the item height
    uint itemHeight() const;
    //! Set the item height
    void itemHeight(uint h);
    //@}

public:
    //! Event: update items
    Yuni::Event<void(const IItem::VectorPtr&, const SearchToken::VectorPtr&)> onUpdateItems;

    /*!
    ** \brief Update the GUI with the new results
    */
    void updateResults();

protected:
    /*!
    ** \brief Convert a raw text into a vector of tokens
    */
    void convertRawTextIntoSearchTokenVector(SearchToken::Vector& out, const Yuni::String& text);

    void createComponents(Spotlight* parent, bool input, bool results);

    //! Event: the text input has been changed
    void onInputUpdated(wxCommandEvent& evt);

    void onComboUpdated(wxCommandEvent& evt);

    //! Resize the parent window
    void resizeParentWindow();

protected:
    //! Flags
    uint pFlags;
    //!
    IProvider::Ptr pDataProvider;
    //! Result set
    IItem::VectorPtr pResults;
    //!
    SearchToken::VectorPtr pTokens;
    //! The last research
    Yuni::String pLastResearch;
    //! Internal updater for the provider
    Yuni::Event<void(IProvider::Ptr)> pResetProvider;
    //! ComboBox Filter
    wxComboBox* pLayerFilter;
    //! Text edit
    wxTextCtrl* pEdit;
    //! The listbox
    wxWindow* pListbox;
    //! Item height
    uint pItemHeight;

    // data related to frame
    //! Display monitor handle
    int pDisplayHandle;
    //! Parent to consider for layout positioning when used from a frame
    wxWindow* pLayoutParent;
    //! Previous height
    // When the mini frame is above the parent control, it should not be resized
    // the frame can be resized if this variable is zero
    bool pAllowFrameResize;

    // keeping the sizers for some hide/show Magic depending on the number of layers
    wxBoxSizer* vz;
    wxBoxSizer* hzCombo;

}; // class Spotlight

} // namespace Component
} // namespace Antares

#include "spotlight.hxx"
#include "iitem.hxx"

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_H__
