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

#include "parameter.h"
#include <antares/date/date.h>
#include "../../../../validator.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include "../operator.h"
#include "wx-wrapper.h"
#include <antares/logs/logs.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{
Parameter::Parameter(AOperator& parent) :
 pOperator(parent), dataType(DataType::dtNone), pSizer(nullptr)
{
}

Parameter::Parameter(const Parameter& copy) :
 wxEvtHandler(),
 pOperator(copy.pOperator),
 caption(copy.caption),
 postCaption(copy.postCaption),
 dataType(copy.dataType),
 defaultValues(copy.defaultValues),
 value(copy.value),
 pSizer(nullptr)
{
}

Parameter::~Parameter()
{
}

Parameter& Parameter::operator=(const Parameter& copy)
{
    caption = copy.caption;
    postCaption = copy.postCaption;
    dataType = copy.dataType;
    defaultValues = copy.defaultValues;
    value = copy.value;
    return *this;
}

wxSizer* Parameter::sizer(wxWindow* parent)
{
    if (nullptr == pSizer)
    {
        pSizer = new wxBoxSizer(wxHORIZONTAL);

        // Adding the caption
        if (!caption.IsEmpty())
            pSizer->Add(
              new wxStaticText(parent, wxID_ANY, caption), 0, wxALIGN_CENTER_VERTICAL | wxALL);

        // Creating controls according the real type
        switch (dataType)
        {
        case DataType::dtList:
        {
            wxChoice* ch = new wxChoice(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 22));
            uint max = (uint)defaultValues.size();
            for (uint i = 0; i < max; ++i)
                ch->Insert(defaultValues[i], i);
            if (max)
                ch->SetSelection(0);
            pSizer->Add(ch, 0, wxALL | wxEXPAND);

            ch->Connect(ch->GetId(),
                        wxEVT_COMMAND_CHOICE_SELECTED,
                        wxCommandEventHandler(Parameter::onListChanged),
                        nullptr,
                        this);
            break;
        }
        default:
        {
            // Default value
            wxString defVal;
            switch (dataType)
            {
            case DataType::dtInt:
                defVal = wxT("0");
                break;
            case DataType::dtFloat:
                defVal = wxT("0.0");
                break;
            default:;
            }
            wxTextCtrl* edit = new wxTextCtrl(parent,
                                              wxID_ANY,
                                              defVal,
                                              wxDefaultPosition,
                                              wxSize(-1, 22),
                                              0,
                                              Toolbox::Validator::Numeric());
            pSizer->Add(edit, 0, wxALL | wxEXPAND);

            edit->Connect(edit->GetId(),
                          wxEVT_COMMAND_TEXT_UPDATED,
                          wxCommandEventHandler(Parameter::onChange),
                          nullptr,
                          this);
        }
        }

        // Post caption
        if (!postCaption.IsEmpty())
            pSizer->Add(
              new wxStaticText(parent, wxID_ANY, postCaption), 0, wxALIGN_CENTER_VERTICAL | wxALL);

        // Relayout
        pSizer->Layout();
    }
    return pSizer;
}

Parameter& Parameter::presetInt()
{
    // logs.debug() << "  preset int";
    caption.clear();
    postCaption.clear();
    dataType = DataType::dtInt;
    return *this;
}

Parameter& Parameter::presetWeekDay()
{
    // logs.debug() << "  preset week day";
    caption.clear();
    postCaption.clear();
    dataType = DataType::dtList;
    for (int i = 0; i != 7; ++i)
        defaultValues.push_back(wxStringFromUTF8(Date::WeekdayToString(i)));
    return *this;
}

Parameter& Parameter::presetHourYear()
{
    // logs.debug() << "  preset hour year";
    caption.clear();
    postCaption.clear();
    dataType = DataType::dtInt;
    return *this;
}

Parameter& Parameter::presetHourDay()
{
    // logs.debug() << "  preset hour day";
    caption.clear();
    postCaption.clear();
    dataType = DataType::dtInt;
    return *this;
}

Parameter& Parameter::presetWeek()
{
    // logs.debug() << "  preset week";
    caption.clear();
    postCaption.clear();
    dataType = DataType::dtInt;
    return *this;
}

Parameter& Parameter::presetModuloAddon()
{
    // logs.debug() << "  preset module addon";
    caption = wxT(" = ");
    dataType = DataType::dtInt;
    return *this;
}

void Parameter::onChange(wxCommandEvent& evt)
{
    value.reset(evt.GetString());
    refreshAttachedGrid();
}

void Parameter::onListChanged(wxCommandEvent& evt)
{
    value.reset(evt.GetSelection());
    refreshAttachedGrid();
}

void Parameter::refreshAttachedGrid()
{
    pOperator.refreshAttachedGrid();
}

} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares
