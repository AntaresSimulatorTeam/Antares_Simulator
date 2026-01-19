// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "drawingcontext.h"

namespace Antares::Map
{
DrawingContext::DrawingContext(wxDC& dc,
                               const wxPoint& origin,
                               const wxPoint& offset,
                               const wxPoint& scroll,
                               const wxPoint& clientSize,
                               const wxPoint& btRight,
                               const size_t& layer,
                               bool forFileExport):
    pDC(dc),
    pOrigin(origin),
    pOffsetForSelectedNodes(offset),
    pScroll(scroll),
    pClientSize(clientSize),
    pBottomRight(btRight),
    layerId(layer),
    pForFileExport(forFileExport)
{
}

} // namespace Antares::Map
