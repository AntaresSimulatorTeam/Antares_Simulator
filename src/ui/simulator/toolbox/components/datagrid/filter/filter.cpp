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
#include "filter.h"
#include <antares/date/date.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include "input.h"
#include "component.h"

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
AFilterBase::AFilterBase(Input* parent) :
 operators(this),
 currentOperator(nullptr),
 pDataGridPrecision(Date::stepAny),
 pParentInput(parent),
 pMainSizer(nullptr),
 pSizerParameters(nullptr),
 pLastParent(nullptr)
{
}

AFilterBase::~AFilterBase()
{
    deleteGUI();
    operators.clear();
}

bool AFilterBase::rowIsValid(const int) const
{
    return true;
}

bool AFilterBase::colIsValid(const int) const
{
    return true;
}

bool AFilterBase::cellIsValid(const double) const
{
    return true;
}

bool AFilterBase::cellIsValid(const wxString&) const
{
    return true;
}

void AFilterBase::recreateGUI(wxWindow* parent)
{
    // Make sure there is no component left
    deleteGUI();

    pLastParent = parent;
    pMainSizer = new wxBoxSizer(wxHORIZONTAL);
    pSizerParameters = new wxBoxSizer(wxHORIZONTAL);

    // Choice box, only if the filter have operators (which is not
    // the case for Operator::Any)
    if (not operators.empty())
    {
        auto* ch = new wxChoice(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 22));
        const uint max = operators.size();
        for (uint i = 0; i < max; ++i)
            ch->Insert(operators.at(i)->caption(), i);
        // Adding the wxChoice
        pMainSizer->AddSpacer(10);
        pMainSizer->Add(ch, 0, wxALL | wxEXPAND);

        // Default selection
        if (max)
        {
            ch->SetSelection(0);
            currentOperator = operators.at(0);
        }

        // Event
        ch->Connect(ch->GetId(),
                    wxEVT_COMMAND_CHOICE_SELECTED,
                    wxCommandEventHandler(AFilterBase::onOperatorChanged),
                    nullptr,
                    this);
    }

    pMainSizer->AddSpacer(5);
    pMainSizer->Add(pSizerParameters, 1, wxALL | wxEXPAND);

    refreshGUIOperator();
}

void AFilterBase::refreshGUIOperator()
{
    // Calling the refresh with the current selected operator
    AFilterBase::refreshGUIOperator(currentOperator);
}

void AFilterBase::onOperatorChanged(wxCommandEvent& evt)
{
    currentOperator = operators.at(evt.GetSelection());
    refreshGUIOperator(currentOperator);
    refreshAttachedGrid();
}

void AFilterBase::refreshGUIOperator(Operator::AOperator* op)
{
    // Hide all controls
    wxSizerItemList::iterator end = pSizerParameters->GetChildren().end();
    for (wxSizerItemList::iterator i = pSizerParameters->GetChildren().begin(); i != end; ++i)
        (*i)->Show(false);

    // Creating all needed components required by the operator
    if (op && pLastParent)
    {
        // Attaching our filter to the given sizer
        wxSizer* s = op->sizer(pLastParent);
        if (!pSizerParameters->GetItem(s))
            pSizerParameters->Add(s, 0, wxALL | wxEXPAND);
        // Show our controls
        pSizerParameters->Show(s, true);
        pSizerParameters->Layout();
    }
}

void AFilterBase::deleteGUI()
{
    if (pSizerParameters)
    {
        // Clearing the area
        pSizerParameters->Clear(true);
        pSizerParameters = nullptr;
    }
    if (pMainSizer)
    {
        // Clearing the area
        pMainSizer->Clear(true);
        pMainSizer = nullptr;
    }
}

void AFilterBase::refreshAttachedGrid()
{
    if (pParentInput && pParentInput->component())
        pParentInput->component()->refresh();
}

void AFilterBase::dataGridPrecision(Date::Precision p)
{
    pDataGridPrecision = p;
}

Date::Precision AFilterBase::precision() const
{
    return Date::stepNone;
}

} // namespace Filter
} // namespace Toolbox
} // namespace Antares
