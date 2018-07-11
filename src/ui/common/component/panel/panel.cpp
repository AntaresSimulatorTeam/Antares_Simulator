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

#include "panel.h"
#include "../../lock.h"
#include <wx/sizer.h>

using namespace Yuni;



namespace Antares
{
namespace Component
{

	static Panel* gLastWindowUnderMouse = nullptr;




	Panel::Panel(wxWindow* parent) :
		wxPanel(parent, wxID_ANY, wxDefaultPosition,wxDefaultSize,
			wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxFULL_REPAINT_ON_RESIZE)
	{
		assert(parent && "invalid parent");

		Connect(GetId(), wxEVT_MOTION,
			wxMouseEventHandler(Panel::onInternalMotion), NULL, this);
		Connect(GetId(), wxEVT_LEFT_DOWN,
			wxMouseEventHandler(Panel::onInternalMouseDown), NULL, this);
		Connect(GetId(), wxEVT_LEFT_UP,
			wxMouseEventHandler(Panel::onInternalMouseUp), NULL, this);
	}


	Panel::~Panel()
	{
		if (gLastWindowUnderMouse == this)
			gLastWindowUnderMouse = nullptr;

		// To avoid corrupt vtable in some rare cases / misuses
		// (when children try to access to this object for example),
		// we should destroy all children as soon as possible.
		wxSizer* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
	}


	void Panel::onInternalMouseDown(wxMouseEvent& evt)
	{
		onMouseDown(evt);
	}


	void Panel::onInternalMouseUp(wxMouseEvent& evt)
	{
		if (GUIIsLock())
			return;

		onMouseUp(evt);

		if (triggerMouseClickEvent())
		{
			// Delay the click
			Yuni::Bind<void ()> callback;
			callback.bind(this, &Panel::onMouseClick);
			Dispatcher::GUI::Post(callback, 50);
		}
	}


	void Panel::OnMouseMoveFromExternalComponent()
	{
		if (gLastWindowUnderMouse && not GUIIsLock())
		{
			gLastWindowUnderMouse->onMouseLeave();
			gLastWindowUnderMouse = nullptr;
		}
	}


	void Panel::onInternalMotion(wxMouseEvent& evt)
	{
		if (this == gLastWindowUnderMouse || GUIIsLock())
			return;
		if (gLastWindowUnderMouse)
			gLastWindowUnderMouse->onMouseLeave();
		gLastWindowUnderMouse = this;
		gLastWindowUnderMouse->onMouseEnter();

		const auto& position = evt.GetPosition();
		onMouseMoved(position.x, position.y);
	}




} // namespace Component
} // namespace Antares

