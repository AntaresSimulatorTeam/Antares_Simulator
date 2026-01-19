// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_WXGRID_RENDERER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_WXGRID_RENDERER_H__

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <wx/grid.h>
#include "renderer.h"

namespace Antares::Component::Datagrid
{
class AntaresWxGridRenderer: public wxGridCellFloatRenderer
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

} // namespace Antares::Component::Datagrid

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_WXGRID_RENDERER_H__
