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
#ifndef __ANTARES_TOOLBOX_COMPONENT_MAPNOTEBOOK_H__
#define __ANTARES_TOOLBOX_COMPONENT_MAPNOTEBOOK_H__

#include "notebook.h"
#include <wx/textctrl.h>
#include <antares/logs/logs.h>
namespace Antares
{
namespace Component
{
/*!
** \brief MapNotebook
*/
class MapNotebook : public Notebook, public Yuni::IEventObserver<MapNotebook>
{
    friend class MapTabs;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    ** \param parent The parent window
    */
    MapNotebook(wxWindow* parent, Notebook::Orientation orientation = orLeft);
    ~MapNotebook()
    {
        destroyBoundEvents();
    }

    /*!
    ** \brief remove a page by its name
    */
    void onMapLayerChanged(const wxString* text);
    bool remove(Page* page);

public:
    class TabTextCtrl : public wxTextCtrl
    {
    public:
        TabTextCtrl(Tabs* parent, wxWindowID id, Page* pageToRename) :
         wxTextCtrl((wxWindow*)parent,
                    id,
                    pageToRename->caption(),
                    wxPoint(pageToRename->pBoundingBox.x, pageToRename->pBoundingBox.y),
                    pageToRename->pBoundingBox.GetSize(),
                    wxTE_PROCESS_ENTER),
         pPage(pageToRename),
         pNotebook(&(parent->pNotebook))
        {
            SelectAll();
        };
        ~TabTextCtrl() = default;
        void OnTextEnter(wxCommandEvent& evt);
        void SetPage(Page* p)
        {
            if (p != nullptr)
            {
                pPage = p;
                SetValue(pPage->caption());
                Move(pPage->pBoundingBox.x, pPage->pBoundingBox.y);
                SetSize(pPage->pBoundingBox.GetSize());
                SelectAll();
                Show();
            }
            else
                Hide();
        }

    private:
        Page* pPage;
        Notebook* pNotebook;
    };

public:
    class MapTabs : public Notebook::Tabs
    {
        friend class MapNotebook;

    private:
        class tabButton
        {
        public:
            enum BtnType
            {
                btnLeft,
                btnRight,
                btnNone
            };
            tabButton(std::string imagePath,
                      MapTabs* parentFrame,
                      BtnType t = btnNone,
                      const char* hoverImagePath = nullptr);
            ~tabButton();
            void drawButton(wxDC& dc, int x, int y);
            void onMouseUp(wxMouseEvent&);
            void onMouseEnter();
            void onMouseLeave();
            void onClick(wxMouseEvent&, BtnType t);

            BtnType type;
            wxPoint coords;
            wxBitmap* buttonImage = nullptr;
            wxBitmap* hoverImage = nullptr;
            wxRect* pBoundingBox = nullptr;
            MapTabs* parentTabFrame = nullptr;
            bool isVisible;
            bool drawHover = false;
        };

        int sizingOffset;
        int undrawnLeftTabs;
        int remainingRightTabs;
        tabButton* rightTabScroll;
        tabButton* leftTabScroll;
        tabButton* addPageButton;
        TabTextCtrl* pTabNameCtrl;

    public:
        MapTabs(wxWindow* parent, Notebook& notebook);

        ~MapTabs()
        {
            delete rightTabScroll;
            delete leftTabScroll;
            delete addPageButton;
        }

        void onDraw(wxPaintEvent&);
        void drawOrientationTop(wxDC& dc);
        //! \name Constructor & Destructor
        //@{
        /*!
        ** \brief compute the offset and draw tab scrolling button if tabs don't fit in the window
        ** \
        */
        void doSizing(wxDC& dc);

        void drawItemTop(wxDC& dc, Page* page, int& pos, Notebook::Alignment align);
        void onMouseUp(wxMouseEvent&);
        void onMouseMove(wxMouseEvent&);
        void onMouseLeave(wxMouseEvent&);

        /*!
        ** \brief Event: adding a page
        */
        Yuni::Event<void(int&, int&)> onMouseMotion;

        DECLARE_EVENT_TABLE()
    };

private:
    wxRect closePageBoundingBox;

public:
    /*!
    ** \brief Event: adding a page
    */
    Yuni::Event<> onAddPage;
    /*!
    ** \brief Event: Page deletion
    **
    ** Prototype :
    ** \code
    ** // page : The page to be deleted
    ** \endcode
    */
    Yuni::Event<void(Page&)> onPageDelete;

}; // class MapNotebook

} // namespace Component
} // namespace Antares
#endif
