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

#include "operator.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{
AOperator::AOperator(AFilterBase* parent, const wxChar* name, const wxChar* caption) :
 pParentFilter(parent), pName(name), pCaption(caption), pSizer(nullptr)
{
    parameters.push_back(Parameter(*this).presetInt());
}

AOperator::~AOperator()
{
}

wxSizer* AOperator::sizer(wxWindow* parent)
{
    // Lazy instanciation
    if (!pSizer)
    {
        pSizer = new wxBoxSizer(wxHORIZONTAL);
        const Parameter::List::iterator end = parameters.end();
        for (Parameter::List::iterator i = parameters.begin(); i != end; ++i)
            pSizer->Add(i->sizer(parent), 0, wxALL | wxEXPAND);
        pSizer->Layout();
    }
    return pSizer;
}

void AOperator::refreshAttachedGrid()
{
    pParentFilter->refreshAttachedGrid();
}

} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares
