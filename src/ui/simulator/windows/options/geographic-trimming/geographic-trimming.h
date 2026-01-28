// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_H__

#include "../../../toolbox/components/datagrid/component.h"
#include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"
#include <wx/dialog.h>

namespace Antares::Window::Options
{
class geographicTrimming: public wxDialog
{
public:
    geographicTrimming(wxFrame* parent,
                       Component::Datagrid::Renderer::geographicTrimmingGrid* renderer);
    virtual ~geographicTrimming();

protected:
    void mouseMoved(wxMouseEvent& evt);
    void onClose(void*);

private:
    Component::Datagrid::Renderer::geographicTrimmingGrid* pRenderer;

    DECLARE_EVENT_TABLE()

}; // class geographicTrimming

class areasTrimming final: public geographicTrimming
{
public:
    areasTrimming(wxFrame* parent);
}; // class areasTrimming

class linksTrimming final: public geographicTrimming
{
public:
    linksTrimming(wxFrame* parent);
}; // class areasTrimming

} // namespace Antares::Window::Options
#endif
