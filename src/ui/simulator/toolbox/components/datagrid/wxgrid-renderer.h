/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_WXGRID_RENDERER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_WXGRID_RENDERER_H__

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <wx/grid.h>
#include "renderer.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
class AntaresWxGridRenderer : public wxGridCellFloatRenderer
{
public:
    AntaresWxGridRenderer();

    virtual ~AntaresWxGridRenderer()
    {
    }

    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& r,
                      int row,
                      int col,
                      bool isSelected);

public:
    Renderer::IRenderer* renderer;

}; // class AntaresWxGridRenderer

} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_WXGRID_RENDERER_H__
