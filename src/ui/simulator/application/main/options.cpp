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

#include "../main.h"
#include "antares/study/study.h"
#include "../../windows/options/temp-folder/temp-folder.h"
#include "../../windows/options/geographic-trimming/geographic-trimming.h"
#include "../../windows/options/select-output/select-output.h"
#include "../../windows/options/playlist/playlist.h"
#include "../../windows/options/optimization/optimization.h"
#include "../../windows/options/adequacy-patch/adequacy-patch-options.h"
#include "../../windows/options/adequacy-patch/adequacy-patch-areas.h"
#include "../../windows/options/advanced/advanced.h"
#include "../../windows/message.h"

namespace Antares
{
namespace Forms
{
void ApplWnd::evtOnOptionsTempFolder(wxCommandEvent&)
{
    Dispatcher::GUI::CreateAndShowModal<Window::Options::ConfigureTempFolder>(this);
}

void ApplWnd::evtOnOptionsSelectOutput(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::SelectOutput>(this);
}

void ApplWnd::evtOnOptionsSelectAreasTrimming(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::areasTrimming>(this);
}

void ApplWnd::evtOnOptionsSelectLinksTrimming(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::linksTrimming>(this);
}

void ApplWnd::evtOnOptionsMCPlaylist(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::MCPlaylist>(this);
}

void ApplWnd::evtOnOptionsDistricts(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        assert(pNotebook);
        pNotebook->select("sets");
    }
}

void ApplWnd::evtOnOptionsMCScenarioBuilder(wxCommandEvent&)
{
    auto study = GetCurrentStudy();
    if (!study)
        return;

    Forms::Disabler<ApplWnd> disabler(*this);

    // Checking the minimum requirements
    if (study->areas.empty())
    {
        // No area, it is meaningless to go further
        Window::Message message(this,
                                wxT("Configure MC Scenario Builder"),
                                wxEmptyString,
                                wxT("The study is empty. Please add one or more areas before using "
                                    "the MC Scenario builder"),
                                "images/misc/warning.png");
        message.add(Window::Message::btnCancel, true);
        message.showModal();
    }
    else
    {
        // Open the MC Scenario Builder
        pSectionNotebook->select(wxT("scenariobuilder"), true);
    }
}

void ApplWnd::evtOnOptionsOptimizationPrefs(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::Optimization>(this);
}

void ApplWnd::evtOnOptionsAdvanced(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::AdvancedParameters>(this);
}

void ApplWnd::evtOnOptionsAdequacyPatchOptions(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::AdequacyPatchOptions>(this);
}

void ApplWnd::evtOnOptionsAdequacyPatchAreas(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
        Dispatcher::GUI::CreateAndShowModal<Window::Options::AdequacyPatchAreas>(this);
}

} // namespace Forms
} // namespace Antares
