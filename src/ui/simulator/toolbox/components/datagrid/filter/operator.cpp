// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "operator.h"

namespace Antares::Toolbox::Filter::Operator
{
AOperator::AOperator(AFilterBase* parent, const wxChar* name, const wxChar* caption):
    pParentFilter(parent),
    pName(name),
    pCaption(caption),
    pSizer(nullptr)
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
        {
            pSizer->Add(i->sizer(parent), 0, wxALL | wxEXPAND);
        }
        pSizer->Layout();
    }
    return pSizer;
}

void AOperator::refreshAttachedGrid()
{
    pParentFilter->refreshAttachedGrid();
}

} // namespace Antares::Toolbox::Filter::Operator
