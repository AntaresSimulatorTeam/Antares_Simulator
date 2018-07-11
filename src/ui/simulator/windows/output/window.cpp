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

#include "window.h"
#include <wx/sizer.h>
#include <wx/icon.h>
#include "../../toolbox/resources.h"
#include <ui/common/dispatcher/gui.h>


using namespace Yuni;

# define SEP  Yuni::IO::Separator



namespace Antares
{
namespace Window
{
namespace OutputViewer
{

	BEGIN_EVENT_TABLE(Window, wxFrame)
		EVT_CLOSE(Window::onClose)
	END_EVENT_TABLE()




	Window::Window(wxFrame* parent, const OutputViewer::Component* viewer) :
		Antares::Component::Frame::WxLocalFrame(parent, wxID_ANY,
			wxT("Output viewer"),
			wxDefaultPosition, wxSize(1000, 600),
			wxCAPTION|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU|wxRESIZE_BORDER|wxCLIP_CHILDREN)
	{
		wxIcon icon(Resources::WxFindFile("icons/study.ico"), wxBITMAP_TYPE_ICO);
		SetIcon(icon);

		// The main sizer
		auto* sizer = new wxBoxSizer(wxVERTICAL);

		auto* newviewer = new OutputViewer::Component(this, true);
		newviewer->updateLayerList();
		sizer->Add(newviewer, 1, wxALL|wxEXPAND);

		if (viewer)
			newviewer->copyFrom(*viewer);

		auto study = Data::Study::Current::Get();
		if (!(!study))
			SetTitle(wxString(wxT("Output: ")) << wxStringFromUTF8(study->header.caption));

		// Content
		SetSizer(sizer);
		sizer->Layout();
	}


	Window::~Window()
	{
		// Destroy all components before
		if (GetSizer())
			GetSizer()->Clear(true);
	}


	void Window::onClose(wxCloseEvent&)
	{
		if (GetSizer())
			GetSizer()->Clear(true);
		Dispatcher::GUI::Destroy(this);
	}






} // namespace OutputViewer
} // namespace Window
} // namespace Antares
