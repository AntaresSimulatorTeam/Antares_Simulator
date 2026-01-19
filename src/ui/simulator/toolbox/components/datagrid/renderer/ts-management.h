// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include <vector>
#include "../renderer.h"
#include "column.h"
#include "../../../../application/study.h"

namespace Antares::Component::Datagrid::Renderer
{
class TSmanagement: public IRenderer
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

} // namespace Antares::Component::Datagrid::Renderer
