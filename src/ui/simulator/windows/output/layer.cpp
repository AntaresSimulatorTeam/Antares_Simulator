// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "layer.h"

namespace Antares::Window::OutputViewer
{
Layer::Layer(LayerType t):
    index((uint)-1),
    type(t),
    detached(false),
    customSelectionType(stNone)
{
    resetAlternativeCaption();
}

Layer::Layer(const Layer& rhs):
    index(rhs.index),
    selection(rhs.selection),
    type(rhs.type),
    detached(rhs.detached),
    customSelectionType(rhs.customSelectionType),
    customAreaOrLink(rhs.customAreaOrLink)
{
    resetAlternativeCaption();
}

Layer::~Layer()
{
}

void Layer::resetAlternativeCaption()
{
    switch (type)
    {
    case ltOutput:
        pAlternativeCaption.clear();
        break;
    case ltSum:
        pAlternativeCaption = wxT("Sum");
        break;
    case ltDiff:
        pAlternativeCaption = wxT("Differences");
        break;
    case ltAverage:
        pAlternativeCaption = wxT("Average");
        break;
    case ltMin:
        pAlternativeCaption = wxT("Minimum");
        break;
    case ltMax:
        pAlternativeCaption = wxT("Maximum");
        break;
    }
}

} // namespace Antares::Window::OutputViewer
