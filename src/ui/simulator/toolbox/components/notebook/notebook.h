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
#ifndef __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_H__
# define __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_H__

# include <antares/wx-wrapper.h>
# include <ui/common/component/panel.h>
# include <vector>
# include <wx/dc.h>
# include "../../input/input.h"
# include "../refresh.h"



namespace Antares
{
namespace Component
{

	/*!
	** \brief Notebook
	*/
	class Notebook : public Panel
	{
	public:
		enum Orientation
		{
			orLeft,
			orTop,
		};
		enum Theme
		{
			themeDefault,
			themeLight,
		};

		enum Alignment
		{
			alignRight,
			alignLeft,
		};

	public:
		class Tabs;

	public:
		class Page
		{
			friend class Tabs;
			friend class Notebook;
			friend class MapNotebook;
		public:
			//! \name Constructors & Destructor
			//@{
			Page(Notebook& notebook);
			Page(Notebook& notebook, wxWindow* ctrnl, const wxString& caption);
			Page(Notebook& notebook, wxWindow* ctrnl, const wxString& name, const wxString& caption);
			//! Destructor
			~Page();
			//@}

			//! \name Notebook
			//@{
			//! Get the attached notebook
			Notebook& notebook();
			const Notebook& notebook() const;
			//@}

			//! \name Caption
			//@{
			//! Get the caption of the page
			const wxString& caption() const;
			//! Set the caption of the page
			void caption(const wxString& s);
			//@}

			//! \name Name
			//@{
			//! Get the name of the page
			const wxString& name() const;
			//! Set the name of the page
			void name(const wxString& s);
			//@}

			//! \name Selection
			//@{
			//! Get if the page is currently selected
			bool selected() const {return pSelected;}
			//! Select the page
			Page* select(bool force = false);
			//! Select a subpage of this page
			void selectSubPage(Page* subPage);
			//@}

			//! \name Visible
			//@{
			bool visible() const {return pVisible;}
			void visible(const bool v);
			//@}

			//! \name Controls
			//@{
			bool displayExtraControls() const {return pDisplayExtraControls;}
			void displayExtraControls(bool v) {pDisplayExtraControls = v;}
			//! Get the control of the page
			wxWindow* control() const {return pControl;}

			//! Refresh
			void refresh() {RefreshAllControls(pControl);}
			//@}

			//! \name subpage management
			//@{
			//! Remove a subpage
			void removeSubPage(Page* p);
			//! Add a page as subpage
			Page* addSubPage(Page* p);
			//@}

		private:
			//! Subpages
			std::vector<Page*> subPages;

		private:
			//! Select the page (delayed)
			void onSelectPage();

		private:
			//! The parent notebook
			Notebook& pNotebook;
			//! The attached control (window)
			wxWindow* pControl;
			//! Name of the page
			wxString pName;
			//! Caption of the page
			wxString pCaption;
			//! Bounding box of the button
			wxRect pBoundingBox;
			//! Selected
			bool pSelected;
			//! Visible
			bool pVisible;
			//! Display extra controls
			bool pDisplayExtraControls;

		}; // class Page

		friend class Page;


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		** \param parent The parent window
		*/
		Notebook(wxWindow* parent, Orientation orientation = orLeft);
		//! Destructor
		virtual ~Notebook();
		//@}

		//! \name Orientation
		//@{
		//! Get the orientation of the notebook
		Orientation orientation() const;
		//@}

		/*!
		** \brief Delete all pages
		*/
		void clear();

		/*!
		** \brief Add a new page
		*/
		Page* add(wxWindow* ctnrl, const wxString& caption);

		/*!
		** \brief Add a new page
		*/
		Page* add(wxWindow* ctnrl, const wxString& name, const wxString& caption);

		/*!
		** \brief Add a separator
		*/
		void addSeparator();

		//! \name Graphical behavior
		//@{
		//! Get if the tabs are visible
		bool tabsVisible() const;
		//! Set if the tabs are visible
		void tabsVisible(bool v);

		//! Get if the tabs must always be displayed, even if a single one is available
		bool alwaysDisplayTabs() const;
		//! Set if the tabs must always be displayed
		void alwaysDisplayTabs(const bool v);

		bool displayTitle() const;
		void displayTitle(const bool v);
		//@}


		void addCommonControl(Toolbox::InputSelector::AInput* input, const int border = 0);
		void addCommonControl(wxWindow* ctrnl, const int border = 0, const wxPoint& recommendedSize = wxPoint(20,20));

		void addCommonControlTop(Toolbox::InputSelector::AInput* input, const int border = 0);
		void addCommonControlTop(wxWindow* ctrnl, const int border = 0, const wxPoint& recommendedSize = wxPoint(20,20));

		//! \name Caption of the notebook
		//@{
		//! Get the caption of the notebook
		const wxString& caption() const;
		//! Set the caption
		void caption(const wxString& s);
		//@}


		/*!
		** \brief Select a page by its name
		*/
		bool select(const wxString& name, bool triggerEvents = true);

		Page* selected() {return pLastSelected;}
		const Page* selected() const {return pLastSelected;}

		/*!
		** \brief Find a page by its name
		*/
		Page* find(const wxString& name, bool warn = true);


		//! \name Theme
		//@{
		/*!
		** \brief Set the current theme
		*/
		void theme(Theme t) {pTheme = t;}

		Theme theme() const {return pTheme;}
		//@}

		void alignment(Alignment a) {pAlignment = a;}

		/*!
		** \brief Get the controls of the main bar
		*/
		wxWindow* titlePanelControl() const;

		/*!
		** \brief Enable / Disable the refresh for all datagrid
		*/
		void enableRefreshForAllDatagrid(bool enabled);

		// From wxWidgets standard API
		virtual bool HasMultiplePages() const;

		void forceRefresh();

	public:
		/*!
		** \brief Event: Page changing
		**
		** Prototype :
		** \code
		** // page : The new selected page
		** // accept : True to accept the selection, false for a veto
		** void eventReceiverOnPageChanged(Page& page, bool& accept);
		** \endcode
		*/
		Yuni::Event<void (Page&, bool&)> onPageAccept;

		/*!
		** \brief Event: Page changing
		**
		** Prototype :
		** \code
		** // page : The new selected page
		** // accept : True to accept the selection, false for a veto
		** void eventReceiverOnPageChanged(Page& page, bool& accept);
		** \endcode
		*/
		Yuni::Event<void (Page&)> onPageChanged;

	public:
		class Tabs : public Panel
		{
			friend class Notebook;
		public:
			Tabs(wxWindow* parent, Notebook& notebook);
			virtual ~Tabs();

			virtual void onMouseUp(wxMouseEvent&);
			void drawItem(wxDC& dc, Page* page, int& pos);
			virtual void drawItemTop(wxDC& dc, Page* page, int& pos, Alignment align);
			void drawItemOnCanvasNotSelected(Page* page, wxDC& dc, const int pos, const int h, const wxSize& textExtent);
			void drawItemOnCanvasSelected(Page* page, wxDC& dc, const int pos, const int h, const wxSize& textExtent);

			Notebook& pNotebook;
		protected:
			virtual void onDraw(wxPaintEvent& evt);
			void onEraseBackground(wxEraseEvent& evt);
			void drawOrientationLeft(wxDC& dc);
			void drawOrientationTop(wxDC& dc);
			void onForceRefresh();
			wxPoint pCachedSize;
			
			wxRect pRect;
			int pMaxFound;
			// A SegV occurs for an unknon reason when using a wxPoint on the stack
			// when calling DrawPolygon.
			// (MinGW TDM 4.4.1 - Windows XP - wxWidgets 2.8.10)
			// It seems that the drawing context uses it later
			wxPoint pPts[3];

			DECLARE_EVENT_TABLE()
		};

	private:
		//! Refresh the component (delayed)
		void onForceRefresh();

	public:
		friend class Tabs;
		//! Orientation
		const Orientation pOrientation;
		//! All pages
		typedef std::vector<Page*> Pages;
		//! All pages
		Pages pPages;
		//! The tab component
		Tabs* pTabs;
		//! The caption of the notebook
		wxString pCaption;
		//! Can display tabs
		bool pTabsVisible;
		//! Always display tabs
		bool pAlwaysDisplayTab;
		//! Display the title
		bool pDisplayTitle;

		wxSizer* pSizerForPages;
		wxSizer* pTopSizer;
		wxSizer* pLeftSizer;

		Page* pLastSelected;

		//! The complete list of extra controls
		std::vector<wxWindow*> pExtraControls;

		Theme pTheme;
		Alignment pAlignment;

		wxString pCacheTitle;
		wxColour pOriginalColor;
		wxColour pOriginalColorDark;

	}; // class Notebook





} // namespace Component
} // namespace Antares

# include "notebook.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_NOTEBOOK_H__
