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
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_H__

#include <antares/wx-wrapper.h>
#include <wx/htmllbox.h>
#include "item/item.h"
#include "sort.h"
#include <wx/srchctrl.h>
#include "datasource/datasource.h"
#include <ui/common/component/panel.h>
#include <yuni/core/event.h>

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
/*!
** \brief Listbox with HTML content
**
** \warning This component is DEPRECATED !!!
**
** This component can be considered as a ListBox, with HTML content.
** Each item is more than a simple string, and must inherit from
** the class Item::IItem. The component can only be filled with
** a custom datasource.
** This component is lazy. It onlys updates from the datasource
** when it is really visible by the user.
*/
class Component final : public Panel, public Yuni::IEventObserver<Component>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    ** \param parent The parent window
    */
    Component(wxWindow* parent);
    //! Destructor
    virtual ~Component();
    //@}

    /*!
    ** \brief Removes all items
    */
    void clear();

    /*!
    ** \brief Add an item
    **
    ** \param invalidate True to force the visual update of the component
    */
    void add(Item::IItem::Ptr it);

    /*!
    ** \brief Add a datasource
    **
    ** \code
    ** c->addDatasource<MyDatasource>();
    ** \endcode
    */
    template<class D>
    D* addDatasource();

    /*!
    ** \brief Force a visual refresh of the component
    */
    void invalidate();

    /*!
    ** \brief Refresh only the content of all items
    **
    ** The list itself won't be refreshed, only the content
    ** \see forceUpdate()
    */
    void updateHtmlContent();

    /*!
    ** \brief Force the update of the whole content
    */
    void forceUpdate();

    /*!
    ** \brief Get the current datasource
    */
    Datasource::IDatasource* datasource();

    /*!
    ** \brief Get the number of items
    */
    uint size() const;

    /*!
    ** \brief Get if the container is empty
    */
    bool empty() const;

    //! \name Operators
    //@{
    //! Operator +=
    Component& operator+=(Item::IItem::Ptr it);
    //! Operator +=
    Component& operator+=(Item::IItem* it);
    //! Operator <<
    Component& operator<<(Item::IItem::Ptr it);
    //! Operator <<
    Component& operator<<(Item::IItem* it);
    //@}

public:
    //! Event: An item has been selected
    Yuni::Event<void(Item::IItem::Ptr)> onItemSelected;
    //! Event: An item has been double clicked
    Yuni::Event<void(Item::IItem::Ptr)> onItemDblClick;

private:
    /*!
    ** \brief Sync the real component with our data
    */
    void internalUpdateItems();

    void internalAddDatasource(Datasource::IDatasource* ds);

    void onDatasourceClicked(void*);

    //! Event: An item in the listbox has been selected
    void onSelectionChanged(wxCommandEvent& evt);
    //! Event: An item has been double clicked
    void onSelectionDblClick(wxCommandEvent& evt);

    void onDraw(wxPaintEvent& evt);

    /*!
    ** \brief Clear the listbox _and_ the custom data in the same time
    **
    ** \note It seems that wx is unable to delete itself the
    ** attached custom data.
    */
    void internalClearTheListbox();

    void onStudyClosed();

private:
    //! Items
    Item::IItem::List pItems;
    //! The listbox
    wxSimpleHtmlListBox* pListbox;
    //! Sizer for datasources
    wxSizer* pSizerForDatasources;
    //! The search edit
    wxSearchCtrl* pSearchEdit;
    //! The last selected item
    Item::IItem::Ptr pLastSelectedItem;
    //! Datasource
    Datasource::IDatasource::Map pDatasources;
    //! The current datasource
    Datasource::IDatasource* pCurrentDatasource;
    //! Invalidate
    bool pInvalidated;

    // Event table
    DECLARE_EVENT_TABLE();

}; // class Component

} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#include "component.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_H__
