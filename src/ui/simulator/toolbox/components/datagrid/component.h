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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_COMPONENT_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_COMPONENT_H__

#include <ui/common/component/panel.h>
#include <antares/date/date.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
// classes forwarding
class VGridHelper;
class DBGrid;
class InternalState;
namespace Renderer
{
class IRenderer;
}

class ComponentRefresh
{
public:
    ComponentRefresh(InternalState*& i) : pInternal(i)
    {
    }
    //! \name Refresh
    //@{
    /*!
    ** \brief Force a complete refresh of the grid
    **
    ** The sub-component Grid (wxGrid) caches some values, like the size
    ** of the grid, and the name of the columns.
    ** This method is especially useful when any important changes occur
    ** (for example another area has just been selected) and the entire grid
    ** is invalidated.
    */
    void forceRefresh();
    //! Force refresh for the next main loop
    void forceRefreshDelayed();

    /*!
    ** \brief Allow refresh
    */
    void enableRefresh(bool enabled);
    //@}
private:
    InternalState*& pInternal;
};

/*!
** \brief A datagrid with virtual values
**
** This datagrid only provides a common interface to deal with 1D-arrays and
** matrices used by the solver.
** It does not store any values, and it is controlled by a `renderer`, which
** is the only one able to provide informations about the grid (values, column names...).
**
** In the most cases, the renderer is attached to an input selector (for example an area)
** to update the GUI accordinly.
*/
class Component : public Panel, public Yuni::IEventObserver<Component>, public ComponentRefresh
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent control
    ** \param renderer The renderer that knows all about the values contained by the datagrid
    */
    Component(wxWindow* parent,
              Renderer::IRenderer* renderer = NULL,
              const wxString& caption = wxEmptyString,
              bool hasFilter = true,
              bool colorMappingRowLabels = true,
              bool copypasteOnly = false,
              bool readonly = false,
              bool hasLayerFilter = false);
    /*!
    ** \brief Destructor
    */
    virtual ~Component();
    //@}

    //! \name Renderer & Grid
    //@{
    //! Get the renderer attached to the datagrid
    Renderer::IRenderer* renderer() const;
    //! Reset the renderer for the datagrid
    void renderer(Renderer::IRenderer* r);

    //! Get the real datagrid component
    DBGrid* grid() const;
    //! Get the real datagrid component as a scrolled window
    wxScrolledWindow* gridAsScrolledWindow();
    //@}

    //! \name Precision
    //@{
    //! Get the precision of the datagrid
    Date::Precision precision() const;
    //! Set the precision of the datagrid
    void precision(const Date::Precision p);
    //@}

    //! \name Clipboard
    //@{
    /*!
    ** \brief Copy the current selection to the clipboard
    */
    void copyToClipboard();
    /*!
    ** \brief Copy the whole datagrid to the clipboard
    */
    void copyAllToClipboard();
    /*!
    ** \brief Paste data from the clipboard
    */
    void pasteFromClipboard();
    //@}

    //! \name Scroll
    //@{
    /*!
    ** \brief Copy the scroll position from another component
    */
    void scroll(wxScrolledWindow* component);
    ;
    //@}

    // For synchronizing scroll with another grid purpose
    void setOtherGrid(Component* other);

    //! \name Modification
    //@{
    //! Get if the component can mark the study as modified if a single cell is modified
    bool markTheStudyAsModified() const;
    //! Set if the component can mark the study as modified if a single cell is modified
    void markTheStudyAsModified(bool flag);
    //@}

private:
    //! Create a toolbar for editing the datagrid
    void createModifyPanel(wxSizer* sizer, bool copypasteOnly, bool readonly);
    //! Create a toolbar for editing the datagrid
    void createModifyPanelValues(wxSizer* sizer, bool copypasteOnly);

    /*!
    ** \brief Display or not the datagrid
    */
    void internalShowDatagrid(bool v);

    void runSelectedAction(uint selectedSet,
                           uint selectedAction,
                           Yuni::String value,
                           VGridHelper* gridHelper);

    /*!
    ** \brief Modify all visible cells
    */
    void onModifyAll(void*);

    //! Ask to the user where to load the matrix
    void onLoadMatrix(void*);
    //! Ask to the user where to save the current matrix
    void onSaveMatrix(void*);

    void onCopyValues(void*);
    void onCopyAllValues(void*);
    void onPasteValues(void*);

    void onCopyValuesFromMenu(wxCommandEvent&);
    void onCopyAllValuesFromMenu(wxCommandEvent&);

    //! Event: The study has been closed
    void onStudyClosed();

    void onBeginUpdate();
    void onEndUpdate();

    void onComboUpdated(wxCommandEvent& evt);

    void createPopupCopy(Button&, wxMenu& menu, void*);

    void onChangeModifierSet(wxCommandEvent& event);
    void onChangeModifierOperator(wxCommandEvent& event);
    void onPopupMenuModifierSet(Button& sender, wxMenu& menu, void*);
    void onPopupMenuModifierOperators(Button&, wxMenu& menu, void*);

private:
    //! Internal states varaibles
    // This variable is mainly used to reduce header dependencies
    InternalState* pInternal;
    // Outside Component instance, for simultaneous actions with the current Component
    Component* otherComponent_ = nullptr;

    // nakama
    friend class InternalState;

}; // class Component

} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_COMPONENT_H__
