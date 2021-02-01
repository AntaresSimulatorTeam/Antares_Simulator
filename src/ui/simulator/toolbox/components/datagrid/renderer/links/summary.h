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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_SUMMARY_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_SUMMARY_H__

#include <antares/wx-wrapper.h>
#include "../../renderer.h"
#include <yuni/core/event.h>
#include "../../../../../application/study.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
namespace Links
{
class Summary : public virtual IRenderer
{
public:
    Summary(wxWindow* parent);
    virtual ~Summary();

    virtual int width() const
    {
        return 2;
    }
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    {
        // Do nothing
    }

    virtual bool valid() const
    {
        return height() > 0;
    }

    virtual IRenderer::CellStyle cellStyle(int x, int y) const;

protected:
    wxWindow* pControl;

}; // class Summary

} // namespace Links
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_SUMMARY_H__
