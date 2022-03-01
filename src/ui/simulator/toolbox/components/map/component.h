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
#ifndef __ANTARES_TOOLBOX_MAP_COMPONENT_H__
#define __ANTARES_TOOLBOX_MAP_COMPONENT_H__

#include <antares/wx-wrapper.h>
#include <antares/study.h>
#include "control.h"
#include <wx/stattext.h>
#include <ui/common/component/panel.h>
#include "../button.h"
#include "../notebook/mapnotebook.h"
#include "settings.h"

#include <wx/textctrl.h>

namespace Antares
{
namespace Map
{
/*!
** \brief Standard study MAP
*/
class Component final : public Antares::Component::Panel, public Yuni::IEventObserver<Component>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Component(wxWindow* parent);
    /*!
    ** \brief Destructor
    */
    virtual ~Component();
    //@}

    //! \name Study
    //@{
    /*!
    ** \brief Attach a study to the map to reflect changes
    **
    ** The user is responsible for deleting the study. However
    ** to not forget to detach it just before
    **
    ** \param study The study to attach
    */
    void attachStudy(Data::Study::Ptr study);

    /*!
    ** \brief Detach the study (if any) currently attached to the map
    */
    void detachStudy(bool canRefresh = true);

    /*!
    ** \brief Get the study attached to the map
    ** \return A pointer to a Study structure, or NULL
    */
    Data::Study::Ptr attachedStudy();
    //@}

    //! \name Import Export
    //@{
    /*!
    ** \brief Load the map's settings from an existing Antares study
    **
    ** \param study The study to import the data from
    ** \return True if the operation succeeded, False otherwise
    */
    bool loadFromStudy(Data::Study& study);

    /*!
    ** \brief Load the map's settings from the attached study
    ** \return True if the operation succeeded, False otherwise
    */
    bool loadFromAttachedStudy();

    /*!
    ** \brief Save the map's settings to a study
    **
    ** \param study The target study
    ** \param incremental Modify the study only for detected changes
    ** \return True if the operation succeded, False otherwise
    */
    bool saveToStudy(Data::Study& study, bool incremental = true);

    /*!
    ** \brief Save the map's settings to the attached study (if any)
    **
    ** \param incremental Modify the study only for detected changes
    ** \return True if the operation succeeded, False otherwise
    */
    bool saveToAttachedStudy(bool incremental = true);
    //@}

    /*!
    ** \brief Save the map into an image file
    **
    ** \param filePath
    ** \return True if the operation succeeded, False otherwise
    */
    bool saveToImageFile(const AnyString& filePath, const MapRenderOptions& options);
    //@}

    void clear();

    //! \name UI
    //@{
    /*!
    ** \brief Recenter scrollbars to the center of the map
    */
    void recenterView();

    void onCenterXY(void*);

    /*!
    ** \brief Set the focus to the map
    */
    void setFocus();

    /*!
    ** \brief Get how many areas have been selected
    */
    uint selectedAreaCount() const;
    //@}

    void drawerVisible(bool v);

    void showLayerAll();

    void invalidate();
    void refresh();

    /*!
    ** \brief Refresh the header informations (how many areas/connexions)
    */
    void refreshHeaderInformations();

    void renameNodeFromArea(const Data::Area* area);

    //! \name Edition
    //@{
    /*!
    ** \brief Select all nodes of type "area"
    */
    void unselectAll(bool canRefresh = true);

    /*!
    ** \brief Select all nodes of type "area"
    */
    void selectAll();

    /*!
    ** \brief Reverse the selection
    */
    void reverseSelection();

    /*!
    ** \brief Select only items from a list of areas
    */
    void selectOnly(const Data::Area::Vector& areas);

    /*!
    ** \brief Select only items from a list of areas and links
    */
    void selectOnly(const Data::Area::Vector& areas, const Data::AreaLink::Vector& links);
    //@}

    //! \name Cache management
    //@{
    /*!
    ** \brief Refresh the internal cache about the color of a single area
    */
    void reimportNodeColors(const Data::Area* area);

    /*!
    ** \brief Refresh the internal cache about the X-coordinate  of a single area
    */
    void moveNodeFromAreaX(const Data::Area* area, int x);

    /*!
    ** \brief Refresh the internal cache about the X-coordinate  of a single area
    */
    void moveNodeFromAreaY(const Data::Area* area, int y);
    //@}

    size_t getActiveLayerID()
    {
        return pMapActiveLayer ? pMapActiveLayer->getUid() : 0;
    }

public:
    //! Popup event
    Yuni::Event<void(int, int)> onPopupEvent;
    //! Status bar event
    Yuni::Event<void(const wxString&)> onStatusBarText;
    //! Event: Double-Click on a item
    Yuni::Event<void(Component&)> onDblClick;
    //! Event: Enter in a text item
    // Yuni::Event<void (void*)> onTextEnter;

private:
    class Drawer final : public Antares::Component::Panel
    {
    public:
        Drawer(wxWindow* parent, Component& com) :
         Antares::Component::Panel(parent), pComponent(com)
        {
        }
        void onDraw(wxPaintEvent& evt);
        void onSize(wxSizeEvent& evt);

    private:
        void drawFromDevice(wxDC& dc);

    private:
        Component& pComponent;
        DECLARE_EVENT_TABLE()
    };

    void onApplicationQuit();

    void onPageChanged(Antares::Component::MapNotebook::Page& page);

    Antares::Component::MapNotebook::Page* addNewLayer(wxString pageName = wxString(""),
                                                       size_t uID = 0);
    void addNewEmptyLayer()
    {
        addNewLayer();
    }
    void removeLayer(Antares::Component::MapNotebook::Page& page);

    void evtOnPopupEvent(int x, int y);

    void evtOnSelectionHide(wxCommandEvent&);

    void evtOnSelectionShow(wxCommandEvent&);

    void onCopy(void*);
    void onCopyDropdown(Antares::Component::Button&, wxMenu& menu, void*);
    void evtPopupCopy(wxCommandEvent&);
    void evtPopupCopyAll(wxCommandEvent&);
    void evtPopupCopyAllAreas(wxCommandEvent&);
    void evtPopupCopyAllLinks(wxCommandEvent&);
    void onPaste(void*);
    void onPasteDropdown(Antares::Component::Button&, wxMenu& menu, void*);
    void evtPopupPaste(wxCommandEvent&);
    void evtPopupPasteSpecial(wxCommandEvent&);
    void onSelectAll(void*);

    void onNew(void*);
    void onNewDropdown(Antares::Component::Button&, wxMenu& menu, void*);

    template<unsigned int N>
    inline void evtNewArea(wxCommandEvent&)
    {
       if (pMapActiveLayer)
           pMapActiveLayer->addNewNode(N);
       setFocus();
    }

    void onToggleMouseSelectionArea(void*);
    void onToggleMouseSelectionLink(void*);
    void onToggleMouseSelectionPlant(void*);
    void onToggleMouseSelectionConstraint(void*);

    void onCenter(void*);

    void onEvtCenterXY(wxCommandEvent& WXUNUSED(event));

private:
    Control* pMapActiveLayer;
    Antares::Component::MapNotebook* pNoteBook;
    std::vector<Control*> mapLayersPtrList;

    wxStaticText* pInfosAreaCount;
    wxStaticText* pInfosConnxCount;
    // Toggle buttons
    // The class wxWindow is used here to not include button.h everywhere
    // It is already such a pain to compile on Windows...
    //! The toggle button for selecting areas with the mouse
    wxWindow* pBtnSelectionArea;
    //! The toggle button for selecting links with the mouse
    wxWindow* pBtnSelectionLink;
    //! The toggle button for selecting clusters with the mouse
    wxWindow* pBtnSelectionPlant;
    //! The toggle button for selecting constraints with the mouse
    wxWindow* pBtnSelectionConstraint;

    // PopUp Menu
    wxMenu* pSelectionPopUpMenu;

}; // class Component

} // namespace Map
} // namespace Antares

#endif // __ANTARES_TOOLBOX_MAP_COMPONENT_H__
