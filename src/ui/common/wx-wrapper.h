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
#ifndef __ANTARES_TOOLBOX_WX_WIDGETS_H__
# define __ANTARES_TOOLBOX_WX_WIDGETS_H__

# if defined(__cplusplus)
#	include <yuni/yuni.h>
#	include <yuni/core/string.h>
#	include <antares/study/fwd.h>
# endif


# if defined(__cplusplus)


//// For compilers that support precompilation, includes "wx/wx.h".
//# include "wx/wxprec.h"

// forward declarations
class wxScrolledWindow;

#include <wx/defs.h>
#include <wx/object.h>
#include <wx/dynarray.h>
#include <wx/list.h>
#include <wx/hash.h>
#include <wx/string.h>
#include <wx/hashmap.h>
#include <wx/arrstr.h>
#include <wx/intl.h>
#include <wx/event.h>
#include <wx/app.h>
#include <wx/utils.h>
#include <wx/stream.h>
#include <wx/memory.h>
#include <wx/window.h>

#include "wx/validate.h"

# if !wxUSE_MENUS
#	error "Requires wxUSE_MENUS=1"
# endif // wxUSE_MENUS

/* not all ports have support for EVT_CONTEXT_MENU yet, don't define
** USE_CONTEXT_MENU for those which don't
*/
# if defined(__WXMOTIF__) || defined(__WXPM__) || defined(__WXX11__) || defined(__WXMGL__)
#	define USE_CONTEXT_MENU 0
# else
#	define USE_CONTEXT_MENU 1
# endif



# ifndef wxUSE_LIBPNG
#	error "The wxWidgets library must be compiled with the PNG support"
# endif


/*!
** \brief Convert any standard string into a wxString
**
** \param s The string to convert
*/
// std::string
wxString wxStringFromUTF8(const std::string& s);
// C-String
wxString wxStringFromUTF8(const char* s);
// Fixed length
wxString wxStringFromUTF8(const char* const s, uint length);
// Yuni::String
wxString wxStringFromUTF8(const Yuni::String& s);
// Yuni::CString
template<uint ChunkT, bool FixedT>
wxString wxStringFromUTF8(const Yuni::CString<ChunkT, FixedT>& s);


/*!
** \brief Convert an Unicode wxString into a standard string from the STL
*/
template<class StringT>
void wxStringToString(const wxString& s, StringT& out);

/*!
** \brief Convert a double into a wxString
*/
wxString DoubleToWxString(double f);

/*!
** \brief Convert a double into a wxString with a given precision (static)
*/
template<int PrecisionT> wxString DoubleToWxStringS(double f);

/*!
** \brief Convert a double into a wxString with a given precision (dynamic)
*/
wxString DoubleToWxString(double f, uint precision);


/*!
** \brief Convert a wxString into a double
*/
double wxStringToDouble(const wxString& s, double defValue = 0.);



/*!
** \brief Append a text with quotes
*/
template<class StringT1, class StringT2>
void AppendWithQuotes(StringT1& out, const StringT2& text);


/*!
** \brief Try to find the wxFrame parent of a control
*/
wxWindow* wxFindFrameParent(wxWindow* control);



# include "wx-wrapper.hxx"
# include "fwd.h"
# include "dispatcher.h"

# else /* C++ */

# include <yuni/yuni.h>

# endif


#endif /* __ANTARES_TOOLBOX_WX_WIDGETS_H__ */
