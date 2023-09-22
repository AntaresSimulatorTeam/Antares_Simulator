/*
** Copyright 2007-2023 RTE
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

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_ADEQUACY_PATCH_AREAS_GRID_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_ADEQUACY_PATCH_AREAS_GRID_H__

#include "../renderer.h"
#include "../../../../application/study.h"

namespace Antares::Component::Datagrid::Renderer
{
class AdequacyPatchAreaGrid : public IRenderer
{
public:
    AdequacyPatchAreaGrid();
    ~AdequacyPatchAreaGrid() override;

    int width() const override
    {
        return 1;
    }
    virtual uint gridSize() const;

    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    void resetColors(int, int, wxColour&, wxColour&) const override
    {
        // Do nothing
    }

    virtual bool valid() const;

    uint maxWidthResize() const override
    {
        return 0;
    }
    IRenderer::CellStyle cellStyle(int col, int row) const override;

    void control(wxWindow* control)
    {
        pControl = control;
    }

    //! An item has been updated
    Yuni::Bind<void()> onTriggerUpdate;

private:
    wxWindow* pControl;

}; // class AdequacyPatchAreaGrid

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_ADEQUACY_PATCH_AREAS_GRID_H__
