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
#ifndef __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__
# define __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__

# include <yuni/yuni.h>
# include "../../wx-wrapper.h"
# include <wx/frame.h>
# include "frame.h"


namespace Antares
{
namespace Component
{
namespace Frame
{

	class WxLocalFrame : public wxFrame, public IFrame
	{
	public:
		WxLocalFrame();
		WxLocalFrame(wxWindow* parent, wxWindowID id, const wxString& title,
			const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("registered-frame"));

		virtual ~WxLocalFrame();

		virtual bool Destroy();

		virtual bool excludeFromMenu() override;
		virtual void updateOpenWindowsMenu() override;
		virtual void frameRaise() override;
		virtual Yuni::String frameTitle() const override;
		virtual int frameID() const override;

		// Override Frame title
		virtual void SetTitle(const wxString& title) override;

	}; // class WxLocalFrame





} // namespace Frame
} // namespace Component
} // namespace Antares

#endif // __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__
