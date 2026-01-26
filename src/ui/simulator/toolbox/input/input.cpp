// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "input.h"

namespace Antares::Toolbox::InputSelector
{
AInput::AInput(wxWindow* parent):
    Antares::Component::Panel(parent)
{
}

AInput::~AInput()
{
}

void AInput::updateInnerData(const wxString&)
{
}

void AInput::updateRowContent()
{
}

} // namespace Antares::Toolbox::InputSelector
