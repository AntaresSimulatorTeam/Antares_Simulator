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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#pragma once

#include <vector>
#include "../renderer.h"
#include "column.h"
#include "../../../../application/study.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class TSmanagement : public IRenderer
{
public:
    TSmanagement();
    virtual ~TSmanagement();

    virtual wxString columnCaption(int colIndx) const override;

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
        return !(!study);
    }

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual wxColour horizontalBorderColor(int x, int y) const;

    void control(wxWindow* control)
    {
        pControl = control;
    }

protected:
    virtual int width() const;
    int height() const;
    void checkLineNumberInColumns();

private:
    void onSimulationTSManagementChanged();

protected:
    const int MAX_NB_OF_LINES = 13;
    std::vector<Column*> columns_;
    wxWindow* pControl;

}; // class TSmanagement

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
