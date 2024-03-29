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

#include "layer.h"

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
Layer::Layer(LayerType t) : index((uint)-1), type(t), detached(false), customSelectionType(stNone)
{
    resetAlternativeCaption();
}

Layer::Layer(const Layer& rhs) :
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

} // namespace OutputViewer
} // namespace Window
} // namespace Antares
