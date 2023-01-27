/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "parameter.h"
#include <antares/date.h>
#include "../../../../validator.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include "../operator.h"
#include <antares/logs.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{
Parameter::Parameter(AOperator& parent) :
 pOperator(parent), dataType(DataType::dtNone), pSizer(NULL)
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
 pSizer(NULL)
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
    if (NULL == pSizer)
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
                        NULL,
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
                          NULL,
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
