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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__

#include "../renderer.h"
#include "../../../../application/study.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class MCPlaylist : public IRenderer
{
public:
    enum MCPlaylistCol
    {
        STATUS,
        WEIGHT
    };

    MCPlaylist();
    virtual ~MCPlaylist();

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

    virtual bool valid() const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    void control(wxWindow* control)
    {
        pControl = control;
    }

public:
    //! An item has been updated
    Yuni::Bind<void()> onTriggerUpdate;

protected:
    wxWindow* pControl;

}; // class MCPlaylist

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__
