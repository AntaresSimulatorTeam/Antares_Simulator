// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "internal-data.h"
#include <antares/study/study.h>

namespace Antares::Forms
{
MainFormData::MainFormData(ApplWnd& form):
    wipEnabled(false),
    wipPanel(nullptr),
    pMainForm(form)
{
}

void MainFormData::editCurrentLocation(const wxString& string)
{
    assert(pEditCurrentLocation);
    pEditCurrentLocation->SetItemLabel(wxString(wxT("Current tab : ")) << string);
}

} // namespace Antares::Forms
