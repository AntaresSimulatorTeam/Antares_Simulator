// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_SUMMARY_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_SUMMARY_H__

#include "../../renderer.h"
#include <yuni/core/event.h>
#include "../../../../../application/study.h"

namespace Antares::Component::Datagrid::Renderer::Links
{
class Summary: public virtual IRenderer
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

} // namespace Antares::Component::Datagrid::Renderer::Links

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_SUMMARY_H__
