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

#include <antares/wx-wrapper.h>
#include <wx/panel.h>
#include <wx/sizer.h>

#include "action-panel.h"
#include <cassert>
#include <wx/dcbuffer.h>
#include <yuni/core/math.h>
#include "../../application/menus.h"


using namespace Yuni;
using namespace Antares::Action;



namespace Antares
{
namespace Private
{
namespace Window
{

	enum
	{
		fontSize = 8,
		gutterLength = 85,
		stateLength = 70,
		captionLength = 315,
	};


	BEGIN_EVENT_TABLE(ActionPanel, Component::Panel)
		EVT_PAINT(ActionPanel::onDraw)
		EVT_ENTER_WINDOW(ActionPanel::onEnter)
		EVT_LEAVE_WINDOW(ActionPanel::onLeave)
		EVT_LEFT_UP(ActionPanel::onMouseDown)
	END_EVENT_TABLE()


	ActionPanel*  ActionPanel::SelectedItem = nullptr;




	ActionPanel::ActionPanel(wxWindow* parent, ActionPanel* parentPanel,
		const Context::Ptr& context, const IAction::Ptr& action)
		:Antares::Component::Panel(parent),
		pParent(parentPanel),
		pContext(context),
		pAction(action), pCollapsed(true),
		pPopupMenu(nullptr),
		pTotalChildrenCount(1)
	{
		assert(!(!pAction));
		SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
		SetSize(50, itemHeight);

		pBackgroundColor = GetBackgroundColour();
		pLineColor.Set(
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Red()   - 70, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Green() - 70, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Blue()  - 70, 0, 255) );
		//pBackgroundColorLight.Set(
		//		(unsigned char) Math::MinMax<int>(pBackgroundColor.Red()   + 8, 0, 255),
		//		(unsigned char) Math::MinMax<int>(pBackgroundColor.Green() + 8, 0, 255),
		//		(unsigned char) Math::MinMax<int>(pBackgroundColor.Blue()  + 8, 0, 255) );
		pBackgroundColorLight.Set(255, 255, 255);

		pStateColor[stDisabled].Set(
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Red()   - 35, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Green() - 35, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Blue()  - 35, 0, 255) );
		pStateColor[stError].Set(
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Red()   + 25, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Green() - 105, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Blue()  - 105, 0, 255) );
		pStateColor[stReady].Set(
				(unsigned char) Math::MinMax<int>(pLineColor.Red()   - 70, 0, 255),
				(unsigned char) Math::MinMax<int>(pLineColor.Green()     , 0, 255),
				(unsigned char) Math::MinMax<int>(pLineColor.Blue()  - 70, 0, 255) );
		pStateColor[stNothingToDo].Set(
				(unsigned char) Math::MinMax<int>(pLineColor.Red()   - 30, 0, 255),
				(unsigned char) Math::MinMax<int>(pLineColor.Green()     , 0, 255),
				(unsigned char) Math::MinMax<int>(pLineColor.Blue()  - 30, 0, 255) );

		pStateColor[stConflict].Set(255, 0, 0);

		update();

		if (action->autoExpand())
		{
			Yuni::Bind<void ()> bind;
			bind.bind(this, &ActionPanel::expand);
			Antares::Dispatcher::GUI::Post(bind);
		}
	}


	ActionPanel::~ActionPanel()
	{
		delete pPopupMenu;
	}


	void ActionPanel::forceUpdate()
	{
		pAction->prepare(*pContext);
		update();
		Antares::Dispatcher::GUI::Refresh(this);
	}


	void ActionPanel::update()
	{
		pBehavior = pAction->behavior();
		pState = pAction->state();

		pDepthSpace     = pAction->depth() * 20;
		pText           = wxStringFromUTF8(pAction->caption());
		pComments       = wxStringFromUTF8(pAction->message());
		pBold           = pAction->bold();
		pHasChildren    = !pAction->leaf();
		pDisabled       = (pState == stDisabled) || (pBehavior == bhSkip);
		pCanDoSomething = pAction->canDoSomething();

		String tmp;
		pAction->behaviorToText(pBehavior, tmp);
		pBehaviorText = wxStringFromUTF8(tmp);

		switch (pState)
		{
			case stUnknown:      pStateText = wxT("unknown"); break;
			case stDisabled:     pStateText = wxT("skip"); break;
			case stError:        pStateText = wxT("error"); break;
			case stNothingToDo:  pStateText = wxT("ready"); break;
			case stReady:        pStateText = wxT("ready"); break;
			case stConflict:     pStateText = wxT("conflict"); break;
			case stMax: break;
		}
	}


	void ActionPanel::relayoutAllParents()
	{
		ActionPanel* object = this;
		wxSizer* lastSizer = nullptr;
		wxWindow* lastParent = nullptr;
		uint incr = 0;
		uint h = 0;
		do
		{
			// asserts
			assert(object);
			assert(object->GetParent());
			assert(object->GetParent()->GetSizer());

			object->computeTotalChildrenCount();
			lastParent = object->GetParent();
			lastSizer  = lastParent->GetSizer();
			h = (object->pTotalChildrenCount) * itemHeight;
			object->SetMinSize(wxSize(wxSIZE_AUTO_WIDTH, (int) h));
			lastSizer->Layout();

			++incr;
		}
		while ((object = object->pParent));

		// redraw the top parent
		lastSizer->Layout();
		lastParent->SetVirtualSize(wxSIZE_AUTO_WIDTH, h);
		Antares::Dispatcher::GUI::Refresh(lastParent);
	}


	void ActionPanel::expand()
	{
		if (!pCollapsed)
			return;

		pCollapsed = false;
		// Alias to the current sizer
		wxSizer* sizer = GetSizer();

		// Create the sizer if needed
		if (!sizer)
		{
			sizer = new wxBoxSizer(wxVERTICAL);
			sizer->AddSpacer(itemHeight);
			SetSizer(sizer);
		}
		else
		{
			sizer->Clear(true);
			sizer->AddSpacer(itemHeight);
		}

		auto end = pAction->end();
		for (auto i = pAction->begin(); i != end; ++i)
		{
			if (!(*i).visible())
				continue;
			ActionPanel* panel = new ActionPanel(this, this, pContext, Action::IAction::Ptr(&(*i)));
			sizer->Add(panel, 0, wxALL|wxEXPAND);
			panel->SetMinSize(wxSize(wxSIZE_AUTO_WIDTH, itemHeight));
			pChildren.push_back(panel);
		}

		// Refresh the size
		relayoutAllParents();
		Antares::Dispatcher::GUI::Refresh(this);

		if (pPopupMenu)
		{
			delete pPopupMenu;
			pPopupMenu = nullptr;
		}
	}


	void ActionPanel::computeTotalChildrenCount()
	{
		pTotalChildrenCount = 1;
		for (uint i = 0; i != pChildren.size(); ++i)
			pTotalChildrenCount += pChildren[i]->pTotalChildrenCount;
	}


	void ActionPanel::collapse()
	{
		if (pCollapsed)
			return;
		pCollapsed = true;
		wxSizer* sizer = GetSizer();
		sizer->Clear(true);
		pChildren.clear();
		relayoutAllParents();
		Antares::Dispatcher::GUI::Refresh(this);

		if (pPopupMenu)
		{
			delete pPopupMenu;
			pPopupMenu = nullptr;
		}
	}


	void ActionPanel::DrawBackgroundWithoutItems(wxWindow& obj, wxDC& dc, const wxRect& rect)
	{
		const wxColour& pBackgroundColor = obj.GetBackgroundColour();
		wxColour pLineColor;
		pLineColor.Set(
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Red()   - 70, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Green() - 70, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Blue()  - 70, 0, 255) );
		wxColour pBorderColor;
		pBorderColor.Set(
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Red()   - 40, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Green() - 40, 0, 255),
				(unsigned char) Math::MinMax<int>(pBackgroundColor.Blue()  - 40, 0, 255) );

		dc.SetPen(wxPen(pBackgroundColor, 1, wxPENSTYLE_SOLID));
		dc.SetBrush(wxBrush(pBackgroundColor, wxBRUSHSTYLE_SOLID));
		dc.DrawRectangle(0, 0, gutterLength, rect.height - 1);

		wxColour pBackgroundColorLight;
		//pBackgroundColorLight.Set(
		//		(unsigned char) Math::MinMax<int>(pBackgroundColor.Red()   + 8, 0, 255),
		//		(unsigned char) Math::MinMax<int>(pBackgroundColor.Green() + 8, 0, 255),
		//		(unsigned char) Math::MinMax<int>(pBackgroundColor.Blue()  + 8, 0, 255) );
		pBackgroundColorLight.Set(255, 255, 255);
		dc.SetPen(wxPen(pBackgroundColorLight, 1, wxPENSTYLE_SOLID));
		dc.SetPen(wxPen(pBorderColor, 1, wxPENSTYLE_SOLID));
		dc.SetBrush(wxBrush(pBackgroundColorLight, wxBRUSHSTYLE_SOLID));
		dc.DrawRectangle(gutterLength, 0, rect.width, rect.height);

		// The gutter
		dc.SetPen(wxPen(pLineColor, 1, wxPENSTYLE_SOLID));
		dc.DrawLine(gutterLength, 0, gutterLength, rect.height);
	}


	void ActionPanel::onDraw(wxPaintEvent&)
	{
		# ifndef YUNI_OS_WINDOWS
		const bool mouseIsOver = (SelectedItem == this);
		# else
		enum
		{
			mouseIsOver = 0, // always disabled in Windows
		};
		# endif

		// The font re-used for each drawing
		static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
		static const wxFont fontBold(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).Bold().FaceName("Tahoma"));
		static const wxFont fontComments(wxFontInfo(fontSize-1).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
		// The DC
		wxAutoBufferedPaintDC dc(this);
		if (!dc.IsOk())
			return;
		// Shifts the device origin so we don't have to worry
		// about the current scroll position ourselves
		PrepareDC(dc);
		if (!dc.IsOk())
			return;

		dc.SetFont(font);

		wxRect rect = this->GetClientSize();
		rect.height = itemHeight;

		dc.SetPen(wxPen(pBackgroundColor, 1, wxPENSTYLE_SOLID));
		dc.SetBrush(wxBrush(pBackgroundColor, wxBRUSHSTYLE_SOLID));
		dc.DrawRectangle(0, 0, gutterLength, rect.height);

		if (!mouseIsOver)
		{
			dc.SetPen(wxPen(pBackgroundColorLight, 1, wxPENSTYLE_SOLID));
			dc.SetBrush(wxBrush(pBackgroundColorLight, wxBRUSHSTYLE_SOLID));
		}
		else
		{
			dc.SetPen(wxPen(wxColour(245, 245, 255), 1, wxPENSTYLE_SOLID));
			dc.SetBrush(wxBrush(wxColour(245, 245, 255), wxBRUSHSTYLE_SOLID));
		}
		dc.DrawRectangle(gutterLength, 0, rect.width, rect.height);

		// The gutter
		dc.SetPen(wxPen(pLineColor, 1, wxPENSTYLE_SOLID));
		dc.DrawLine(gutterLength, 0, gutterLength, rect.height);
		// Separator
		dc.SetPen(wxPen(pBackgroundColor, 1, wxPENSTYLE_DOT));
		dc.DrawLine(gutterLength + 1, rect.height - 1, rect.width - 1, rect.height - 1);
		dc.DrawLine(gutterLength + stateLength, 1, gutterLength + stateLength, rect.height - 2);

		// State
		if (pCanDoSomething)
		{
			dc.SetTextForeground(pStateColor[pState]);
			const wxSize size = dc.GetTextExtent(pStateText);
			dc.DrawText(pStateText, gutterLength + stateLength / 2 - size.GetWidth() / 2, (rect.height >> 1) - (size.GetHeight() >> 1));
		}

		// Caption
		{
			if (pBold)
				dc.SetFont(fontBold);

			const wxSize size = dc.GetTextExtent(pText);
			if (!pDisabled)
				dc.SetTextForeground(wxColour(20,20,20));
			else
				dc.SetTextForeground(pLineColor);
			dc.DrawText(pText, stateLength + gutterLength + pDepthSpace + 23, (rect.height >> 1) - (size.GetHeight() >> 1));
		}
		// restore default font
		dc.SetFont(font);

		// Behavior
		if (pCanDoSomething)
		{
			dc.SetTextForeground(wxColour(50,50,50));
			const wxSize size = dc.GetTextExtent(pBehaviorText);
			dc.DrawText(pBehaviorText, gutterLength - 16 - size.GetWidth(), (rect.height >> 1) - (size.GetHeight() >> 1));
		}

		// arrows, for the behavior selector + collapse/expand
		drawArrows(dc, rect);

		// Comments
		{
			// redraw the last surface to avoid collisions
			if (!mouseIsOver)
			{
				dc.SetPen(wxPen(pBackgroundColorLight, 1, wxPENSTYLE_SOLID));
				dc.SetBrush(wxBrush(pBackgroundColorLight, wxBRUSHSTYLE_SOLID));
			}
			else
			{
				dc.SetPen(wxPen(wxColour(245, 245, 255), 1, wxPENSTYLE_SOLID));
				dc.SetBrush(wxBrush(wxColour(245, 245, 255), wxBRUSHSTYLE_SOLID));
			}
			dc.DrawRectangle(stateLength + gutterLength + captionLength - 7, 0, rect.width, rect.height - 1);

			dc.SetPen(wxPen(pBackgroundColor, 1, wxPENSTYLE_DOT));
			dc.DrawLine(gutterLength + stateLength + captionLength, 1, gutterLength + stateLength + captionLength, rect.height - 3);

			if (!pComments.empty())
			{
				dc.SetFont(fontComments);
				const wxSize size = dc.GetTextExtent(pComments);
				if (!pDisabled)
					dc.SetTextForeground(wxColour(50,50,50));
				else
					dc.SetTextForeground(pLineColor);
				dc.DrawText(pComments, stateLength + gutterLength + captionLength + 10,
							(rect.height >> 1) - (size.GetHeight() >> 1));
			}
		}
	}


	void ActionPanel::drawArrows(wxDC& dc, const wxRect& rect)
	{
		wxPoint pPts[3];
		dc.SetPen(wxPen(pLineColor, 1, wxPENSTYLE_SOLID));
		dc.SetBrush(wxBrush(pLineColor));

		if (pHasChildren)
		{
			if (pCollapsed)
			{
				pPts[0].x = gutterLength + stateLength + pDepthSpace + 14;
				pPts[0].y = (rect.height >> 1);
				pPts[1].x = pPts[0].x - 3;
				pPts[1].y = pPts[0].y - 3;
				pPts[2].x = pPts[0].x - 3;
				pPts[2].y = pPts[0].y + 3;
			}
			else
			{
				pPts[0].x = gutterLength + stateLength + pDepthSpace + 12;
				pPts[0].y = (rect.height >> 1) + 2;
				pPts[1].x = pPts[0].x - 3;
				pPts[1].y = pPts[0].y - 3;
				pPts[2].x = pPts[0].x + 3;
				pPts[2].y = pPts[0].y - 3;
			}
			dc.DrawPolygon(3, pPts);
		}

		// Behavior arrows
		if (pCanDoSomething)
		{
			pPts[0].x = gutterLength - 10;
			pPts[0].y = (rect.height >> 1) + 4;
			pPts[1].x = pPts[0].x - 2;
			pPts[1].y = pPts[0].y - 2;
			pPts[2].x = pPts[0].x + 2;
			pPts[2].y = pPts[0].y - 2;
			dc.DrawPolygon(3, pPts);

			pPts[0].x = gutterLength - 10;
			pPts[0].y = (rect.height >> 1) - 3;
			pPts[1].x = pPts[0].x - 2;
			pPts[1].y = pPts[0].y + 2;
			pPts[2].x = pPts[0].x + 2;
			pPts[2].y = pPts[0].y + 2;
			dc.DrawPolygon(3, pPts);
		}
	}


	void ActionPanel::onEnter(wxMouseEvent& evt)
	{
		# ifndef YUNI_OS_WINDOWS
		SelectedItem = this;
		Antares::Dispatcher::GUI::Refresh(this);
		# endif
		evt.Skip();
	}


	void ActionPanel::onLeave(wxMouseEvent& evt)
	{
		# ifndef YUNI_OS_WINDOWS
		SelectedItem = nullptr;
		Antares::Dispatcher::GUI::Refresh(this);
		# endif
		evt.Skip();
	}


	void ActionPanel::onMouseDownCollapseExpand()
	{
		// removed useless menu
		if (pPopupMenu)
		{
			delete pPopupMenu;
			pPopupMenu = nullptr;
		}

		Yuni::Bind<void ()> bind;
		if (pCollapsed)
			bind.bind(this, &ActionPanel::expand);
		else
			bind.bind(this, &ActionPanel::collapse);
		Antares::Dispatcher::GUI::Post(bind);
	}


	void ActionPanel::onMouseDownBehaviorSelect(wxWindow* obj)
	{
		using namespace ::Antares::Action;

		if (!obj)
			return;
		if (!pPopupMenu)
		{
			pPopupMenu = new wxMenu();
			wxMenuItem* item;

			item = Menu::CreateItem(pPopupMenu, wxID_ANY, wxString() << pText << wxT("   "),
				"images/16x16/empty.png");
			item->Enable(false);
			pPopupMenu->AppendSeparator();

			String tmp;
			if (pAction->allowUpdate())
			{
				pAction->behaviorToText(bhMerge, tmp);
				item = Menu::CreateItem(pPopupMenu, wxID_ANY, wxStringFromUTF8(tmp) << wxT("  "));
				pPopupMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(ActionPanel::onBehaviorMerge), nullptr, this);
			}

			if (pAction->allowOverwrite())
			{
				pAction->behaviorToText(bhOverwrite, tmp);
				item = Menu::CreateItem(pPopupMenu, wxID_ANY, wxStringFromUTF8(tmp) << wxT("  "));
				pPopupMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(ActionPanel::onBehaviorOverwrite), nullptr, this);
			}

			if (pAction->allowSkip())
			{
				pAction->behaviorToText(bhSkip, tmp);
				item = Menu::CreateItem(pPopupMenu, wxID_ANY, wxStringFromUTF8(tmp) << wxT("  "));
				pPopupMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(ActionPanel::onBehaviorSkip), nullptr, this);
			}
		}

		assert(pPopupMenu);
		PopupMenu(pPopupMenu, 10, 0);
	}


	void ActionPanel::onBehaviorMerge(wxCommandEvent&)
	{
		using namespace Antares::Action;
		pAction->behavior(bhMerge);
		prepareAll();
	}


	void ActionPanel::onBehaviorSkip(wxCommandEvent&)
	{
		using namespace Antares::Action;
		pAction->behavior(bhSkip);
		prepareAll();
	}


	void ActionPanel::onBehaviorOverwrite(wxCommandEvent&)
	{
		using namespace Antares::Action;
		pAction->behavior(bhOverwrite);
		prepareAll();
	}


	void ActionPanel::prepareAll(bool force)
	{
		if (!force && pAction->shouldPrepareRootNode())
		{
			pAction->prepareRootNode(*pContext);

			ActionPanel* object = this;
			do
			{
				if (!object->pParent)
					break;
				object = object->pParent;
			}
			while (true);
			object->prepareAll(true);
		}
		else
		{
			pAction->prepare(*pContext);
			for (uint i = 0; i != pChildren.size(); ++i)
				pChildren[i]->prepareAll(true);
			update();
			Antares::Dispatcher::GUI::Refresh(this);
		}
	}


	void ActionPanel::onMouseDown(wxMouseEvent& evt)
	{
		enum { offsetArrow = gutterLength + stateLength, };

		// Mouse position
		const uint x = (uint) evt.GetX();
		const uint y = (uint) evt.GetY();

		// collapse / expand
		if (pHasChildren && y <= itemHeight && x >= offsetArrow + pDepthSpace + 2 && x <= offsetArrow + pDepthSpace + 20)
		{
			onMouseDownCollapseExpand();
			evt.Skip();
			return;
		}

		// behavior
		if (x < gutterLength && x > 8)
		{
			onMouseDownBehaviorSelect(dynamic_cast<wxWindow*>(evt.GetEventObject()));
			evt.Skip();
			return;
		}
		evt.Skip();
	}


	void ActionPanel::SetFocus()
	{
		// Do nothing, better than doing whatever you don't want
	}




} // namespace Window
} // namespace Private
} // namespace Antares

