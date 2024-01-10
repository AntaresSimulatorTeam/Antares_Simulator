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
#ifndef __ANTARES_TOOLBOX_FILTER_INPUT_H__
#define __ANTARES_TOOLBOX_FILTER_INPUT_H__

#include <wx/choice.h>
#include "filter.h"
#include "component/panel.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
// Forward declaration (Parent class)
class Component;

class Input : public Antares::Component::Panel
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Input(Component* parent);
    //! Destructor
    virtual ~Input();
    //@}

    //! Get the Unique ID for the input
    int id() const
    {
        return pId;
    }

    //! \name Precision
    //@{
    //! Get the precision used by the filter
    Date::Precision precision() const
    {
        return pPrecision;
    }
    //! Set the precision to use for the filter
    void precision(const Date::Precision p)
    {
        pPrecision = p;
    }
    //@}

    /*!
    ** \brief Add a filter from its ID
    */
    void add(const wxString& filterName);

    //! Get the current selected filter (may be null)
    AFilterBase* selected() const
    {
        return pSelected ? pSelected->filter : NULL;
    }

    /*!
    ** \brief Add a standard preset for hourly matrices
    */
    void addStdPreset();

    /*!
    ** \brief Enable/Disable the button to remove the current filter
    **
    ** Filters are always available. So when the last one remains,
    ** it is better to hide the button to remove it.
    */
    void showBtnToRemoveFilter(bool visible);

    Component* component() const
    {
        return pParent;
    }

private:
    /*!
    ** \brief Structure that holds data to retrieve the good filter in the input selector
    */
    struct SelectorClientData : public wxClientData
    {
        //! \name Constructor && Destructor
        //@{
        /*!
        ** \brief Default constructor
        ** \param i Index of the client data in the input selector
        */
        explicit SelectorClientData(const wxString& i);
        //! Destructor
        virtual ~SelectorClientData();
        //@}

        /*!
        ** \brief Create all controls associated to the filter if not already done
        */
        AFilterBase* createIfNeeded(Input* input, wxWindow* parent);

        /*!
        ** \brief Re-Attach the controls owned by the filter to a given sizer
        */
        void attachToSizer(wxSizer& sizer);

        //! ID of the filder
        wxString id;
        //! Filter  (may be NULL if not used)
        AFilterBase* filter;

    }; // SelectorClientData

private:
    //! Select a filter from the client data
    void selectFilter(SelectorClientData* data);

    //! Event: The filter has been changed from the input selector
    void onFilterChanged(wxCommandEvent& evt);
    //! Event: The user is asking to add a filter
    void onAddFilter(void*);
    //! Event: The user is asking to remove the filter
    void onRemoveFilter(void*);

private:
    //! Input ID
    const int pId;
    //! Parent
    Component* pParent;
    //! The current selection
    SelectorClientData* pSelected;
    //! Filter selector
    wxChoice* pChoice;
    //! Sizer used for all controls owned by the filter
    wxSizer* pFilterSizer;
    //! Button: remove the filter
    Antares::Component::Button* pBtnMinus;
    //! Button: remove the filter
    Antares::Component::Button* pBtnPlus;
    //! Precision
    Date::Precision pPrecision;

}; // class Input

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_INPUT_H__
